/**
 * Configuration file for ESP-01 Cloudflare DDNS
 *
 * ╔══════════════════════════════════════════════════════════════════════════╗
 * ║  EDIT THIS FILE WITH YOUR SETTINGS BEFORE UPLOADING TO ESP-01            ║
 * ║  All parameters are explained in detail below                            ║
 * ║  See README.md "Configuration Parameters Guide" for full documentation   ║
 * ╚══════════════════════════════════════════════════════════════════════════╝
 */

#ifndef CONFIG_H
#define CONFIG_H

// ════════════════════════════════════════════════════════════════════════════
// WiFi Configuration - Connect to your wireless network
// ════════════════════════════════════════════════════════════════════════════

#define WIFI_SSID "YOUR_WIFI_SSID"
// ► Your WiFi network name (SSID)
// ► Must be 2.4GHz network (ESP-01 doesn't support 5GHz)
// ► Case-sensitive! "MyWiFi" is different from "mywifi"
// ► Example: "HomeNetwork" or "Linksys-5G"

#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
// ► Your WiFi password
// ► Case-sensitive and special characters are supported
// ► Use quotes even if password is numeric: "12345678"
// ► Example: "MySecureP@ssw0rd!"

#define WIFI_TIMEOUT_SECONDS 30
// ► Maximum time to wait for WiFi connection before giving up
// ► Default: 30 seconds (recommended)
// ► Increase to 45-60 if you have slow WiFi or weak signal
// ► Decrease to 15-20 if you have fast, strong WiFi

// ════════════════════════════════════════════════════════════════════════════
// Cloudflare API Configuration - Your Cloudflare account credentials
// ════════════════════════════════════════════════════════════════════════════
//
// HOW TO GET THESE VALUES:
// 1. Zone ID: Cloudflare Dashboard → Select your domain → Overview →
//             Right sidebar under "API" section
// 2. API Token: Cloudflare Dashboard → Profile → API Tokens → Create Token →
//               Use "Edit zone DNS" template
// 3. Record ID: Run the included script: ./get_record_id.sh
//               Or see README.md "Step 5: Get Your DNS Record ID"

#define CLOUDFLARE_API_TOKEN "YOUR_CLOUDFLARE_API_TOKEN"
// ► Your Cloudflare API Token (starts with "cfat_...")
// ► NOT the same as API Key! Use Token for better security
// ► Must have "Zone.DNS.Edit" permission for your domain
// ► Example: "cfat_abc123XYZ789..." (actual tokens are much longer)
// ► Keep this secret! Don't share or commit to public repos

#define CLOUDFLARE_ZONE_ID "YOUR_ZONE_ID"
// ► Your Cloudflare Zone ID (32-character hexadecimal)
// ► One Zone ID per domain (example.com has one Zone ID)
// ► Example: "a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6"
// ► This is NOT sensitive, but keep it organized

#define CLOUDFLARE_RECORD_ID "YOUR_DNS_RECORD_ID"
// ► The ID of the specific DNS A record to update
// ► Different from Zone ID - each DNS record has its own ID
// ► 32-character hexadecimal like Zone ID
// ► Run ./get_record_id.sh to find this easily
// ► Example: "1234567890abcdef1234567890abcdef"

// ════════════════════════════════════════════════════════════════════════════
// DNS Record Settings - What DNS record to update and how
// ════════════════════════════════════════════════════════════════════════════

#define DNS_RECORD_NAME "ddns.example.com"
// ► The FULL domain name of your DNS record
// ► For subdomain: "home.example.com" or "vpn.example.com"
// ► For root domain: "example.com" (use @ in Cloudflare dashboard)
// ► Must match EXACTLY what's in Cloudflare (case-sensitive)
// ► Examples: "ddns.mysite.com", "remote.myhouse.net", "mysite.com"

#define DNS_TTL 120
// ► Time To Live - how long DNS servers cache this record (in seconds)
// ► 120 = 2 minutes (good for dynamic IPs - fast updates)
// ► 300 = 5 minutes (balanced)
// ► 600 = 10 minutes (if IP rarely changes)
// ► 1 = Auto (Cloudflare decides, usually 300)
// ► Lower = faster DNS propagation but more DNS queries
// ► Recommended: 120 for DDNS, 300-600 for stable IPs

#define CLOUDFLARE_PROXIED false
// ► Whether to proxy traffic through Cloudflare's network
// ► false = DNS only (orange cloud OFF) - shows your real IP
// ►         Use for: SSH, VPN, game servers, direct connections
// ► true = Proxied (orange cloud ON) - hides your IP behind Cloudflare
// ►        Use for: Web servers (HTTP/HTTPS only), DDoS protection
// ► Note: Proxied only works for HTTP(S) traffic on standard ports
// ► Most DDNS users want: false (DNS only)

// ════════════════════════════════════════════════════════════════════════════
// Update Interval & Behavior - How often to check and update
// ════════════════════════════════════════════════════════════════════════════

