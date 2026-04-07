#!/bin/bash
# diagnose.sh — Diagnóstico del entorno de impresión para LuckJingle L81
# Soporta CUPS deb y snap / Supports deb and snap CUPS
# Ejecutar como: bash diagnose.sh
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

info()  { echo -e "${CYAN}[INFO]${NC} $*"; }
ok()    { echo -e "${GREEN}[OK]${NC}   $*"; }
warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
fail()  { echo -e "${RED}[FAIL]${NC} $*"; }

# ───────────────────────────────────────
# 0. Detectar CUPS snap vs deb
#    Detect CUPS snap vs deb
# ───────────────────────────────────────
CUPS_MODE="unknown"

if snap list cups 2>/dev/null | grep -q cups; then
    CUPS_MODE="snap"
elif dpkg -l cups 2>/dev/null | grep -q "^ii"; then
    CUPS_MODE="deb"
fi

# Wrapper para comandos CUPS — usa snap run si CUPS es snap
# Wrapper for CUPS commands — uses snap run if CUPS is snap
cups_cmd() {
    local cmd="$1"; shift
    if [ "$CUPS_MODE" = "snap" ]; then
        snap run "cups.${cmd}" "$@" 2>/dev/null
    elif command -v "$cmd" &>/dev/null; then
        "$cmd" "$@" 2>/dev/null
    else
        echo "(comando $cmd no disponible)" >&2
        return 1
    fi
}

echo "============================================"
echo " LuckJingle L81 — Diagnóstico del entorno"
echo "============================================"
echo ""

# ───────────────────────────────────────
# 1. Sistema operativo
# ───────────────────────────────────────
info "Sistema operativo:"
if [ -f /etc/os-release ]; then
    . /etc/os-release
    echo "  Distro:  $PRETTY_NAME"
    echo "  ID:      $ID"
fi
echo "  Kernel:  $(uname -r)"
echo "  Arch:    $(uname -m)"
echo ""

# ───────────────────────────────────────
# 2. CUPS — ¿deb o snap?
# ───────────────────────────────────────
info "Detectando CUPS..."

CUPS_VER=""
CUPS_FILTERDIR=""
CUPS_MODELDIR=""

if [ "$CUPS_MODE" = "snap" ]; then
    CUPS_VER=$(snap list cups 2>/dev/null | awk '/cups/{print $2}')
    ok "CUPS instalado como SNAP (v${CUPS_VER})"
    info "El snap de CUPS es de solo lectura — no se pueden instalar filtros/PPDs directamente."
    info "Opciones: LPrint snap (recomendado), legacy-printer-app, o CUPS deb."

    # Detectar rutas del snap
    SNAP_CUPS_FILTER="/snap/cups/current/usr/lib/cups/filter"
    if [ -d "$SNAP_CUPS_FILTER" ]; then
        CUPS_FILTERDIR="$SNAP_CUPS_FILTER"
    fi
    # El snap no tiene model dir escribible
    CUPS_MODELDIR="(snap — solo lectura / read-only)"

elif [ "$CUPS_MODE" = "deb" ]; then
    CUPS_VER=$(dpkg -l cups 2>/dev/null | awk '/^ii.*cups/{print $3}' | head -1)
    ok "CUPS instalado como DEB (v${CUPS_VER})"

    # Rutas estándar para CUPS deb
    for dir in /usr/lib/cups/filter /usr/libexec/cups/filter; do
        if [ -d "$dir" ]; then
            CUPS_FILTERDIR="$dir"
            break
        fi
    done
    for dir in /usr/share/cups/model /usr/share/ppd; do
        if [ -d "$dir" ]; then
            CUPS_MODELDIR="$dir"
            break
        fi
    done
else
    fail "CUPS no encontrado. Instalar con: sudo apt install cups"
    fail "  o como snap: sudo snap install cups"
fi

echo "  Modo:          $CUPS_MODE"
echo "  Versión:       ${CUPS_VER:-desconocida}"
echo "  Filter dir:    ${CUPS_FILTERDIR:-NO ENCONTRADO}"
echo "  Model dir:     ${CUPS_MODELDIR:-NO ENCONTRADO}"
echo ""

