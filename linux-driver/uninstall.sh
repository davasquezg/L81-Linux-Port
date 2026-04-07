#!/bin/bash
# uninstall.sh - LuckJingle L81/A80/A80H Linux Driver Uninstaller
# Soporta CUPS deb y snap / Supports deb and snap CUPS
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

# Detectar CUPS snap vs deb
CUPS_MODE="unknown"
if snap list cups 2>/dev/null | grep -q cups; then
    CUPS_MODE="snap"
elif dpkg -l cups 2>/dev/null | grep -q "^ii"; then
    CUPS_MODE="deb"
fi

# Wrapper para comandos CUPS
cups_cmd() {
    local cmd="$1"; shift
    if [ "$CUPS_MODE" = "snap" ]; then
        snap run "cups.${cmd}" "$@" 2>/dev/null
    elif command -v "$cmd" &>/dev/null; then
        "$cmd" "$@" 2>/dev/null
    fi
}

# ───────────────────────────────────────
# 1. Eliminar colas de impresión
# ───────────────────────────────────────
echo "[1/4] Eliminando colas de impresión..."
for queue in L81 L81-ZPL L81-EPL2 L81-CPCL L81-tspl; do
    if cups_cmd lpstat -p "$queue" &>/dev/null; then
        $SUDO cups_cmd lpadmin -x "$queue" 2>/dev/null && \
            echo "      Cola eliminada: $queue" || true
    fi
done
echo ""

# ───────────────────────────────────────
# 2. Desinstalar filtro y PPDs clásicos
# ───────────────────────────────────────
echo "[2/4] Eliminando filtro y PPDs..."
$SUDO make uninstall 2>/dev/null || {
    # Manual removal if make is not available
    $SUDO rm -f /usr/lib/cups/filter/rastertolabel-l81
    $SUDO rm -f /usr/libexec/cups/filter/rastertolabel-l81
    $SUDO rm -f /usr/share/cups/model/luckjingle/a81-printer.ppd
    $SUDO rm -f /usr/share/cups/model/luckjingle/a80-printer.ppd
    $SUDO rm -f /usr/share/cups/model/luckjingle/a80h-printer.ppd
    $SUDO rmdir /usr/share/cups/model/luckjingle 2>/dev/null || true
}
echo "      Filtro y PPDs eliminados."
echo ""

# ───────────────────────────────────────
# 3. Limpiar PPDs de prueba rápida
# ───────────────────────────────────────
echo "[3/4] Limpiando archivos de prueba..."
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if [ -d "${SCRIPT_DIR}/ppd-quicktest" ]; then
    rm -rf "${SCRIPT_DIR}/ppd-quicktest"
    echo "      PPDs de prueba eliminados."
else
    echo "      No se encontraron PPDs de prueba."
fi
echo ""

# ───────────────────────────────────────
# 4. Instrucciones para LPrint
# ───────────────────────────────────────
echo "[4/4] Verificando LPrint..."
if snap list lprint 2>/dev/null | grep -q lprint; then
    echo ""
    echo "  LPrint snap está instalado. Si deseas eliminarlo:"
    echo "    lprint delete -d L81        # Eliminar impresora de LPrint"
    echo "    sudo snap remove lprint     # Desinstalar LPrint snap"
elif command -v lprint &>/dev/null; then
    echo "  LPrint está instalado. Si deseas eliminar impresoras:"
    echo "    lprint delete -d L81"
fi
echo ""

# Reiniciar CUPS
if [ "$CUPS_MODE" = "deb" ]; then
    echo "Reiniciando CUPS..."
    if command -v systemctl &> /dev/null; then
        $SUDO systemctl restart cups
    elif command -v service &> /dev/null; then
        $SUDO service cups restart
    fi
fi

echo ""
echo "============================================"
echo " Desinstalación completa / Uninstall complete"
echo "============================================"
echo ""
