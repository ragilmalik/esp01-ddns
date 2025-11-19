#!/bin/bash

####################################################################################################
# Cloudflare DNS Record ID Finder
# This script helps you find your DNS record ID which is required for the ESP-01 DDNS configuration
####################################################################################################

echo "=================================="
echo "Cloudflare DNS Record ID Finder"
echo "=================================="
echo ""

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo "Warning: 'jq' is not installed. Output will be raw JSON."
    echo "Install jq for prettier output: sudo apt-get install jq (Debian/Ubuntu) or brew install jq (Mac)"
    echo ""
    USE_JQ=false
else
    USE_JQ=true
fi

# Prompt for API Token
read -p "Enter your Cloudflare API Token: " API_TOKEN
echo ""

# Prompt for Zone ID
read -p "Enter your Zone ID: " ZONE_ID
echo ""

# Optional: Filter by record name
read -p "Enter DNS record name to filter (optional, press Enter to show all): " RECORD_NAME
echo ""

echo "Fetching DNS records..."
echo ""

# Build URL
URL="https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records"

# Add filter if provided
if [ ! -z "$RECORD_NAME" ]; then
    URL="${URL}?name=${RECORD_NAME}"
fi

# Make API request
RESPONSE=$(curl -s -X GET "$URL" \
  -H "Authorization: Bearer $API_TOKEN" \
  -H "Content-Type: application/json")

# Check if jq is available
if [ "$USE_JQ" = true ]; then
    # Pretty print with jq
    echo "DNS Records:"
    echo "============"
    echo "$RESPONSE" | jq -r '.result[] | "ID: \(.id)\nName: \(.name)\nType: \(.type)\nContent: \(.content)\nProxied: \(.proxied)\nTTL: \(.ttl)\n---"'

    # Check for errors
    SUCCESS=$(echo "$RESPONSE" | jq -r '.success')
    if [ "$SUCCESS" != "true" ]; then
        echo ""
        echo "Error occurred:"
        echo "$RESPONSE" | jq -r '.errors[] | "- \(.message)"'
    fi
else
    # Raw JSON output
    echo "$RESPONSE"
fi

echo ""
echo "Copy the 'ID' value of the record you want to update into your config.h file"
echo "as CLOUDFLARE_RECORD_ID"
