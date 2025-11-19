#!/bin/bash

####################################################################################################
# Cloudflare API Test Script
# This script tests your Cloudflare credentials and performs a test DNS update
####################################################################################################

echo "====================================="
echo "Cloudflare API Connection Test"
echo "====================================="
echo ""

# Prompt for credentials
read -p "Enter your Cloudflare API Token: " API_TOKEN
echo ""
read -p "Enter your Zone ID: " ZONE_ID
echo ""
read -p "Enter your DNS Record ID: " RECORD_ID
echo ""

# Step 1: Verify API Token
echo "[1/3] Testing API Token..."
VERIFY_RESPONSE=$(curl -s -X GET "https://api.cloudflare.com/client/v4/user/tokens/verify" \
  -H "Authorization: Bearer $API_TOKEN" \
  -H "Content-Type: application/json")

if command -v jq &> /dev/null; then
    SUCCESS=$(echo "$VERIFY_RESPONSE" | jq -r '.success')
    if [ "$SUCCESS" = "true" ]; then
        echo "✓ API Token is valid!"
        echo "$VERIFY_RESPONSE" | jq -r '.result | "  Status: \(.status)"'
    else
        echo "✗ API Token verification failed!"
        echo "$VERIFY_RESPONSE" | jq -r '.errors[] | "  Error: \(.message)"'
        exit 1
    fi
else
    echo "$VERIFY_RESPONSE"
fi
echo ""

# Step 2: Get current DNS record
echo "[2/3] Fetching current DNS record..."
RECORD_RESPONSE=$(curl -s -X GET "https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records/${RECORD_ID}" \
  -H "Authorization: Bearer $API_TOKEN" \
  -H "Content-Type: application/json")

if command -v jq &> /dev/null; then
    SUCCESS=$(echo "$RECORD_RESPONSE" | jq -r '.success')
    if [ "$SUCCESS" = "true" ]; then
        echo "✓ DNS Record found!"
        echo "$RECORD_RESPONSE" | jq -r '.result | "  Name: \(.name)\n  Type: \(.type)\n  Content: \(.content)\n  TTL: \(.ttl)\n  Proxied: \(.proxied)"'
        CURRENT_IP=$(echo "$RECORD_RESPONSE" | jq -r '.result.content')
        RECORD_NAME=$(echo "$RECORD_RESPONSE" | jq -r '.result.name')
        RECORD_TTL=$(echo "$RECORD_RESPONSE" | jq -r '.result.ttl')
        RECORD_PROXIED=$(echo "$RECORD_RESPONSE" | jq -r '.result.proxied')
    else
        echo "✗ Failed to fetch DNS record!"
        echo "$RECORD_RESPONSE" | jq -r '.errors[] | "  Error: \(.message)"'
        exit 1
    fi
else
    echo "$RECORD_RESPONSE"
    echo ""
    echo "Warning: Install 'jq' for better output formatting"
    exit 0
fi
echo ""

# Step 3: Test update (optional)
echo "[3/3] DNS Update Test"
read -p "Do you want to test updating the DNS record? (y/n): " TEST_UPDATE

if [ "$TEST_UPDATE" = "y" ] || [ "$TEST_UPDATE" = "Y" ]; then
    # Get current public IP
    echo "Getting your current public IP..."
    PUBLIC_IP=$(curl -s http://api.ipify.org)
    echo "Your public IP: $PUBLIC_IP"
    echo ""

    if [ "$PUBLIC_IP" = "$CURRENT_IP" ]; then
        echo "Note: Your public IP matches the current DNS record."
        echo "The update will still be sent as a test."
    fi
    echo ""

    read -p "Update DNS record '$RECORD_NAME' to '$PUBLIC_IP'? (y/n): " CONFIRM

    if [ "$CONFIRM" = "y" ] || [ "$CONFIRM" = "Y" ]; then
        # Perform update
        UPDATE_RESPONSE=$(curl -s -X PATCH "https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records/${RECORD_ID}" \
          -H "Authorization: Bearer $API_TOKEN" \
          -H "Content-Type: application/json" \
          --data "{\"type\":\"A\",\"name\":\"${RECORD_NAME}\",\"content\":\"${PUBLIC_IP}\",\"ttl\":${RECORD_TTL},\"proxied\":${RECORD_PROXIED}}")

        SUCCESS=$(echo "$UPDATE_RESPONSE" | jq -r '.success')
        if [ "$SUCCESS" = "true" ]; then
            echo "✓ DNS record updated successfully!"
            echo "$UPDATE_RESPONSE" | jq -r '.result | "  Name: \(.name)\n  Type: \(.type)\n  Content: \(.content)\n  Modified: \(.modified_on)"'
        else
            echo "✗ Failed to update DNS record!"
            echo "$UPDATE_RESPONSE" | jq -r '.errors[] | "  Error: \(.message)"'
        fi
    else
        echo "Update cancelled."
    fi
else
    echo "Skipping update test."
fi

echo ""
echo "====================================="
echo "Test Complete!"
echo "====================================="
echo ""
echo "If all tests passed, you can now:"
echo "1. Copy these values to your config.h file"
echo "2. Upload the sketch to your ESP-01"
echo "3. Monitor the serial output to verify operation"
