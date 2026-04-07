#!/bin/bash
# uninstall.sh - LuckJingle L81/A80/A80H Linux Driver Uninstaller
set -e

echo "============================================"
echo " LuckJingle L81/A80/A80H Linux Driver"
echo " Uninstaller"
echo "============================================"
echo ""

# Check for root
if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

# Remove any configured printer queues using this driver
echo "[1/3] Removing printer queues..."
for printer in $(lpstat -p 2>/dev/null | awk '{print $2}'); do
    ppd_file="/etc/cups/ppd/${printer}.ppd"
    if [ -f "$ppd_file" ] && grep -q "rastertolabel-l81" "$ppd_file" 2>/dev/null; then
        echo "      Removing printer queue: $printer"
        $SUDO lpadmin -x "$printer" 2>/dev/null || true
    fi
done
echo ""

# Uninstall files
echo "[2/3] Removing driver files..."
$SUDO make uninstall 2>/dev/null || {
    # Manual removal if make uninstall fails
    $SUDO rm -f /usr/lib/cups/filter/rastertolabel-l81
    $SUDO rm -f /usr/share/cups/model/luckjingle/a81-printer.ppd
    $SUDO rm -f /usr/share/cups/model/luckjingle/a80-printer.ppd
    $SUDO rm -f /usr/share/cups/model/luckjingle/a80h-printer.ppd
    $SUDO rmdir /usr/share/cups/model/luckjingle 2>/dev/null || true
}
echo "      Driver files removed."
echo ""

# Restart CUPS
echo "[3/3] Restarting CUPS..."
if command -v systemctl &> /dev/null; then
    $SUDO systemctl restart cups
elif command -v service &> /dev/null; then
    $SUDO service cups restart
fi
echo ""

echo "============================================"
echo " Uninstallation complete."
echo "============================================"
