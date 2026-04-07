#!/bin/bash
# install.sh - LuckJingle L81/A80/A80H Linux Driver Installer
set -e

echo "============================================"
echo " LuckJingle L81/A80/A80H Linux Driver"
echo " Installer"
echo "============================================"
echo ""

# Check for root
if [ "$(id -u)" -eq 0 ]; then
    SUDO=""
else
    SUDO="sudo"
fi

# Check for CUPS
if ! command -v cupsd &> /dev/null && ! systemctl is-active --quiet cups 2>/dev/null; then
    echo "WARNING: CUPS does not appear to be installed or running."
    echo "Install CUPS first: sudo apt install cups"
    echo ""
fi

# Check for build dependencies
MISSING=""

if ! command -v gcc &> /dev/null; then
    MISSING="$MISSING build-essential"
fi

if ! command -v cups-config &> /dev/null; then
    MISSING="$MISSING libcups2-dev"
fi

# Check for cupsimage header
if [ ! -f /usr/include/cups/raster.h ] && [ ! -f /usr/include/cupsimage.h ]; then
    MISSING="$MISSING libcupsimage2-dev"
fi

if [ -n "$MISSING" ]; then
    echo "ERROR: Missing build dependencies:$MISSING"
    echo ""
    echo "Install them with:"
    echo "  sudo apt install$MISSING"
    echo ""
    echo "On Fedora/RHEL:"
    echo "  sudo dnf install gcc make cups-devel"
    echo ""
    echo "On Arch Linux:"
    echo "  sudo pacman -S base-devel cups"
    echo ""
    exit 1
fi

# Build
echo "[1/3] Building filter..."
make clean
make
echo "      Build successful."
echo ""

# Install
echo "[2/3] Installing driver..."
$SUDO make install
echo "      Filter installed to $(cups-config --serverbin)/filter/"
echo "      PPDs installed to /usr/share/cups/model/luckjingle/"
echo ""

# Restart CUPS
echo "[3/3] Restarting CUPS..."
if command -v systemctl &> /dev/null; then
    $SUDO systemctl restart cups
elif command -v service &> /dev/null; then
    $SUDO service cups restart
else
    echo "WARNING: Could not restart CUPS automatically."
    echo "Please restart CUPS manually."
fi
echo ""

echo "============================================"
echo " Installation complete!"
echo "============================================"
echo ""
echo "To add your printer:"
echo ""
echo "  Option 1: CUPS Web Interface"
echo "    Open http://localhost:631 in your browser"
echo "    Go to Administration > Add Printer"
echo "    Select your USB printer"
echo "    Choose 'LuckJingle' as manufacturer"
echo "    Select your model (A81, A80, or A80H)"
echo ""
echo "  Option 2: Command Line"
echo "    # List available USB printers:"
echo "    lpinfo -v | grep usb"
echo ""
echo "    # Add the printer (replace URI with your printer's):"
echo "    sudo lpadmin -p L81 -E \\"
echo "      -v usb://Unknown/Printer \\"
echo "      -P /usr/share/cups/model/luckjingle/a81-printer.ppd"
echo ""
echo "    # Set as default:"
echo "    sudo lpadmin -d L81"
echo ""
echo "    # Test print:"
echo "    echo 'Hello World' | lpr -P L81"
echo ""
