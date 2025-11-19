/**
 * ESP-01 Cloudflare Dynamic DNS Updater
 *
 * This sketch is optimized for ESP-01's limited memory (512KB-1MB flash, ~80KB RAM)
 * It periodically checks your public IP and updates a Cloudflare DNS A record
 *
 * Memory optimization techniques used:
 * - F() macro for all string literals to store in flash
 * - Minimal libraries
 * - Efficient string handling
 * - Deep sleep between updates
 *
 * Author: Claude
 * Version: 1.0.0
 * License: MIT
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "config.h"

// Global variables (minimized to save RAM)
String currentIP = "";
unsigned long lastUpdate = 0;
int failCount = 0;

void setup() {
  // Initialize serial for debugging (can be disabled to save memory)
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.begin(115200);
  delay(100);
  Serial.println(F("\n\n=== ESP-01 Cloudflare DDNS ==="));
  Serial.print(F("Compiled: "));
  Serial.println(F(__DATE__ " " __TIME__));
  #endif

  // Connect to WiFi
  connectWiFi();

  // Initial IP check and update
  checkAndUpdateIP();
}

void loop() {
  // Check if it's time for an update
  if (millis() - lastUpdate >= UPDATE_INTERVAL) {
    checkAndUpdateIP();
  }

  // Small delay to prevent watchdog timer reset
  delay(100);

  // Optional: Use deep sleep to save power
  #ifdef ENABLE_DEEP_SLEEP
  deepSleepForUpdate();
  #endif
}

/**
 * Connect to WiFi network
 */
void connectWiFi() {
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.print(F("Connecting to WiFi: "));
  Serial.println(WIFI_SSID);
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < WIFI_TIMEOUT_SECONDS) {
    delay(1000);
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.print(".");
    #endif
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("\nWiFi connected!"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.print(F("Signal: "));
    Serial.print(WiFi.RSSI());
    Serial.println(F(" dBm"));
    #endif
  } else {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("\nWiFi connection failed!"));
    #endif
    // Restart ESP if WiFi fails
    delay(5000);
    ESP.restart();
  }
}

/**
 * Get current public IP address
 */
String getPublicIP() {
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.println(F("Getting public IP..."));
  #endif

  WiFiClient client;
  HTTPClient http;

  // Use lightweight IP check service
  http.begin(client, F("http://api.ipify.org"));

  int httpCode = http.GET();
  String ip = "";

  if (httpCode == HTTP_CODE_OK) {
    ip = http.getString();
    ip.trim();
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.print(F("Public IP: "));
    Serial.println(ip);
    #endif
  } else {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.print(F("Failed to get IP. HTTP code: "));
    Serial.println(httpCode);
    #endif
  }

  http.end();
  return ip;
}

/**
 * Get current DNS record IP from Cloudflare
 */
String getCurrentDNSRecord() {
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.println(F("Checking current DNS record..."));
  #endif

  WiFiClientSecure client;
  client.setInsecure(); // Skip certificate validation to save memory

  HTTPClient http;

  // Build URL
  String url = String(F("https://api.cloudflare.com/client/v4/zones/")) +
               CLOUDFLARE_ZONE_ID + F("/dns_records/") + CLOUDFLARE_RECORD_ID;

  http.begin(client, url);
  http.addHeader(F("Authorization"), String(F("Bearer ")) + CLOUDFLARE_API_TOKEN);
  http.addHeader(F("Content-Type"), F("application/json"));

  int httpCode = http.GET();
  String recordIP = "";

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();

    // Simple JSON parsing to extract IP (to avoid ArduinoJson library and save memory)
    int contentPos = payload.indexOf(F("\"content\":\""));
    if (contentPos != -1) {
      int startPos = contentPos + 11; // Length of "content":""
      int endPos = payload.indexOf("\"", startPos);
      recordIP = payload.substring(startPos, endPos);

      #ifdef ENABLE_SERIAL_DEBUG
      Serial.print(F("Current DNS IP: "));
      Serial.println(recordIP);
      #endif
    }
  } else {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.print(F("Failed to get DNS record. HTTP code: "));
    Serial.println(httpCode);
    #endif
  }

  http.end();
  return recordIP;
}

/**
 * Update Cloudflare DNS record
 */