# ───────────────────────────────────────
# 3. Verificar rastertolabel del sistema
# ───────────────────────────────────────
info "Buscando rastertolabel del sistema..."
RTL_SYSTEM=""
for path in /snap/cups/current/usr/lib/cups/filter/rastertolabel \
            /usr/lib/cups/filter/rastertolabel \
            /usr/libexec/cups/filter/rastertolabel; do
    if [ -x "$path" ]; then
        RTL_SYSTEM="$path"
        ok "Encontrado: $path"
        file "$path" 2>/dev/null | sed 's/^/  /'
        break
    fi
done
if [ -z "$RTL_SYSTEM" ]; then
    warn "rastertolabel del sistema no encontrado"
fi
echo ""

# ───────────────────────────────────────
# 4. Verificar librerías de desarrollo
# ───────────────────────────────────────
info "Verificando dependencias de compilación..."
MISSING_DEPS=()

if dpkg -l libcups2-dev 2>/dev/null | grep -q "^ii"; then
    ok "libcups2-dev instalado"
elif dpkg -l libcups2t64-dev 2>/dev/null | grep -q "^ii"; then
    ok "libcups2t64-dev instalado (transitional)"
else
    fail "libcups2-dev NO instalado"
    MISSING_DEPS+=("libcups2-dev")
fi

if dpkg -l libcupsimage2-dev 2>/dev/null | grep -q "^ii"; then
    ok "libcupsimage2-dev instalado"
elif dpkg -l libcupsimage2t64-dev 2>/dev/null | grep -q "^ii"; then
    ok "libcupsimage2t64-dev instalado (transitional)"
else
    fail "libcupsimage2-dev NO instalado"
    MISSING_DEPS+=("libcupsimage2-dev")
fi

if command -v gcc &>/dev/null; then
    ok "gcc: $(gcc --version | head -1)"
else
    fail "gcc NO instalado"
    MISSING_DEPS+=("build-essential")
fi

