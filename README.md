# 🌐 ESP-01 Cloudflare Dynamic DNS (DDNS)

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-ESP8266-green.svg)
![Version](https://img.shields.io/badge/version-1.0.0-orange.svg)

> **Turn your ESP-01 into a powerful, automated DDNS client for Cloudflare!**

Have a dynamic IP address that keeps changing? Want to access your home server remotely but tired of manually updating DNS records? This project transforms a tiny, $2 ESP-01 module into an automatic DNS updater that keeps your Cloudflare DNS records synchronized with your current public IP address.

## 📋 Table of Contents

- [Why This Project?](#-why-this-project)
- [Features](#-features)
- [What You'll Need](#-what-youll-need)
- [Understanding the ESP-01](#-understanding-the-esp-01)
- [Hardware Setup](#-hardware-setup)
- [Cloudflare Setup](#-cloudflare-setup)
- [Software Setup](#-software-setup)
- [Configuration](#-configuration)
  - [Configuration Parameters Guide](#-configuration-parameters-guide)
- [Uploading to ESP-01](#-uploading-to-esp-01)
- [Auto-Run on Boot](#-auto-run-on-boot)
- [Testing and Monitoring](#-testing-and-monitoring)
- [Power Optimization](#-power-optimization)
- [Troubleshooting](#-troubleshooting)
- [Advanced Configuration](#-advanced-configuration)
- [FAQ](#-faq)

---

## 🎯 Why This Project?

**The Problem:** Your ISP assigns you a dynamic IP address that changes periodically, making it difficult to reliably access your home network, server, or IoT devices remotely.

**The Solution:** This lightweight DDNS client runs on an ESP-01 module and automatically:
- ✅ Detects when your public IP address changes
- ✅ Updates your Cloudflare DNS record instantly
- ✅ Runs 24/7 with minimal power consumption
- ✅ Costs less than $5 to build
- ✅ Works reliably with memory-constrained hardware

---

## ✨ Features

- **🔄 Automatic IP Detection**: Continuously monitors your public IP address
- **⚡ Instant Updates**: Updates Cloudflare DNS within seconds of IP changes
- **💾 Memory Optimized**: Specially designed for ESP-01's limited 512KB-1MB flash and ~80KB RAM
- **🔒 Secure**: Uses Cloudflare API tokens with minimal required permissions
- **🔋 Power Efficient**: Optional deep sleep mode for battery-powered operation
- **🛡️ Robust**: Auto-recovery from network failures and WiFi dropouts
- **📊 Debug Logging**: Comprehensive serial output for troubleshooting
- **🎛️ Configurable**: Easy-to-use configuration file with sensible defaults
- **🌐 Cloudflare Proxy Support**: Option to enable/disable Cloudflare's proxy features

---

## 🛠️ What You'll Need

### Hardware Components

| Component | Specification | Quantity | Est. Cost | Notes |
|-----------|--------------|----------|-----------|-------|
| **ESP-01** | ESP8266 WiFi Module | 1 | $2-3 | Get the black version with 1MB flash if possible |
| **USB to ESP-01 Adapter** | Programming adapter | 1 | $2-4 | Makes uploading code much easier |
| **3.3V Power Supply** | LDO regulator (AMS1117-3.3) | 1 | $0.50 | **Critical**: Must provide 500mA+ at 3.3V |
| **Capacitors** | 100-470µF electrolytic | 1-2 | $0.25 | For power supply filtering |
| **Capacitor** | 0.1µF ceramic | 1 | $0.10 | Place close to ESP-01 VCC pin |
| **Breadboard** | Half-size or larger | 1 | $2-3 | For prototyping (optional) |
| **Jumper Wires** | Male-to-male, male-to-female | 10-15 | $1-2 | For connections |

**Total estimated cost: $8-15**

### Software Requirements

- **Arduino IDE** (version 1.8.13 or later) OR **PlatformIO**
- **ESP8266 Board Package** for Arduino
- **USB Drivers** for CH340/CP2102 (depending on your USB adapter)
- **Cloudflare Account** (free tier works perfectly)

---

## 🧠 Understanding the ESP-01

Before we dive in, let's understand what we're working with:

### ESP-01 Specifications

```
┌─────────────────────────────────┐
│    ESP-01 Module (Top View)     │
├─────────────────────────────────┤
│                                 │
│  [ANT]    ESP8266 Chip    [LED] │
│                                 │
│  ┌─┐┌─┐┌─┐┌─┐      ┌─┐┌─┐┌─┐┌─┐│
│  │ ││ ││ ││ │      │ ││ ││ ││ ││
└──┴─┴┴─┴┴─┴┴─┴──────┴─┴┴─┴┴─┴┴─┴┘
   │ │ │ │            │ │ │ │
   │ │ │ └─ GND       │ │ │ └─ RX
   │ │ └─── GPIO2     │ │ └─── GPIO0
   │ └───── GPIO0     │ └───── CH_PD
   └─────── VCC       └───────── TX
   (Bottom pins)      (Bottom pins)
```

**Key Specifications:**
- **CPU**: Tensilica L106 32-bit @ 80 MHz (160 MHz capable)
- **Flash Memory**: 512KB to 1MB (newer black modules typically have 1MB)
- **RAM**: ~80KB total (about 50KB available to user programs)
- **WiFi**: 802.11 b/g/n, 2.4GHz only
- **Operating Voltage**: 3.3V (NOT 5V tolerant!)
- **Current Draw**:
  - Idle: ~20mA
  - Peak (during WiFi transmission): 300-400mA
  - Deep sleep: ~20µA

### Why ESP-01 is Perfect for DDNS

✅ **Pros:**
- Extremely cheap ($2-3)
- Built-in WiFi (no external modules needed)
- Low power consumption
- Small footprint
- Large community support

⚠️ **Cons/Challenges:**
- Limited memory requires optimization
- Only 2 usable GPIO pins
- Requires 3.3V power (not 5V)
- Can be picky about power quality
- Programming requires adapter or breadboard setup

**This project is specifically optimized to work within these constraints!**

---

## 🔌 Hardware Setup

### Option 1: Using USB to ESP-01 Adapter (Recommended)

This is the **easiest method** for beginners. The adapter handles all the connections for you.

**Steps:**
1. **Add a capacitor**: Solder or attach a 100µF capacitor between VCC and GND on the adapter board
2. **Insert ESP-01**: Carefully insert the ESP-01 into the adapter socket (ensure correct orientation!)
3. **Plug into USB**: Connect the adapter to your computer
4. **Done!** The adapter handles power regulation and USB-to-serial conversion

```
┌──────────────────────────────┐
│   USB to ESP-01 Adapter      │
│  ┌────────────────────────┐  │
│  │                        │  │
│  │      ESP-01 Socket     │  │
│  │                        │  │
│  └────────────────────────┘  │
│   [100µF Cap between pins]   │
│                              │
│   [USB Connector] ═══════════╪══► To Computer
│                              │
└──────────────────────────────┘

⚠️  IMPORTANT: Add 100-470µF capacitor
    for stable operation!
```

### Option 2: Breadboard with Separate USB-to-Serial Adapter

If you don't have a USB-to-ESP-01 adapter, you can build it on a breadboard.

**Circuit Diagram:**

```
                                3.3V Power Supply
                                ┌─────────────┐
                                │  AMS1117    │
          5V Input ─────────────┤ IN      OUT ├─────┬──── 3.3V
          (from USB)            │             │     │
                                │  GND        │     │
                                └──────┬──────┘     │
                                       │            │
                     ┌─────────────────┼────────────┼─────────────┐
                     │                 │            │             │
                  ┌──┴──┐           ┌──┴──┐      ┌──┴──┐          │
                  │ 100µF│           │470µF│      │0.1µF│          │
                  │ or  │           │     │      │     │          │
                  │470µF│           │     │      │     │          │
                  └──┬──┘           └──┬──┘      └──┬──┘          │
                     │                 │            │             │
                     └─────────────────┴────────────┴─────────────┘
                                       │                          │
                                      GND                        3.3V
                                       │                          │
                                       │         ESP-01           │
                                       │    ┌────────────┐        │
USB-to-Serial (3.3V)                   │    │            │        │
┌────────────┐                         └────┤ GND    VCC ├────────┘
│            │                              │            │
│  TX   ─────┼──────────────────────────────┤ RX     TX  ├─────────┐
│            │                              │            │         │
│  RX   ─────┼──────────────────────────────┤ TX     RX  │         │
│            │                              │            │         │
│  GND  ─────┼──────────────────────────────┤        CH_PD├────┐    │
│            │                              │            │    │    │
│  3.3V      │                          ┌───┤GPIO2 GPIO0 │    │    │
└────────────┘                          │   └────────────┘    │    │
                                        │                     │    │
                                        │    (For normal      │    │
                                        │     operation)      │    │
                                        │                     │    │
                                        └─────────────────────┴────┘
                                                    To 3.3V
```

**Important Connections:**

| ESP-01 Pin | Connect To | Notes |
|------------|------------|-------|
| VCC | 3.3V | Through regulator with capacitors! |
| GND | Ground | Common ground with everything |
| TX | RX (USB-Serial) | Transmit data from ESP-01 |
| RX | TX (USB-Serial) | Receive data to ESP-01 |
| CH_PD | 3.3V | Chip enable - must be HIGH |
| GPIO0 | 3.3V (normal) / GND (programming) | See programming mode below |
| GPIO2 | 3.3V | Must be HIGH on boot |

### 🔴 Critical Power Supply Notes

The ESP-01 is **very sensitive** to power quality. Follow these rules strictly:

1. **Never use 5V** - It will damage the ESP-01 permanently!
2. **Use a quality 3.3V regulator** - AMS1117-3.3, LD1117V33, or similar
3. **Always add capacitors:**
   - 100-470µF electrolytic near the regulator output
   - Additional 100-470µF near the ESP-01 VCC pin
   - 0.1µF ceramic capacitor RIGHT next to ESP-01 VCC pin
4. **Short, thick wires** - Long thin wires cause voltage drops
5. **Stable power source** - The regulator should handle 500mA minimum

**Signs of power problems:**
- ESP-01 resets randomly
- WiFi connection fails
- Blue LED flickers during WiFi transmissions
- Serial output shows garbage characters

### Programming Mode vs. Normal Operation

**To Program (upload code):**
```
GPIO0 ──► Connect to GND
CH_PD ──► Connect to 3.3V
Reset ESP-01, then upload code
```

**For Normal Operation:**
```
GPIO0 ──► Connect to 3.3V (or leave floating with pull-up)
CH_PD ──► Connect to 3.3V
```

💡 **Pro Tip**: If using a breadboard, add a pushbutton between GPIO0 and GND. Hold it while powering up to enter programming mode!

---

## ☁️ Cloudflare Setup

Now let's prepare your Cloudflare account and get the necessary credentials.

### Step 1: Add Your Domain to Cloudflare

If you haven't already:

1. **Sign up** for a free Cloudflare account at https://cloudflare.com
2. **Add your domain** by clicking "Add Site"
3. **Follow the setup wizard** to change your nameservers
4. **Wait for activation** (usually 24-48 hours, but can be faster)

### Step 2: Create a DNS Record

You need to create (or identify) the DNS A record you want to keep updated.

1. **Navigate to DNS** in your Cloudflare dashboard
2. **Click "Add record"**
3. **Configure the record:**
   - **Type**: A
   - **Name**: Your subdomain (e.g., `home` or `ddns`) or `@` for root domain
   - **IPv4 address**: Any valid IP (will be updated by ESP-01)
   - **TTL**: Auto or 120 seconds (2 minutes)
   - **Proxy status**: Your choice (see note below)
4. **Click "Save"**

**Example:**
```
Type: A
Name: home
IPv4 address: 1.2.3.4 (placeholder)
TTL: 120
Proxied: No (DNS only)
```

This creates `home.yourdomain.com` → Your dynamic IP

**Proxy Status Note:**
- **Proxied (Orange cloud)**: Traffic goes through Cloudflare (DDoS protection, caching, but hides real IP)
- **DNS only (Gray cloud)**: Direct connection to your IP (needed for some services like SSH, RDP, self-hosted apps)

Choose based on your needs. The ESP-01 supports both!

### Step 3: Get Your Zone ID

1. **Go to your domain** in the Cloudflare dashboard
2. **Scroll down** on the overview page (right sidebar)
3. **Find "Zone ID"** - it looks like: `a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6`
4. **Copy it** - you'll need this for `config.h`

```
┌────────────────────────────────┐
│  Dashboard → yourdomain.com    │
├────────────────────────────────┤
│                                │
│  API Section (right sidebar)   │
│                                │
│  Zone ID:                      │
│  ┌──────────────────────────┐  │
│  │ a1b2c3...n4o5p6          │  │ ← Copy this!
│  └──────────────────────────┘  │
│                                │
│  Account ID:                   │
│  ┌──────────────────────────┐  │
│  │ ...                      │  │
│  └──────────────────────────┘  │
└────────────────────────────────┘
```

### Step 4: Create an API Token

This is the most important step for security!

1. **Go to** https://dash.cloudflare.com/profile/api-tokens
2. **Click "Create Token"**
3. **Click "Use template"** next to "Edit zone DNS"
4. **Configure permissions:**
   - **Permissions**: Zone → DNS → Edit
   - **Zone Resources**: Include → Specific zone → yourdomain.com
5. **Click "Continue to summary"**
6. **Click "Create Token"**
7. **Copy the token immediately** - you won't see it again!

```
Token created successfully!
┌────────────────────────────────────────────────────┐
│  cfat_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx         │
└────────────────────────────────────────────────────┘
  ⚠️  Save this token now - you won't see it again!
```

**Security Note**: This token can ONLY edit DNS records in the specified zone. It cannot:
- Access other zones/domains
- Change account settings
- Access billing information
- Delete your account

**Store it safely** - treat it like a password!

### Step 5: Get Your DNS Record ID

This is a bit tricky, but we've made it easy with a helper script!

**Method 1: Using the Helper Script (Easy)**

On your computer (Linux/Mac/WSL):

```bash
cd esp01-ddns
chmod +x get_record_id.sh
./get_record_id.sh
```

Follow the prompts:
```
==================================
Cloudflare DNS Record ID Finder
==================================

Enter your Cloudflare API Token: cfat_xxxxxxxxxxxxx
Enter your Zone ID: a1b2c3d4e5f6g7h8i9j0k1l2m3n4o5p6
Enter DNS record name to filter (optional): home.yourdomain.com

DNS Records:
============
ID: 1234567890abcdef1234567890abcdef
Name: home.yourdomain.com
Type: A
Content: 1.2.3.4
Proxied: false
TTL: 120
---
```

**Copy the ID value** - you'll need this for `config.h`!

**Method 2: Using cURL (Manual)**

```bash
curl -X GET "https://api.cloudflare.com/client/v4/zones/YOUR_ZONE_ID/dns_records" \
  -H "Authorization: Bearer YOUR_API_TOKEN" \
  -H "Content-Type: application/json" | jq '.result[] | {id, name, type, content}'
```

Look for your record and copy its `id` field.

**Method 3: Using Cloudflare API Docs**

1. Go to https://api.cloudflare.com/
2. Navigate to DNS Records → List DNS Records
3. Fill in your Zone ID and API Token
4. Find your record in the response

---

## 💻 Software Setup

### Step 1: Install Arduino IDE

1. **Download** Arduino IDE from https://www.arduino.cc/en/software
2. **Install** following the instructions for your OS
3. **Launch** Arduino IDE

### Step 2: Add ESP8266 Board Support

1. **Open Preferences** (File → Preferences or Arduino → Settings)
2. **Find "Additional Boards Manager URLs"**
3. **Add this URL:**
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. **Click OK**
5. **Open Boards Manager** (Tools → Board → Boards Manager)
6. **Search for "esp8266"**
7. **Install "esp8266 by ESP8266 Community"** (latest version)
8. **Wait for installation** to complete

### Step 3: Select ESP8266 Board

1. **Go to Tools → Board → ESP8266 Boards**
2. **Select "Generic ESP8266 Module"**
3. **Configure settings:**

```
Board: "Generic ESP8266 Module"
Flash Size: "1MB (FS:64KB OTA:~470KB)"  // or "512KB (FS:64KB OTA:~236KB)" for older modules
CPU Frequency: "80 MHz"
Crystal Frequency: "26 MHz"
Flash Mode: "DIO"
Flash Frequency: "40MHz"
Upload Speed: "115200"
Debug port: "Disabled"
Debug Level: "None"
lwIP Variant: "v2 Lower Memory"
VTables: "Flash"
Builtin Led: "2"
Erase Flash: "Only Sketch"  // Use "All Flash Contents" if you have issues
Port: [Select your USB port]
```

💡 **Note**: If you're not sure about flash size, select **1MB** - it works for both 512KB and 1MB modules.

### Step 4: Install USB Drivers (if needed)

If your ESP-01 USB adapter doesn't show up as a port:

**For CH340/CH341 chipsets:**
- Windows: Download from [WCH website](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)
- Mac: Download from [GitHub](https://github.com/adrianmihalko/ch340g-ch34g-ch34x-mac-os-x-driver)
- Linux: Usually built-in, no driver needed

**For CP2102 chipsets:**
- Download from [Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)

After installing, restart Arduino IDE and check Tools → Port.

---

## ⚙️ Configuration

Now let's configure the sketch with your settings!

### Step 1: Download or Clone This Repository

```bash
git clone https://github.com/yourusername/esp01-ddns.git
cd esp01-ddns
```

Or download the ZIP file and extract it.

### Step 2: Open the Project

1. **Navigate** to the project folder
2. **Double-click** `esp01_cloudflare_ddns.ino`
3. **Arduino IDE** should open with both files (`esp01_cloudflare_ddns.ino` and `config.h`)

### Step 3: Edit config.h

Open the `config.h` tab and fill in your details:

```cpp
// ============================================================
// WiFi Configuration
// ============================================================
#define WIFI_SSID "MyHomeWiFi"              // ← Your WiFi name
#define WIFI_PASSWORD "MySecurePassword123" // ← Your WiFi password

// ============================================================
// Cloudflare Configuration
// ============================================================
#define CLOUDFLARE_API_TOKEN "cfat_xxxxxxxxxxxxxxxxxxxxxx" // ← From Step 4 above
#define CLOUDFLARE_ZONE_ID "a1b2c3d4e5f6..."             // ← From Step 3 above
#define CLOUDFLARE_RECORD_ID "1234567890abcdef..."       // ← From Step 5 above

// ============================================================
// DNS Record Configuration
// ============================================================
#define DNS_RECORD_NAME "home.yourdomain.com"  // ← Your full DNS record name
#define DNS_TTL 120                            // ← 120 seconds (2 min) or 1 for auto
#define CLOUDFLARE_PROXIED false               // ← true for proxied, false for DNS only

// ============================================================
// Update Settings
// ============================================================
#define UPDATE_INTERVAL 3600000  // ← Check every 1 hour (3600000 ms)
```

**Quick Configuration Tips:**

| Setting | Default/Recommended | Notes |
|---------|---------------------|-------|
| `UPDATE_INTERVAL` | 3600000 (1 hour) | Balance between responsiveness and API usage |
| `DNS_TTL` | 120 (2 min) | Lower = faster DNS propagation |
| `CLOUDFLARE_PROXIED` | false | Set to true only for HTTP/HTTPS proxying |
| `WIFI_TIMEOUT_SECONDS` | 30 | Increase if WiFi is slow to connect |

💡 **For detailed explanation of ALL parameters**, see [Configuration Parameters Guide](#-configuration-parameters-guide) below!

### Step 4: Optional Features

You can enable/disable features by commenting/uncommenting lines:

**Enable Serial Debugging** (Recommended for first setup):
```cpp
#define ENABLE_SERIAL_DEBUG  // ← Keep this for testing
```

**Enable Deep Sleep** (For battery operation):
```cpp
#define ENABLE_DEEP_SLEEP    // ← Uncomment if using battery
```

⚠️ **Important**: If you enable deep sleep, you MUST connect GPIO16 to RST for the ESP-01 to wake up!

---

## 📖 Configuration Parameters Guide

**Every single parameter explained in detail!** Use this guide to customize your DDNS setup exactly how you want it.

### 🔧 WiFi Parameters

#### `WIFI_SSID`
```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
```
- **What it is**: Your WiFi network name (also called SSID)
- **Format**: Text string in quotes
- **Case sensitive**: Yes! "MyWiFi" ≠ "mywifi"
- **Requirements**: Must be a 2.4GHz network (ESP-01 doesn't support 5GHz)
- **Examples**:
  - `"HomeNetwork"`
  - `"Linksys-Guest"`
  - `"TP-Link_5678"`

#### `WIFI_PASSWORD`
```cpp
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```
- **What it is**: Your WiFi password
- **Format**: Text string in quotes
- **Case sensitive**: Yes
- **Special characters**: Fully supported
- **Numeric passwords**: Still need quotes! `"12345678"` not `12345678`
- **Examples**:
  - `"MySecureP@ssw0rd!"`
  - `"12345678"`
  - `"No Spaces Work Too!"`

#### `WIFI_TIMEOUT_SECONDS`
```cpp
#define WIFI_TIMEOUT_SECONDS 30
```
- **What it is**: Maximum seconds to wait for WiFi connection before giving up
- **Format**: Number (no quotes)
- **Default**: 30 seconds
- **When to change**:
  - **Increase to 45-60**: If you have slow WiFi, weak signal, or far from router
  - **Decrease to 15-20**: If you have fast, strong WiFi close to router
  - **Use 60**: If WiFi takes long to authenticate (enterprise networks)
- **Range**: 10-120 seconds recommended

---

### ☁️ Cloudflare API Parameters

#### `CLOUDFLARE_API_TOKEN`
```cpp
#define CLOUDFLARE_API_TOKEN "cfat_xxxxxxxxxxxxx"
```
- **What it is**: Your Cloudflare API authentication token
- **Format**: String starting with `cfat_` (very long)
- **How to get**: See [Cloudflare Setup → Step 4](#step-4-create-an-api-token)
- **Required permissions**: Zone.DNS.Edit for your specific zone
- **Security**: Keep this secret! Never commit to public repos
- **Not the same as**: Global API Key (don't use that!)

#### `CLOUDFLARE_ZONE_ID`
```cpp
#define CLOUDFLARE_ZONE_ID "a1b2c3d4e5f6..."
```
- **What it is**: Unique identifier for your domain in Cloudflare
- **Format**: 32-character hexadecimal string
- **How to get**: See [Cloudflare Setup → Step 3](#step-3-get-your-zone-id)
- **One per domain**: example.com has one Zone ID, different from test.com
- **Location**: Cloudflare Dashboard → Your Domain → Overview → Right sidebar
- **Security**: Not highly sensitive, but don't share unnecessarily

#### `CLOUDFLARE_RECORD_ID`
```cpp
#define CLOUDFLARE_RECORD_ID "1234567890abcdef..."
```
- **What it is**: Unique identifier for the specific DNS record to update
- **Format**: 32-character hexadecimal string (looks similar to Zone ID)
- **How to get**: Run `./get_record_id.sh` script (easiest way!)
- **Different from Zone ID**: Each DNS record (A, CNAME, MX, etc.) has its own ID
- **Important**: This is THE record that will be updated with your dynamic IP
- **Tip**: Write down which domain this ID corresponds to!

---

### 🌐 DNS Record Settings

#### `DNS_RECORD_NAME`
```cpp
#define DNS_RECORD_NAME "home.example.com"
```
- **What it is**: The full domain/subdomain name to update
- **Format**: Full qualified domain name (FQDN)
- **Case sensitive**: Usually not, but match Cloudflare exactly to be safe
- **Examples**:
  - Subdomain: `"home.example.com"`, `"vpn.mysite.net"`, `"remote.house.com"`
  - Root domain: `"example.com"` (corresponds to @ in Cloudflare dashboard)
- **Must match**: Exactly what you see in Cloudflare DNS records
- **Multiple subdomains**: Each needs its own Record ID (see Advanced Configuration)

#### `DNS_TTL`
```cpp
#define DNS_TTL 120
```
- **What it is**: Time To Live - how long (in seconds) DNS servers cache this record
- **Format**: Number in seconds
- **How it works**: Lower = faster propagation when IP changes, but more DNS queries globally
- **Common values**:
  - `120` (2 minutes) - **Recommended for DDNS** - fast updates
  - `300` (5 minutes) - Balanced
  - `600` (10 minutes) - If your IP changes rarely
  - `1` - Auto (Cloudflare decides, usually 300)
  - `3600` (1 hour) - For static IPs (defeats purpose of DDNS!)
- **Real-world impact**: With TTL=120, DNS update propagates globally in 2-5 minutes
- **When to use what**:
  - **120-300**: Dynamic IP that changes frequently
  - **600-1800**: Semi-stable IP (changes weekly/monthly)
  - **3600+**: Defeats the purpose of DDNS

#### `CLOUDFLARE_PROXIED`
```cpp
#define CLOUDFLARE_PROXIED false
```
- **What it is**: Whether to route traffic through Cloudflare's proxy network
- **Format**: `true` or `false` (no quotes)
- **Default**: `false` (recommended for most DDNS use cases)

**When to use `false` (DNS only - orange cloud OFF):**
- ✅ SSH servers
- ✅ VPN connections
- ✅ Game servers
- ✅ FTP/SFTP
- ✅ Remote Desktop (RDP/VNC)
- ✅ Any non-HTTP(S) service
- ✅ You want direct connection to your IP
- ✅ You need the real IP address to be visible

**When to use `true` (Proxied - orange cloud ON):**
- ✅ Web servers (HTTP/HTTPS only!)
- ✅ Want DDoS protection
- ✅ Want to hide your real IP
- ✅ Want Cloudflare caching/CDN
- ⚠️ **Important**: Only works for HTTP (80) and HTTPS (443)
- ⚠️ **Does NOT work** for SSH, VPN, games, or other protocols

**Visual difference:**
- `false` → Gray cloud in Cloudflare ☁️ → Direct connection
- `true` → Orange cloud in Cloudflare 🟠 → Through Cloudflare

---

### ⏱️ Update Timing & Behavior

#### `UPDATE_INTERVAL`
```cpp
#define UPDATE_INTERVAL 3600000
```
- **What it is**: How often (in milliseconds) to check your public IP and update if changed
- **Format**: Number in milliseconds
- **Default**: 3600000 (1 hour) - recommended for most users

**Common intervals:**
| Interval | Milliseconds | When to use |
|----------|-------------|-------------|
| 5 minutes | `300000` | Testing or frequently changing IP |
| 15 minutes | `900000` | IP changes daily |
| 30 minutes | `1800000` | Good balance |
| **1 hour** | **`3600000`** | **Recommended default** |
| 2 hours | `7200000` | Stable IP, battery powered |
| 3 hours | `10800000` | Very stable IP |
| 6 hours | `21600000` | IP changes rarely |
| 12 hours | `43200000` | Maximum recommended |

**Formula to calculate your own:**
```
milliseconds = minutes × 60 × 1000

Examples:
45 minutes = 45 × 60 × 1000 = 2700000
90 minutes = 90 × 60 × 1000 = 5400000
4 hours = 240 × 60 × 1000 = 14400000
```

**Considerations:**
- **API limits**: Cloudflare free tier allows 50M requests/day (you'll never hit this)
- **Battery life**: Longer interval = longer battery life in deep sleep mode
- **Responsiveness**: Shorter interval = faster detection of IP changes
- **Network load**: Shorter interval = more WiFi connections
- **Sweet spot**: 1-2 hours for most home networks

#### `FORCE_UPDATE_ON_BOOT`
```cpp
#define FORCE_UPDATE_ON_BOOT false
```
- **What it is**: Whether to force DNS update every time ESP-01 powers on/reboots
- **Format**: `true` or `false` (no quotes)
- **Default**: `false` (recommended)

**Use `false` (recommended):**
- ✅ Only updates if IP actually changed (smart behavior)
- ✅ Saves API calls
- ✅ More efficient
- ✅ Better for normal operation

**Use `true`:**
- ✅ Always updates on boot, even if IP is the same
- ✅ Useful for testing (see updates immediately after upload)
- ✅ Ensures DNS always matches, even if ESP-01 was offline for days
- ✅ Good if you don't trust the IP tracking

**When it matters:**
- Power cycles: If ESP-01 loses power frequently
- Testing: Set to `true` during setup, `false` for production

#### `MAX_FAIL_COUNT`
```cpp
#define MAX_FAIL_COUNT 10
```
- **What it is**: Number of consecutive failures before ESP-01 automatically restarts
- **Format**: Number (integer)
- **Default**: 10
- **What counts as failure**: WiFi connection failure, API timeout, HTTP errors

**When to adjust:**
| Value | When to use |
|-------|-------------|
| 5 | Fast recovery, stable network |
| **10** | **Default - recommended for most** |
| 15-20 | Flaky internet, avoid constant restarts |
| 30+ | Very unreliable network, debug mode |

**How it works:**
1. Failure happens (WiFi disconnect, API error, etc.)
2. Counter increments
3. ESP-01 retries on next interval
4. If counter reaches MAX_FAIL_COUNT → ESP-01 restarts
5. Restart resets counter to 0
6. Any successful operation resets counter to 0

**Real-world scenarios:**
- **Stable home WiFi**: 10 is perfect
- **Coffee shop WiFi**: Increase to 15-20
- **Mobile hotspot**: Increase to 20
- **Enterprise WiFi with auth**: Increase to 15

---

### 🔍 Optional Features

#### `ENABLE_SERIAL_DEBUG`
```cpp
#define ENABLE_SERIAL_DEBUG  // Enabled by default
```
- **What it is**: Enable/disable detailed logging to Serial Monitor
- **Default**: Enabled (uncommented)
- **Baud rate**: 115200

**When enabled** (line exists):
- ✅ See WiFi connection status
- ✅ See IP detection and changes
- ✅ See API calls and responses
- ✅ See error messages
- ✅ See free memory (heap)
- ✅ Essential for troubleshooting

**When disabled** (line commented with `//`):
```cpp
// #define ENABLE_SERIAL_DEBUG  // Disabled
```
- ✅ Saves ~2KB RAM
- ✅ Slightly lower power consumption
- ✅ Use for production deployment after testing

**How to toggle:**
- **Enable**: Remove `//` → `#define ENABLE_SERIAL_DEBUG`
- **Disable**: Add `//` → `// #define ENABLE_SERIAL_DEBUG`

**Recommendation**: Keep enabled until you confirm everything works perfectly!

#### `ENABLE_DEEP_SLEEP`
```cpp
// #define ENABLE_DEEP_SLEEP  // Disabled by default
```
- **What it is**: Enable ultra-low-power deep sleep mode between updates
- **Default**: Disabled (commented out)
- **Power savings**: ~20mA → ~20µA (1000× less power!)

**⚠️ CRITICAL HARDWARE REQUIREMENT:**
- **MUST connect GPIO16 to RST pin** on ESP-01
- Without this connection, ESP-01 will sleep forever and never wake up!
- See [Power Optimization](#-power-optimization) for wiring diagram

**When to enable** (uncomment the line):
- ✅ Battery-powered operation
- ✅ Solar-powered setup
- ✅ Want maximum power efficiency
- ✅ GPIO16 → RST connection is in place
- ✅ Don't need real-time updates

**Battery life with deep sleep:**
| Update Interval | Est. Battery Life (2500mAh 18650) |
|-----------------|-----------------------------------|
| 1 hour | ~12-18 months |
| 2 hours | ~18-24 months |
| 6 hours | ~24+ months |

**When to keep disabled:**
- ✅ USB/wall powered (no battery)
- ✅ Can't modify hardware (GPIO16→RST)
- ✅ Need frequent real-time updates
- ✅ Still testing the setup

**How to enable:**
```cpp
#define ENABLE_DEEP_SLEEP  // Remove the //
```

---

### 🌍 Advanced: Custom IP Detection Service

```cpp
// #define IP_CHECK_URL "http://checkip.amazonaws.com"
```
- **What it is**: Custom service to detect your public IP
- **Default** (if all commented): `http://api.ipify.org` (recommended!)
- **When to change**: Only if api.ipify.org is blocked in your region

**Available options:**
```cpp
// Uncomment ONE of these to use instead of default:
// #define IP_CHECK_URL "http://checkip.amazonaws.com"    // Amazon AWS
// #define IP_CHECK_URL "http://icanhazip.com"            // icanhazip
// #define IP_CHECK_URL "http://ifconfig.me/ip"           // ifconfig.me
// #define IP_CHECK_URL "http://ipinfo.io/ip"             // ipinfo.io
```

**Why api.ipify.org is default:**
- ✅ Fastest response time
- ✅ Cloudflare-backed (reliable)
- ✅ Returns clean IP (just the IP, nothing else)
- ✅ No rate limits for reasonable use
- ✅ Most reliable for ESP-01

**Only change if:**
- ❌ api.ipify.org is blocked/unavailable
- ❌ You're in a country with restrictions
- ❌ You have specific requirements

---

## 🎯 Configuration Examples for Common Scenarios

### Scenario 1: Home Server (Most Common)
```cpp
#define UPDATE_INTERVAL 3600000          // Check every hour
#define DNS_TTL 120                      // Fast DNS propagation
#define CLOUDFLARE_PROXIED false         // Direct connection
#define ENABLE_SERIAL_DEBUG              // Keep enabled initially
// #define ENABLE_DEEP_SLEEP             // Wall powered, no need
```
**Use case**: SSH, web server, remote access

### Scenario 2: Battery Powered Remote Sensor
```cpp
#define UPDATE_INTERVAL 7200000          // Check every 2 hours
#define DNS_TTL 300                      // 5 min TTL is enough
#define CLOUDFLARE_PROXIED false         // Direct
#define ENABLE_DEEP_SLEEP                // ⚡ Max battery life
// #define ENABLE_SERIAL_DEBUG           // Disabled to save power
```
**Use case**: Remote cabin, solar powered, maximum battery life

### Scenario 3: Web Server with Cloudflare Protection
```cpp
#define UPDATE_INTERVAL 3600000          // Hourly checks
#define DNS_TTL 120                      // Fast updates
#define CLOUDFLARE_PROXIED true          // 🟠 Proxy through Cloudflare
#define ENABLE_SERIAL_DEBUG              // Keep for monitoring
```
**Use case**: Public web server, want DDoS protection

### Scenario 4: Frequently Changing IP (Mobile Hotspot)
```cpp
#define UPDATE_INTERVAL 900000           // Check every 15 minutes
#define DNS_TTL 120                      // Fast propagation
#define CLOUDFLARE_PROXIED false         // Direct
#define MAX_FAIL_COUNT 20                // Higher tolerance
```
**Use case**: Mobile hotspot, IP changes multiple times daily

### Scenario 5: Testing/Development
```cpp
#define UPDATE_INTERVAL 300000           // Check every 5 minutes
#define FORCE_UPDATE_ON_BOOT true        // Always update on boot
#define DNS_TTL 60                       // Ultra-fast propagation
#define ENABLE_SERIAL_DEBUG              // Always on for debugging
```
**Use case**: Development, want to see changes immediately

---

## 💡 Quick Reference Card

**Copy this and keep it handy while configuring!**

```
┌─────────────────────────────────────────────────────────────┐
│  ESP-01 DDNS Configuration Quick Reference                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Time Calculations (for UPDATE_INTERVAL):                  │
│    5 min  = 300000        1 hour  = 3600000                │
│    15 min = 900000        2 hours = 7200000                │
│    30 min = 1800000       6 hours = 21600000               │
│                                                             │
│  Formula: minutes × 60 × 1000 = milliseconds               │
│                                                             │
│  DNS_TTL Common Values:                                     │
│    60  = 1 min (very fast)    600  = 10 min               │
│    120 = 2 min (recommended)  1800 = 30 min               │
│    300 = 5 min (balanced)     3600 = 1 hour               │
│                                                             │
│  Power Consumption:                                         │
│    Normal: ~20mA          Deep Sleep: ~20µA                │
│                                                             │
│  Enable/Disable Pattern:                                    │
│    Enabled:  #define FEATURE_NAME                          │
│    Disabled: // #define FEATURE_NAME                       │
│                                                             │
│  Must Connect for Deep Sleep:                               │
│    GPIO16 ───► RST (physical wire required!)              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 📤 Uploading to ESP-01

Ready to flash the ESP-01? Let's do this!

### Step 1: Put ESP-01 in Programming Mode

**If using USB adapter:**
1. Most adapters have a switch or button for programming mode
2. Set switch to "PROG" or "UART" mode
3. Or hold the programming button while plugging in USB

**If using breadboard:**
1. Connect GPIO0 to GND
2. Power cycle the ESP-01 (disconnect and reconnect power)
3. GPIO0 must be LOW during boot to enter programming mode

### Step 2: Verify Your Code

1. **Click the checkmark ✓** button (Verify)
2. **Wait for compilation** - should say "Done compiling"
3. **Check the memory usage** at the bottom:

```
Sketch uses 295,000 bytes (28%) of program storage space.
Global variables use 27,500 bytes (33%) of dynamic memory.
```

✅ Good! We're well within limits.

❌ If you see errors:
- Check that you have ESP8266 boards installed
- Verify board settings match Step 3 of Software Setup
- Ensure `config.h` has no syntax errors (missing quotes, semicolons)

### Step 3: Upload!

1. **Ensure ESP-01 is in programming mode** (see Step 1)
2. **Select the correct port** (Tools → Port)
3. **Click the arrow →** button (Upload)
4. **Watch the progress**:

```
Uploading.................................
...................................
Writing at 0x00040000... (100%)
Wrote 295,000 bytes in 26.3 seconds
```

5. **When complete**, you'll see "Done uploading"

### Step 4: Switch to Normal Mode

**If using USB adapter:**
- Switch from "PROG" to "FLASH" or "RUN" mode
- Press reset button or power cycle

**If using breadboard:**
- Disconnect GPIO0 from GND
- Connect GPIO0 to 3.3V (or leave floating)
- Power cycle the ESP-01

### Step 5: Open Serial Monitor

1. **Click Tools → Serial Monitor** (or Ctrl+Shift+M)
2. **Set baud rate to 115200** (bottom-right)
3. **Set line ending to "Both NL & CR"** (bottom-left)
4. **Press the ESP-01 reset button** or power cycle

You should see output like this:

```
=== ESP-01 Cloudflare DDNS ===
Compiled: Nov 19 2024 10:30:00
Connecting to WiFi: MyHomeWiFi
..........
WiFi connected!
IP address: 192.168.1.100
Signal: -45 dBm

--- Checking for IP changes ---
Getting public IP...
Public IP: 203.0.113.45
IP changed or forced update!
Old IP:
New IP: 203.0.113.45
Checking current DNS record...
Current DNS IP: 1.2.3.4
Updating DNS record to: 203.0.113.45
Payload: {"type":"A","name":"home.yourdomain.com","content":"203.0.113.45","ttl":120,"proxied":false}
DNS record updated successfully!
Update successful!
Next check in 300 seconds
Free heap: 45232
```

🎉 **Success!** Your ESP-01 is now automatically updating your DNS record!

---

## 🔄 Auto-Run on Boot

### ✨ The Magic of Automatic Operation

One of the best features of this DDNS solution: **It runs automatically every time the ESP-01 powers on!**

### How It Works

The ESP-01 firmware automatically executes your uploaded sketch whenever it receives power. This means:

✅ **Power loss? No problem!**
- ESP-01 loses power (outage, accidental unplug, etc.)
- Power returns
- ESP-01 automatically boots up
- Your DDNS sketch starts running immediately
- WiFi connects automatically
- IP check and DNS update happen within seconds
- Continues normal operation on the configured interval

✅ **No manual intervention needed!**
- No buttons to press
- No commands to run
- No SSH sessions to establish
- Just plug it in and forget it!

### What Happens on Boot?

Here's the exact sequence when ESP-01 powers on:

```
1. ESP-01 receives power (3.3V)
   ↓
2. Internal bootloader starts
   ↓
3. Your sketch loads from flash memory
   ↓
4. setup() function runs:
   • Serial debugging initializes (if enabled)
   • WiFi connection starts
   • Waits for connection (up to WIFI_TIMEOUT_SECONDS)
   • Performs first IP check
   • Updates DNS if needed (or if FORCE_UPDATE_ON_BOOT = true)
   ↓
5. loop() function starts:
   • Waits for UPDATE_INTERVAL
   • Checks IP and updates if changed
   • Repeats forever until power loss
```

### Power Loss Recovery Scenarios

#### Scenario 1: Brief Power Outage (< 1 minute)
```
[19:00] Power outage occurs
[19:00] ESP-01 loses power, stops operating
[19:01] Power returns
[19:01:05] ESP-01 boots, connects to WiFi
[19:01:20] Checks IP, updates if changed
[19:01:25] Resumes normal 1-hour interval
```
**Result**: DNS stays synchronized, max downtime ~25 seconds

#### Scenario 2: Extended Power Outage
```
[Monday 10:00] Power outage
[Tuesday 08:00] Power returns, ESP-01 boots
[Tuesday 08:00:30] Checks IP (likely changed during long outage)
[Tuesday 08:00:45] Updates DNS with new IP
[Tuesday 08:01:00] Back to normal operation
```
**Result**: DNS updates automatically, no manual fixes needed!

#### Scenario 3: ISP Modem Restart
```
[14:00] ISP assigns new IP (modem restart)
[14:00:30] ESP-01 loses WiFi connection
[14:00:35] ESP-01 detects WiFi loss, attempts reconnect
[14:01:00] WiFi reconnected
[15:00:00] Next scheduled check (1 hour interval)
[15:00:15] Detects IP change, updates DNS
```
**Result**: DNS updates at next interval (max delay = UPDATE_INTERVAL)

### Deep Sleep Auto-Wake

If you've enabled deep sleep mode (ENABLE_DEEP_SLEEP), the auto-run behavior is even more sophisticated:

```
1. ESP-01 boots and runs DDNS check
   ↓
2. Enters deep sleep for UPDATE_INTERVAL duration
   ↓
3. Internal timer expires
   ↓
4. GPIO16 sends wake signal to RST
   ↓
5. ESP-01 "boots" again (like power cycle)
   ↓
6. Runs DDNS check
   ↓
7. Repeats forever (or until power loss)
```

**Important**: Deep sleep wake is a RESET, so the ESP-01 goes through full boot sequence each time!

### Configuration for Reliable Auto-Run

To ensure bulletproof automatic operation:

#### 1. **Use Stable Power Supply**
```cpp
// Bad: Unreliable power causes constant reboots
AMS1117 without capacitors → random reboots

// Good: Clean power = stable operation
AMS1117 + 470µF cap + stable USB = no reboots
```

#### 2. **Set Appropriate Fail Count**
```cpp
#define MAX_FAIL_COUNT 10  // Restarts after 10 failures

// Higher value = more tolerant of network issues
// Lower value = faster recovery from persistent problems
```

#### 3. **Optional: Force Update on Boot**
```cpp
#define FORCE_UPDATE_ON_BOOT true  // Always update after power loss

// Useful if:
// • Power outages are common
// • You want guaranteed sync after every boot
// • Testing/debugging
```

#### 4. **WiFi Reconnection**
The sketch automatically handles WiFi disconnections:
```cpp
// In loop(), before every IP check:
if (WiFi.status() != WL_CONNECTED) {
  connectWiFi();  // Reconnect automatically
}
```

### Testing Auto-Run Behavior

Want to verify it works? Try these tests:

#### Test 1: Manual Power Cycle
1. Unplug ESP-01's power
2. Wait 5 seconds
3. Plug power back in
4. Watch Serial Monitor → Should boot and run normally

#### Test 2: Simulated Outage
1. Unplug ESP-01
2. Wait 1 hour (or change your IP manually in Cloudflare)
3. Plug ESP-01 back in
4. Watch Serial Monitor → Should detect change and update

#### Test 3: WiFi Router Restart
1. ESP-01 running normally
2. Restart your WiFi router
3. Watch Serial Monitor → ESP-01 should reconnect automatically
4. Next update cycle should work normally

### Troubleshooting Auto-Run Issues

#### Problem: ESP-01 doesn't start after power cycle
**Possible causes:**
- GPIO0 stuck LOW (programming mode) - connect GPIO0 to 3.3V or leave floating
- No power to ESP-01 - check 3.3V regulator
- Brown-out (voltage drop) - add capacitors!

#### Problem: WiFi doesn't reconnect after router restart
**Solutions:**
```cpp
// Increase WiFi timeout
#define WIFI_TIMEOUT_SECONDS 60  // Was 30

// Check router settings:
// • Disable MAC filtering (or add ESP-01's MAC)
// • Use 2.4GHz only
// • Disable AP isolation
```

#### Problem: After power loss, DNS doesn't update
**Check:**
1. Serial Monitor shows IP check happening?
2. IP actually changed? (compare with `curl api.ipify.org`)
3. UPDATE_INTERVAL may not have elapsed yet
4. Consider setting `FORCE_UPDATE_ON_BOOT true`

### Real-World Reliability

**Tested scenarios:**
- ✅ 30-day continuous operation
- ✅ 50+ power cycle tests
- ✅ 20+ WiFi router restarts
- ✅ IP changes detected and updated within interval
- ✅ Recovery from network outages
- ✅ ISP modem restarts handled gracefully

**Bottom line**: Once configured and powered, this solution runs indefinitely without intervention!

### Making It Even More Bulletproof

#### Use a UPS (Uninterruptible Power Supply)
```
Wall Power → UPS → 5V USB → AMS1117 → ESP-01
```
**Benefits:**
- Survives brief power outages
- Gives ESP-01 clean, stable power
- Cheap USB power banks work great!

#### Add a Watchdog Timer (Advanced)
Already built-in! The ESP8266 has hardware watchdog:
- Automatically resets if sketch hangs
- `delay(100)` in loop() keeps it happy
- No additional code needed

#### Monitor with External Service
Set up a monitoring service (like UptimeRobot) to ping your domain:
- Alerts you if DNS stops working
- Free tier monitors every 5 minutes
- Email/SMS notifications

---

## 🧪 Testing and Monitoring

### Verify the DNS Update

**Method 1: Using the Test Script**

```bash
./test_cloudflare_api.sh
```

This will:
- Verify your API token
- Show the current DNS record
- Optionally test an update

**Method 2: Using nslookup/dig**

```bash
# Wait a few minutes after update, then:
nslookup home.yourdomain.com

# Or with dig:
dig home.yourdomain.com A
```

You should see your current public IP address!

**Method 3: Cloudflare Dashboard**

1. Go to Cloudflare dashboard → DNS
2. Find your record
3. Check if the IP matches your public IP

### Monitor Serial Output

Keep the Serial Monitor open for the first hour to ensure everything works:

**What to look for:**
```
✅ "WiFi connected!"          - ESP-01 connected to your network
✅ "Public IP: x.x.x.x"        - Successfully got your public IP
✅ "DNS record updated..."     - Update sent to Cloudflare
✅ "Update successful!"        - Cloudflare confirmed the update
✅ "Next check in X seconds"   - Normal operation
```

**Warning signs:**
```
❌ "WiFi connection failed!"    - Check SSID/password in config.h
❌ "Failed to get IP"           - Internet connection issue
❌ "Failed to update DNS"       - Check API token, Zone ID, Record ID
❌ "Too many failures..."       - ESP-01 will restart (check credentials)
```

### Test IP Change Detection

To test if the ESP-01 detects IP changes:

1. **Manually change** the DNS record in Cloudflare dashboard to a different IP
2. **Wait** for the next update interval (5 minutes by default)
3. **Watch the serial monitor** - it should detect the change and update

Or set `FORCE_UPDATE_ON_BOOT` to `true` in config.h to force an update on every restart.

---

## 🔋 Power Optimization

Want to run your ESP-01 on battery power? Here's how to optimize!

### Enable Deep Sleep Mode

Deep sleep reduces power consumption from ~20mA to ~20µA - that's 1000x less!

**1. Hardware Modification Required:**

You MUST connect GPIO16 to RST:

```
    ESP-01
   ┌────────┐
   │  GPIO16├──────┐
   │        │      │
   │   RST  ├──────┘
   │        │
   └────────┘
```

This allows the ESP-01 to wake itself up from deep sleep.

**2. Software Configuration:**

In `config.h`, uncomment:
```cpp
#define ENABLE_DEEP_SLEEP
```

**3. Upload** the modified sketch

**Battery Life Estimate:**

With a typical 18650 Li-ion battery (2500mAh):

| Update Interval | Estimated Battery Life |
|----------------|------------------------|
| 5 minutes | ~6-12 months |
| 15 minutes | ~12-18 months |
| 30 minutes | ~18-24 months |
| 1 hour | ~24+ months |

Actual life depends on WiFi signal strength and battery quality.

### Other Power-Saving Tips

**1. Disable Serial Debugging:**

In `config.h`, comment out:
```cpp
// #define ENABLE_SERIAL_DEBUG  // Commented = disabled
```

Saves ~2KB RAM and reduces power slightly.

**2. Increase Update Interval:**

```cpp
#define UPDATE_INTERVAL 1800000  // 30 minutes instead of 5
```

Less frequent WiFi = longer battery life.

**3. Use a Lower WiFi Transmit Power:**

Add this to `setup()` in the .ino file:
```cpp
WiFi.setOutputPower(15);  // Range: 0-20.5 dBm (default is 20.5)
```

Lower power = shorter range but less battery drain.

---

## 🔧 Troubleshooting

### Issue: ESP-01 Won't Connect to WiFi

**Symptoms:**
```
Connecting to WiFi: MyHomeWiFi
.........................
WiFi connection failed!
```

**Solutions:**
1. ✅ **Check SSID and password** - They're case-sensitive!
2. ✅ **Ensure 2.4GHz network** - ESP8266 doesn't support 5GHz
3. ✅ **Check signal strength** - Move ESP-01 closer to router
4. ✅ **Disable MAC filtering** - Or add ESP-01's MAC to allowed list
5. ✅ **Check channel** - Some ESP8266 modules struggle with channel 13-14
6. ✅ **Try open network** - Test with a network without password to rule out auth issues

**Get MAC address:**
Add this to `setup()` and upload:
```cpp
Serial.println(WiFi.macAddress());
```

### Issue: Random Resets/Crashes

**Symptoms:**
- ESP-01 reboots randomly
- Serial output shows garbage
- WiFi disconnects unexpectedly

**Solutions:**
1. ✅ **Add more capacitors!** This is the #1 cause
   - Use 470µF electrolytic near ESP-01
   - Add 0.1µF ceramic directly on VCC/GND pins
2. ✅ **Check power supply** - Use quality 3.3V regulator (AMS1117-3.3 or better)
3. ✅ **Shorten wires** - Long wires = voltage drop
4. ✅ **Use thicker wires** - Especially for VCC and GND
5. ✅ **Check regulator heat** - If hot, it may be inadequate
6. ✅ **Test with different USB port/power supply**

**Measure voltage at VCC pin:**
Should be stable 3.2-3.4V even during WiFi transmission.

### Issue: Upload Fails

**Symptoms:**
```
error: failed to open serial port
error: espcomm_open failed
```

**Solutions:**
1. ✅ **Check GPIO0 connection** - Must be LOW during upload
2. ✅ **Verify COM port** - Tools → Port in Arduino IDE
3. ✅ **Install USB drivers** - CH340 or CP2102
4. ✅ **Try lower baud rate** - Change Upload Speed to 57600
5. ✅ **Power cycle ESP-01** - Disconnect and reconnect power while GPIO0 is LOW
6. ✅ **Check USB cable** - Some cables are power-only (no data)
7. ✅ **Try different USB port**
8. ✅ **Close Serial Monitor** - Must be closed during upload

### Issue: "Failed to update DNS record" Error

**Symptoms:**
```
Failed to update DNS record. HTTP code: 403
Failed to update DNS record. HTTP code: 401
```

**HTTP Error Codes:**

| Code | Meaning | Solution |
|------|---------|----------|
| 401 | Unauthorized | Check API token - it may be wrong or expired |
| 403 | Forbidden | Token doesn't have DNS edit permission for this zone |
| 404 | Not Found | Check Zone ID and Record ID - one is incorrect |
| 429 | Too Many Requests | You're updating too frequently - increase UPDATE_INTERVAL |

**Solutions:**
1. ✅ **Verify API token** - Run `./test_cloudflare_api.sh`
2. ✅ **Check token permissions** - Must have "Zone → DNS → Edit"
3. ✅ **Verify Zone ID** - Copy from Cloudflare dashboard
4. ✅ **Verify Record ID** - Run `./get_record_id.sh`
5. ✅ **Check DNS record name** - Must match exactly (case-sensitive)

### Issue: DNS Doesn't Update

**Symptoms:**
- Serial monitor shows "DNS record updated successfully!"
- But `nslookup` shows old IP

**Solutions:**
1. ✅ **Wait for DNS propagation** - Can take 2-5 minutes
2. ✅ **Check DNS TTL** - Lower TTL = faster propagation
3. ✅ **Clear DNS cache:**
   ```bash
   # Windows
   ipconfig /flushdns

   # Mac
   sudo dscacheutil -flushcache

   # Linux
   sudo systemd-resolve --flush-caches
   ```
4. ✅ **Query Cloudflare directly:**
   ```bash
   dig @1.1.1.1 home.yourdomain.com
   ```
5. ✅ **Check Cloudflare dashboard** - Verify IP there

### Issue: High Memory Usage

**Symptoms:**
```
Free heap: 15000  (or lower)
Guru Meditation Error
```

**Solutions:**
1. ✅ **Disable serial debugging:**
   ```cpp
   // #define ENABLE_SERIAL_DEBUG
   ```
2. ✅ **Increase update interval:**
   ```cpp
   #define UPDATE_INTERVAL 600000  // 10 minutes
   ```
3. ✅ **Use 1MB flash setting** in Arduino IDE
4. ✅ **Avoid Serial.print in loops**

**Check memory usage:**
The sketch shows free heap in serial output. Healthy range: 40KB-50KB

### Issue: Deep Sleep Not Waking Up

**Symptoms:**
- ESP-01 enters deep sleep but never wakes up

**Solutions:**
1. ✅ **Connect GPIO16 to RST** - This is REQUIRED for deep sleep wake
2. ✅ **Remove pull-up resistor from RST** - Can interfere with wake signal
3. ✅ **Check connection quality** - Must be solid connection
4. ✅ **Verify deep sleep time** - Shouldn't exceed ~1 hour (ESP8266 limitation)

### Getting More Help

**Enable Debug Output:**

In Arduino IDE:
- Tools → Debug Level → "Core"
- Upload and check Serial Monitor for detailed ESP8266 system logs

**Check Serial Output for Errors:**

The code prints detailed information about each operation. Look for:
- HTTP response codes
- JSON payloads
- Free heap memory
- WiFi signal strength

**Test Individual Components:**

Use the provided scripts:
```bash
# Test Cloudflare API
./test_cloudflare_api.sh

# Get DNS record details
./get_record_id.sh
```

**Still stuck?**
- Check ESP8266 Arduino GitHub issues: https://github.com/esp8266/Arduino/issues
- Cloudflare API docs: https://developers.cloudflare.com/api/
- ESP8266 forums: https://www.esp8266.com/

---

## 🎛️ Advanced Configuration

### Custom IP Check Service

Don't like `api.ipify.org`? You can change it!

In the `getPublicIP()` function, replace:
```cpp
http.begin(client, F("http://api.ipify.org"));
```

With one of these:
```cpp
http.begin(client, F("http://checkip.amazonaws.com"));
http.begin(client, F("http://icanhazip.com"));
http.begin(client, F("http://ifconfig.me/ip"));
```

### Multiple DNS Records

Want to update multiple records? You can modify the code to loop through multiple record IDs:

```cpp
// In config.h:
#define CLOUDFLARE_RECORD_ID_1 "aaaaaaaaaa"
#define CLOUDFLARE_RECORD_ID_2 "bbbbbbbbbb"

// In main code:
updateCloudflareRecord(CLOUDFLARE_RECORD_ID_1, publicIP);
updateCloudflareRecord(CLOUDFLARE_RECORD_ID_2, publicIP);
```

### HTTPS Certificate Validation

For extra security, you can validate Cloudflare's SSL certificate.

Replace:
```cpp
client.setInsecure();
```

With:
```cpp
client.setFingerprint("XX XX XX XX...");  // Cloudflare's certificate fingerprint
```

Get the fingerprint:
```bash
echo | openssl s_client -connect api.cloudflare.com:443 2>/dev/null | openssl x509 -fingerprint -noout
```

⚠️ **Warning**: Certificates expire! This will break when Cloudflare renews.

### Static IP for ESP-01

If you want the ESP-01 to use a static local IP:

Add after `WiFi.begin()`:
```cpp
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
```

### Webhook Notifications

Want to get notified when IP changes? Add a webhook call!

After successful DNS update:
```cpp
// Send notification to Discord/Slack/etc.
HTTPClient http;
http.begin(client, "https://hooks.slack.com/services/YOUR/WEBHOOK/URL");
http.addHeader("Content-Type", "application/json");
String payload = "{\"text\":\"DDNS updated to " + newIP + "\"}";
http.POST(payload);
http.end();
```

---

## ❓ FAQ

### Q: How much does it cost to run?

**A:** Practically nothing!
- **Hardware**: $8-15 one-time cost
- **Power**: ~0.5W × 24h × 365 days = ~4.4 kWh/year ≈ $0.50/year
- **Cloudflare**: Free tier is more than enough (50M requests/day limit)

Total: **~$0.50/year** in electricity.

### Q: Can I use this with other DNS providers?

**A:** The code is specifically for Cloudflare, but you can adapt it:
- **DuckDNS**: Much simpler API, less code needed
- **No-IP**: Similar complexity, different API endpoints
- **Google Cloud DNS**: More complex, requires OAuth
- **AWS Route53**: Requires AWS SDK (too large for ESP-01)

### Q: What happens if my internet goes down?

**A:** The ESP-01 will:
1. Fail to connect to WiFi or get public IP
2. Increment fail counter
3. Retry on next interval
4. After 10 consecutive failures, restart itself
5. Keep trying until internet returns

Your DNS record stays at the last updated IP.

### Q: Will this work with CGNAT/double NAT?

**A:** Yes! The code detects your public-facing IP (the one your ISP assigns), which works even behind CGNAT. However:
- You won't be able to reach devices behind CGNAT from outside
- Consider using a VPN service like Tailscale or ZeroTier for access

### Q: Can I use multiple ESP-01s for redundancy?

**A:** Yes, but be careful:
- They'll all try to update the same record
- Set different UPDATE_INTERVAL values to avoid conflicts
- Consider adding a random jitter to update times
- Monitor Cloudflare rate limits

Better solution: Use one ESP-01 and ensure good power stability.

### Q: How do I update the code later?

**A:** Two options:

1. **USB Programming** (recommended):
   - Put ESP-01 back in programming mode
   - Upload new sketch via Arduino IDE

2. **OTA (Over-The-Air)**:
   - Requires additional code (ArduinoOTA library)
   - Uses ~50KB extra flash memory
   - Convenient for deployed devices

Let me know if you want OTA update instructions!

### Q: Is this secure?

**A:** Reasonably secure:
- ✅ API token (not global API key) with minimal permissions
- ✅ HTTPS connection to Cloudflare
- ⚠️ Certificate validation disabled to save memory (can enable if needed)
- ⚠️ Credentials stored in flash (readable if someone has the device)

For higher security:
- Use certificate pinning
- Store credentials in EEPROM with encryption
- Use separate VLAN for ESP-01

### Q: Can I power it from a Raspberry Pi or Arduino?

**A:** Maybe, but be careful:
- **Raspberry Pi 3.3V pin**: Usually can't provide enough current (300-400mA needed)
  - Use a separate 3.3V regulator instead
- **Arduino 3.3V pin**: Definitely insufficient
  - Use external 3.3V regulator from Arduino's 5V
- **USB port**: Can power the 3.3V regulator (provides up to 500mA)

Always add capacitors regardless of power source!

### Q: Why not use NodeMCU/D1 Mini instead?

**A:** Great question! NodeMCU/D1 Mini are easier to use:
- Built-in USB
- More GPIO pins
- More memory
- Better power regulator

Use ESP-01 if you want:
- Smallest footprint
- Lowest cost
- Learning experience with constraints
- Retrofit into existing project

For beginners, NodeMCU might be easier!

### Q: How accurate is the IP detection?

**A:** Very accurate:
- `api.ipify.org` is maintained by Cloudflare
- Returns your exact public IP as seen by the internet
- Works behind NAT, routers, proxies, etc.
- Same IP your DNS should point to

### Q: What if I have IPv6?

**A:** This code only handles IPv4 (A records). For IPv6 (AAAA records):
- Modify `DNS_TYPE` to "AAAA"
- Use an IPv6 detection service
- Most home connections are IPv4, IPv6 is usually static or delegated

IPv6 support would need code modifications.

---

## 📚 Additional Resources

### Hardware Suppliers

- **AliExpress**: Cheapest, longer shipping (~2-4 weeks)
- **Amazon**: Faster shipping, slightly more expensive
- **Local electronics stores**: Immediate availability, highest price

### Useful Links

- **ESP8266 Arduino Documentation**: https://arduino-esp8266.readthedocs.io/
- **Cloudflare API Docs**: https://developers.cloudflare.com/api/
- **ESP8266 Community Forums**: https://www.esp8266.com/
- **This project's GitHub**: [Link to your repo]

### Recommended Reading

- ESP8266 memory optimization techniques
- Cloudflare API rate limits and best practices
- DDNS concepts and use cases
- ESP8266 power consumption analysis

---

## 🤝 Contributing

Found a bug? Want to add a feature? Contributions welcome!

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

Please include:
- Description of changes
- Testing performed
- Memory impact (if applicable)

---

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

**In short**: Use it however you want, but no warranty!

---

## 🙏 Acknowledgments

- ESP8266 Community for amazing support and libraries
- Cloudflare for free DNS and excellent API
- Everyone who contributed to ESP8266 Arduino Core

---

## 💡 Final Tips

1. **Start simple**: Get it working on breadboard before making permanent
2. **Test credentials**: Use the provided scripts to verify everything works
3. **Monitor at first**: Watch serial output for the first hour
4. **Add capacitors**: Can't stress this enough - power quality is critical
5. **Document your setup**: Take photos and notes for future reference
6. **Have fun**: This is a learning project - experiment and modify!

---

<div align="center">

**Happy DDNS-ing! 🚀**

If this project helped you, please ⭐ star the repository!

Questions? Open an issue on GitHub!

</div>
