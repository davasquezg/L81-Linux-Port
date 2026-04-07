#!/bin/bash
# install.sh - LuckJingle L81/A80/A80H Linux Driver Installer
# Soporta CUPS deb y snap / Supports deb and snap CUPS
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

# ───────────────────────────────────────
# Detectar CUPS snap vs deb
# ───────────────────────────────────────
CUPS_MODE="unknown"

if snap list cups 2>/dev/null | grep -q cups; then
    CUPS_MODE="snap"
elif dpkg -l cups 2>/dev/null | grep -q "^ii"; then
    CUPS_MODE="deb"
fi

if [ "$CUPS_MODE" = "snap" ]; then
    echo "╔═══════════════════════════════════════════════════════════════╗"
    echo "║  CUPS detectado como SNAP (solo lectura)                    ║"
    echo "║                                                             ║"
    echo "║  El snap de CUPS no permite instalar filtros/PPDs           ║"
    echo "║  directamente en sus directorios.                           ║"
    echo "║                                                             ║"
    echo "║  Selecciona una opción de instalación:                      ║"
    echo "║                                                             ║"
    echo "║  A) LPrint snap (RECOMENDADO)                               ║"
    echo "║     Cero compilación, nativo snap, soporta ZPL/TSPL        ║"
    echo "║                                                             ║"
    echo "║  B) Compilar e instalar en rutas clásicas                   ║"
    echo "║     Para uso con legacy-printer-app                         ║"
    echo "║                                                             ║"
    echo "║  C) Prueba rápida con CUPS snap + PPD                       ║"
    echo "║     Usa rastertolabel del snap con PPD ZPL                  ║"
    echo "╚═══════════════════════════════════════════════════════════════╝"
    echo ""
    echo -n "  Opción [A/b/c]: "
    read -r SNAP_CHOICE
    SNAP_CHOICE=${SNAP_CHOICE:-A}

    case "${SNAP_CHOICE^^}" in
        A)
            echo ""
            echo "[1/4] Instalando LPrint snap..."
            $SUDO snap install lprint 2>/dev/null || echo "  (ya instalado o error)"
            echo ""
            echo "[2/4] Conectando interfaces..."
            $SUDO snap connect lprint:raw-usb 2>/dev/null || true
            $SUDO snap connect lprint:avahi-control 2>/dev/null || true
            # nota: lprint no tiene plug cups-control
            echo ""
            echo "[3/4] Iniciando servidor LPrint..."
            $SUDO snap start lprint.lprint-server 2>/dev/null || true
            sleep 2
            echo ""
            echo "[4/4] Detectando dispositivos..."
            echo ""
            echo "  Dispositivos detectados:"
            snap run lprint devices 2>/dev/null | sed 's/^/    /' || echo "    (ninguno — conecta la impresora por USB)"
            echo ""
            echo "  Drivers ZPL/TSPL disponibles:"
            snap run lprint drivers 2>/dev/null | grep -iE "zpl|tspl" | sed 's/^/    /' || echo "    (no se pudo listar)"
            echo ""
            echo "============================================"
            echo " LPrint instalado correctamente"
            echo "============================================"
            echo ""
            echo "  Para agregar la impresora:"
            echo "    lprint add -d L81 -v 'usb://...' -m zpl_4inch-203dpi-dt"
            echo ""
            echo "  Para imprimir:"
            echo "    lprint submit -d L81 archivo.png"
            echo ""
            echo "  Web UI: http://localhost:8000"
            echo ""
            echo "  Si ZPL no funciona, probar TSPL:"
            echo "    lprint add -d L81-tspl -v 'usb://...' -m tspl_4inch-203dpi-dt"
            echo ""
            exit 0
            ;;
        B)
            echo ""
            echo "  Compilando e instalando en rutas clásicas..."
            echo "  Nota: necesitarás legacy-printer-app para que CUPS snap use estos archivos."
            echo ""
            # Continúa con la instalación clásica abajo
            ;;
        C)
            echo ""
            echo "  Ejecutando prueba rápida..."
            SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
            exec $SUDO bash "${SCRIPT_DIR}/quick-test.sh"
            ;;
        *)
            echo "  Opción no válida. Saliendo."
            exit 1
            ;;
    esac
fi

# ───────────────────────────────────────
# Verificar CUPS (para modo deb o snap opción B)
# ───────────────────────────────────────
if [ "$CUPS_MODE" = "unknown" ]; then
    if ! command -v cupsd &> /dev/null && ! systemctl is-active --quiet cups 2>/dev/null; then
        echo "WARNING: CUPS does not appear to be installed or running."
        echo "Install CUPS first:"
        echo "  sudo apt install cups       # clásico"
        echo "  sudo snap install cups       # snap"
        echo ""
    fi
fi

# Check for build dependencies
MISSING=""

if ! command -v gcc &> /dev/null; then
    MISSING="$MISSING build-essential"
fi

# cups-config puede estar en snap o deb
if [ "$CUPS_MODE" = "snap" ]; then
    if ! snap run cups.cups-config --version &>/dev/null; then
        echo "NOTE: cups-config not available from snap. Using default paths."
    fi
elif ! command -v cups-config &> /dev/null; then
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
    echo "  sudo dnf install gcc make cups-devel cups-libs"
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
echo "      Filter installed."
echo ""

# Restart CUPS
echo "[3/3] Restarting CUPS..."
if [ "$CUPS_MODE" = "snap" ]; then
    echo "      CUPS snap: no es necesario reiniciar manualmente."
    echo "      Si usas legacy-printer-app, reinícialo."
elif command -v systemctl &> /dev/null; then
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
if [ "$CUPS_MODE" = "snap" ]; then
    echo "    # List available USB printers:"
    echo "    snap run cups.lpinfo -v | grep usb"
    echo ""
    echo "    # Add the printer (replace URI with your printer's):"
    echo "    snap run cups.lpadmin -p L81 -E \\"
    echo "      -v usb://Unknown/Printer \\"
    echo "      -P $(pwd)/ppd/a81-printer.ppd"
else
    echo "    # List available USB printers:"
    echo "    lpinfo -v | grep usb"
    echo ""
    echo "    # Add the printer (replace URI with your printer's):"
    echo "    sudo lpadmin -p L81 -E \\"
    echo "      -v usb://Unknown/Printer \\"
    echo "      -P /usr/share/cups/model/luckjingle/a81-printer.ppd"
fi
echo ""
echo "    # Set as default:"
if [ "$CUPS_MODE" = "snap" ]; then
    echo "    snap run cups.lpadmin -d L81"
else
    echo "    sudo lpadmin -d L81"
fi
echo ""
echo "    # Test print:"
echo "    echo 'Hello World' | lpr -P L81"
echo ""