if [ ${#MISSING_DEPS[@]} -gt 0 ]; then
    warn "Instalar dependencias faltantes con:"
    echo "  sudo apt install ${MISSING_DEPS[*]}"
fi
echo ""

# ───────────────────────────────────────
# 5. Detectar LPrint
# ───────────────────────────────────────
info "Buscando LPrint..."
LPRINT_MODE="none"

if snap list lprint 2>/dev/null | grep -q lprint; then
    LPRINT_MODE="snap"
    LPRINT_VER=$(snap list lprint 2>/dev/null | awk '/lprint/{print $2}')
    ok "LPrint snap instalado (v${LPRINT_VER})"

    # Verificar interfaces snap conectadas
    info "Interfaces snap de LPrint:"
    for iface in raw-usb avahi-control; do
        if snap connections lprint 2>/dev/null | grep -q "$iface.*lprint"; then
            ok "  $iface: conectado"
        else
            warn "  $iface: NO conectado — ejecutar: sudo snap connect lprint:$iface"
        fi
    done

    info "Drivers LPrint disponibles:"
    snap run lprint drivers 2>/dev/null | grep -iE "zpl|tspl|epl|cpcl" | sed 's/^/  /' || echo "  (no se pudo listar)"

elif command -v lprint &>/dev/null; then
    LPRINT_MODE="deb"
    ok "LPrint instalado (deb/compilado): $(lprint --version 2>/dev/null || echo 'versión desconocida')"
else
    warn "LPrint no instalado."
    if [ "$CUPS_MODE" = "snap" ]; then
        warn "  RECOMENDADO instalar LPrint snap: sudo snap install lprint"
    else
        warn "  Instalar con: sudo snap install lprint"
    fi
fi
echo ""

# ───────────────────────────────────────
# 6. Detectar impresoras USB conectadas
# ───────────────────────────────────────
info "Buscando impresoras USB..."
if command -v lsusb &>/dev/null; then
    PRINTERS=$(lsusb 2>/dev/null | grep -iE "printer|label|thermal|luckjingle|lujiang|yxwl|a80|a81|l81" || true)
    if [ -n "$PRINTERS" ]; then
        ok "Posibles impresoras detectadas:"
        echo "$PRINTERS" | sed 's/^/  /'
    else
        warn "No se detectaron impresoras conocidas por USB"
        info "Todas las USB:"
        lsusb 2>/dev/null | sed 's/^/  /'
    fi
else
    warn "lsusb no disponible. Instalar con: sudo apt install usbutils"
fi
echo ""

# Verificar device URIs de CUPS
info "URIs de dispositivos CUPS:"
cups_cmd lpinfo -v 2>/dev/null | grep -iE "usb|direct" | sed 's/^/  /' || echo "  (requiere permisos root — ejecutar con sudo)"
echo ""

# ───────────────────────────────────────
# 7. Colas de impresión existentes
# ───────────────────────────────────────
info "Colas de impresión configuradas:"
cups_cmd lpstat -p -d 2>/dev/null | sed 's/^/  /' || echo "  (ninguna o sin permisos)"
echo ""

# ───────────────────────────────────────
# 8. Recomendación
# ───────────────────────────────────────
echo "============================================"
echo " RECOMENDACIÓN"
echo "============================================"

if [ "$CUPS_MODE" = "snap" ]; then
    echo ""
    echo "  Tu CUPS corre como SNAP. No se pueden instalar filtros/PPDs"
    echo "  directamente en el snap (es solo lectura)."
    echo ""
    echo "  ╔══════════════════════════════════════════════════╗"
    echo "  ║  OPCIÓN A — LPrint snap (RECOMENDADO)           ║"
    echo "  ║  Cero compilación, nativo snap, soporta ZPL     ║"
    echo "  ╚══════════════════════════════════════════════════╝"
    echo ""
    echo "    sudo snap install lprint"
    echo "    sudo snap connect lprint:raw-usb"
    echo "    sudo snap connect lprint:avahi-control"
    # nota: lprint no tiene plug cups-control
    echo "    sudo snap start lprint.lprint-server"
    echo "    lprint devices                    # detectar la L81"
    echo "    lprint drivers                    # ver drivers ZPL disponibles"
    echo "    lprint add -d L81 -v 'usb://...' -m zpl_4inch-203dpi-dt"
    echo "    lprint submit -d L81 test.png"
    echo ""
    echo "  OPCIÓN B — Prueba rápida con CUPS snap + PPD:"
    echo "    El snap permite usar lpadmin con un PPD externo."
    echo "    Ejecutar: sudo bash quick-test.sh"
    echo ""
    echo "  OPCIÓN C — legacy-printer-app (avanzado):"
    echo "    1. Compilar e instalar el filtro en rutas clásicas"
    echo "    2. Instalar legacy-printer-app"
    echo "    3. La app detecta el PPD/filtro y lo expone como IPP"
    echo ""

elif [ "$CUPS_MODE" = "deb" ]; then
    echo ""
    echo "  Tu CUPS corre como DEB (instalación clásica)."
    echo ""
    echo "  OPCIÓN A — Prueba rápida (0 compilación):"
    echo "    Usar el rastertolabel del sistema con PPD ZPL estándar."
    echo "    Ejecutar: sudo bash quick-test.sh"
    echo ""
    echo "  OPCIÓN B — Driver personalizado (compilar):"
    echo "    cd linux-driver && make"
    echo "    sudo make install"
    echo "    sudo systemctl restart cups"
    echo ""
    echo "  OPCIÓN C — LPrint snap (CUPS 3.0 ready):"
    echo "    sudo snap install lprint"
    echo "    sudo snap connect lprint:raw-usb"
    echo "    lprint devices"
    echo "    lprint add -d L81 -v 'usb://...' -m zpl_4inch-203dpi-dt"
    echo ""
else
    echo ""
    echo "  CUPS no detectado. Instalar:"
    echo "    sudo apt install cups       # método clásico"
    echo "    sudo snap install cups       # método snap (Ubuntu 23.10+)"
    echo ""
fi

echo "============================================"