bool updateCloudflareRecord(String newIP) {
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.print(F("Updating DNS record to: "));
  Serial.println(newIP);
  #endif

  WiFiClientSecure client;
  client.setInsecure(); // Skip certificate validation to save memory

  HTTPClient http;

  // Build URL
  String url = String(F("https://api.cloudflare.com/client/v4/zones/")) +
               CLOUDFLARE_ZONE_ID + F("/dns_records/") + CLOUDFLARE_RECORD_ID;

  http.begin(client, url);
  http.addHeader(F("Authorization"), String(F("Bearer ")) + CLOUDFLARE_API_TOKEN);
  http.addHeader(F("Content-Type"), F("application/json"));

  // Build JSON payload (manual construction to save memory)
  String payload = String(F("{\"type\":\"A\",\"name\":\"")) + DNS_RECORD_NAME +
                   F("\",\"content\":\"") + newIP +
                   F("\",\"ttl\":")) + String(DNS_TTL) +
                   F(",\"proxied\":") + (CLOUDFLARE_PROXIED ? F("true") : F("false")) + F("}");

  #ifdef ENABLE_SERIAL_DEBUG
  Serial.print(F("Payload: "));
  Serial.println(payload);
  #endif

  int httpCode = http.PATCH(payload);
  bool success = false;

  if (httpCode == HTTP_CODE_OK) {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("DNS record updated successfully!"));
    String response = http.getString();
    Serial.println(response);
    #endif
    success = true;
    failCount = 0;
  } else {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.print(F("Failed to update DNS record. HTTP code: "));
    Serial.println(httpCode);
    Serial.println(http.getString());
    #endif
    failCount++;
  }

  http.end();
  return success;
}

/**
 * Main function to check IP and update if needed
 */
void checkAndUpdateIP() {
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.println(F("\n--- Checking for IP changes ---"));
  #endif

  // Ensure WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Get current public IP
  String publicIP = getPublicIP();

  if (publicIP.length() == 0) {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("Failed to get public IP. Retrying later..."));
    #endif
    lastUpdate = millis();
    return;
  }

  // Check if IP has changed
  if (publicIP != currentIP || FORCE_UPDATE_ON_BOOT) {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("IP changed or forced update!"));
    Serial.print(F("Old IP: "));
    Serial.println(currentIP);
    Serial.print(F("New IP: "));
    Serial.println(publicIP);
    #endif

    // Verify current DNS record
    String dnsIP = getCurrentDNSRecord();

    // Only update if different from DNS record
    if (publicIP != dnsIP) {
      if (updateCloudflareRecord(publicIP)) {
        currentIP = publicIP;

        #ifdef ENABLE_SERIAL_DEBUG
        Serial.println(F("Update successful!"));
        #endif
      }
    } else {
      #ifdef ENABLE_SERIAL_DEBUG
      Serial.println(F("DNS already up to date. No update needed."));
      #endif
      currentIP = publicIP;
    }
  } else {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("No IP change detected."));
    #endif
  }

  // Check if too many failures
  if (failCount >= MAX_FAIL_COUNT) {
    #ifdef ENABLE_SERIAL_DEBUG
    Serial.println(F("Too many failures. Restarting..."));
    #endif
    delay(1000);
    ESP.restart();
  }

  lastUpdate = millis();

  #ifdef ENABLE_SERIAL_DEBUG
  Serial.print(F("Next check in "));
  Serial.print(UPDATE_INTERVAL / 1000);
  Serial.println(F(" seconds"));
  Serial.print(F("Free heap: "));
  Serial.println(ESP.getFreeHeap());
  #endif
}

/**
 * Enter deep sleep mode (optional power saving)
 * Note: Requires GPIO16 connected to RST for wake-up
 */
#ifdef ENABLE_DEEP_SLEEP
void deepSleepForUpdate() {
  #ifdef ENABLE_SERIAL_DEBUG
  Serial.print(F("Entering deep sleep for "));
  Serial.print(UPDATE_INTERVAL / 1000);
  Serial.println(F(" seconds..."));
  Serial.flush();
  #endif

  // Convert milliseconds to microseconds
  ESP.deepSleep(UPDATE_INTERVAL * 1000);
}
#endif