#define UPDATE_INTERVAL 3600000
// ► How often to check your IP and update Cloudflare (in milliseconds)
// ► 3600000 ms = 1 hour (DEFAULT - good balance)
// ►
// ► Common values:
// ►   • 1800000  = 30 minutes (check every half hour)
// ►   • 3600000  = 1 hour     (recommended for most users)
// ►   • 7200000  = 2 hours    (if IP rarely changes)
// ►   • 10800000 = 3 hours    (for very stable IPs)
// ►   • 300000   = 5 minutes  (frequent checks, more API calls)
// ►   • 900000   = 15 minutes (balanced)
// ►
// ► Formula: (minutes × 60 × 1000) = milliseconds
// ►   Example: 2 hours = 2 × 60 × 60 × 1000 = 7200000
// ►
// ► Cloudflare free tier: 50M requests/day (way more than you'll ever use)
// ► Battery life: Longer interval = longer battery (if using deep sleep)

#define FORCE_UPDATE_ON_BOOT false
// ► Whether to force DNS update every time ESP-01 boots up
// ► false = Only update if IP has actually changed (DEFAULT - recommended)
// ►         Saves API calls and is more efficient
// ► true = Always update on boot, even if IP hasn't changed
// ►        Useful for testing or if you want to ensure sync

#define MAX_FAIL_COUNT 10
// ► How many consecutive failures before ESP-01 restarts itself
// ► 10 = Default (recommended)
// ► Increase to 15-20 if you have flaky internet
// ► Decrease to 5 if you want faster recovery
// ► Failures include: WiFi connection failures, API errors, timeout

// ════════════════════════════════════════════════════════════════════════════
// Optional Features - Enable/disable to customize behavior
// ════════════════════════════════════════════════════════════════════════════

#define ENABLE_SERIAL_DEBUG
// ► Enable detailed logging to Serial Monitor (115200 baud)
// ► ENABLED by default for setup and troubleshooting
// ► Shows: WiFi connection, IP changes, API responses, errors, memory usage
// ► To DISABLE: Comment out like this: // #define ENABLE_SERIAL_DEBUG
// ► Disabling saves ~2KB RAM and slightly reduces power consumption
// ► Leave enabled until you confirm everything works perfectly!

// #define ENABLE_DEEP_SLEEP
// ► Enable deep sleep mode between updates (DISABLED by default)
// ► Reduces power from ~20mA to ~20µA (1000x less!)
// ► Perfect for battery-powered operation (6-24 months on 18650 battery)
// ►
// ► ⚠️  HARDWARE REQUIREMENT: Must connect GPIO16 to RST pin!
// ►    Without this connection, ESP-01 will sleep forever and never wake up
// ►
// ► To ENABLE: Uncomment like this: #define ENABLE_DEEP_SLEEP
// ► Only enable if:
// ►   1. You need battery operation
// ►   2. You've connected GPIO16 → RST (see README hardware section)
// ►   3. You don't need real-time updates (updates only happen after wake)

// ════════════════════════════════════════════════════════════════════════════
// Advanced Settings - Usually don't need to change these
// ════════════════════════════════════════════════════════════════════════════

// Uncomment ONE of these lines to use a different IP checking service:
// #define IP_CHECK_URL "http://checkip.amazonaws.com"    // Amazon AWS
// #define IP_CHECK_URL "http://icanhazip.com"            // icanhazip
// #define IP_CHECK_URL "http://ifconfig.me/ip"           // ifconfig.me
// #define IP_CHECK_URL "http://ipinfo.io/ip"             // ipinfo.io
//
// Default (if none selected): http://api.ipify.org
// ► api.ipify.org is recommended - fastest, most reliable, Cloudflare-backed
// ► Only change if api.ipify.org is blocked or unavailable in your region

// ════════════════════════════════════════════════════════════════════════════
// ⚡ QUICK START CHECKLIST
// ════════════════════════════════════════════════════════════════════════════
//
// Before uploading to ESP-01, make sure you've set:
//
//  ☐ WIFI_SSID - Your WiFi network name
//  ☐ WIFI_PASSWORD - Your WiFi password
//  ☐ CLOUDFLARE_API_TOKEN - From Cloudflare dashboard
//  ☐ CLOUDFLARE_ZONE_ID - From Cloudflare dashboard
//  ☐ CLOUDFLARE_RECORD_ID - Run ./get_record_id.sh to get this
//  ☐ DNS_RECORD_NAME - Your full domain/subdomain name
//  ☐ UPDATE_INTERVAL - How often to check (default: 1 hour)
//
// Optional but recommended:
//  ☐ Test credentials first: Run ./test_cloudflare_api.sh
//  ☐ Keep ENABLE_SERIAL_DEBUG enabled for first run
//  ☐ Set DNS_TTL to 120 for faster updates
//
// See README.md for complete setup instructions!
// ════════════════════════════════════════════════════════════════════════════

#endif // CONFIG_H
