/**
 * Configuration file for ESP-01 Cloudflare DDNS
 *
 * IMPORTANT: Copy this file and rename to config.h before uploading
 * Fill in your credentials below
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// WiFi Configuration
// ============================================================
#define WIFI_SSID "YOUR_WIFI_SSID"           // Your WiFi network name
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"   // Your WiFi password
#define WIFI_TIMEOUT_SECONDS 30              // Max seconds to wait for WiFi connection

// ============================================================
// Cloudflare Configuration
// ============================================================
// Get these from your Cloudflare dashboard:
// 1. Zone ID: Dashboard -> Domain -> Overview (right sidebar)
// 2. API Token: Profile -> API Tokens -> Create Token
//    Required permissions: Zone.DNS (Edit)
// 3. Record ID: Use the get_record_id.sh script or check via API

#define CLOUDFLARE_API_TOKEN "YOUR_CLOUDFLARE_API_TOKEN"  // Your API token (NOT API Key)
#define CLOUDFLARE_ZONE_ID "YOUR_ZONE_ID"                 // Your zone/domain ID
#define CLOUDFLARE_RECORD_ID "YOUR_DNS_RECORD_ID"         // The specific DNS record ID to update

// ============================================================
// DNS Record Configuration
// ============================================================
#define DNS_RECORD_NAME "ddns.example.com"   // The full DNS record name (e.g., home.example.com or just example.com)
#define DNS_TTL 120                          // Time to live in seconds (120 = 2 min, 1 = auto)
#define CLOUDFLARE_PROXIED false             // true = proxy through Cloudflare, false = DNS only

// ============================================================
// Update Settings
// ============================================================
#define UPDATE_INTERVAL 300000               // Check interval in milliseconds (300000 = 5 minutes)
#define FORCE_UPDATE_ON_BOOT false           // true = always update on boot, false = only if IP changed
#define MAX_FAIL_COUNT 10                    // Restart ESP after this many consecutive failures

// ============================================================
// Optional Features (comment out to disable and save memory)
// ============================================================
#define ENABLE_SERIAL_DEBUG                  // Enable serial output for debugging (comment out to save ~2KB)
// #define ENABLE_DEEP_SLEEP                 // Enable deep sleep between updates (saves power, requires GPIO16->RST connection)

// ============================================================
// Advanced Settings (usually don't need to change)
// ============================================================
// If you want to use a different IP checking service:
// #define IP_CHECK_URL "http://checkip.amazonaws.com"
// #define IP_CHECK_URL "http://icanhazip.com"
// Default is http://api.ipify.org (fastest and most reliable for ESP-01)

#endif // CONFIG_H
