#!/bin/bash
# quick-test.sh — Pruebas rápidas de la L81 SIN compilar nada
# Soporta CUPS deb y snap / Supports deb and snap CUPS
# Usa el rastertolabel del sistema (CUPS) con PPD ZPL
# o LPrint snap para pruebas ZPL/TSPL nativas
#
# Ejecutar como: sudo bash quick-test.sh
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

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PPD_DIR="${SCRIPT_DIR}/ppd"
QUICK_PPD_DIR="${SCRIPT_DIR}/ppd-quicktest"

echo "============================================"
echo " LuckJingle L81 — Prueba Rápida"
echo " Sin compilación — usa rastertolabel del sistema"
echo " Soporta CUPS snap y deb"
echo "============================================"
echo ""

# ───────────────────────────────────────
# 1. Verificar que somos root
# ───────────────────────────────────────
if [ "$(id -u)" -ne 0 ]; then
    fail "Este script requiere permisos root."
    echo "  Ejecutar con: sudo bash $0"
    exit 1
fi

# ───────────────────────────────────────
# 2. Detectar CUPS snap vs deb
# ───────────────────────────────────────
CUPS_MODE="unknown"

if snap list cups 2>/dev/null | grep -q cups; then
    CUPS_MODE="snap"
    info "CUPS detectado como SNAP"
elif dpkg -l cups 2>/dev/null | grep -q "^ii"; then
    CUPS_MODE="deb"
    info "CUPS detectado como DEB"
else
    fail "CUPS no encontrado. Instalar antes de continuar."
    exit 1
fi

# Wrapper para comandos CUPS — usa snap run si CUPS es snap
cups_cmd() {
    local cmd="$1"; shift
    if [ "$CUPS_MODE" = "snap" ]; then
        snap run "cups.${cmd}" "$@"
    else
        "$cmd" "$@"
    fi
}

# ───────────────────────────────────────
# 3. Buscar rastertolabel
# ───────────────────────────────────────
RTL_PATH=""
for path in /snap/cups/current/usr/lib/cups/filter/rastertolabel \
            /usr/lib/cups/filter/rastertolabel \
            /usr/libexec/cups/filter/rastertolabel; do
    if [ -x "$path" ]; then
        RTL_PATH="$path"
        break
    fi
done

if [ -n "$RTL_PATH" ]; then
    ok "rastertolabel encontrado: $RTL_PATH"
else
    warn "rastertolabel no encontrado en el sistema."
    if [ "$CUPS_MODE" = "snap" ]; then
        info "El snap de CUPS debería tener su propio rastertolabel."
        info "Los PPDs usarán la referencia relativa 'rastertolabel' que CUPS resolverá."
    fi
fi
echo ""

# ───────────────────────────────────────
# 4. Generar PPDs de prueba rápida
# ───────────────────────────────────────
info "Generando PPDs de prueba rápida..."

mkdir -p "$QUICK_PPD_DIR"

if [ -f "${PPD_DIR}/a81-printer.ppd" ]; then
    SOURCE_PPD="${PPD_DIR}/a81-printer.ppd"
else
    fail "No se encontró ${PPD_DIR}/a81-printer.ppd"
    exit 1
fi

# --- PPD Prueba A: ZPL puro (cupsModelNumber 18 = ZEBRA_ZPL) ---
sed -e 's|*cupsFilter:.*|*cupsFilter: "application/vnd.cups-raster 0 rastertolabel"|' \
    -e 's|*cupsModelNumber:.*|*cupsModelNumber: 18|' \
    -e 's|*Manufacturer:.*|*Manufacturer: "LuckJingle"|' \
    -e 's|*Product:.*|*Product: "(LuckJingle L81 ZPL)"|' \
    -e 's|*ModelName:.*|*ModelName: "LuckJingle L81 ZPL Test"|' \
    -e 's|*NickName:.*|*NickName: "LuckJingle L81 (ZPL Mode)"|' \
    -e 's|*ShortNickName:.*|*ShortNickName: "L81-ZPL"|' \
    "$SOURCE_PPD" > "${QUICK_PPD_DIR}/l81-zpl-test.ppd"
ok "PPD generado: l81-zpl-test.ppd (ZPL, cupsModelNumber=18)"

# --- PPD Prueba B: EPL2 page mode (cupsModelNumber 17 = ZEBRA_EPL_PAGE) ---
sed -e 's|*cupsFilter:.*|*cupsFilter: "application/vnd.cups-raster 0 rastertolabel"|' \
    -e 's|*cupsModelNumber:.*|*cupsModelNumber: 17|' \
    -e 's|*Manufacturer:.*|*Manufacturer: "LuckJingle"|' \
    -e 's|*Product:.*|*Product: "(LuckJingle L81 EPL2)"|' \
    -e 's|*ModelName:.*|*ModelName: "LuckJingle L81 EPL2 Test"|' \
    -e 's|*NickName:.*|*NickName: "LuckJingle L81 (EPL2 Mode)"|' \
    -e 's|*ShortNickName:.*|*ShortNickName: "L81-EPL2"|' \
    "$SOURCE_PPD" > "${QUICK_PPD_DIR}/l81-epl2-test.ppd"
ok "PPD generado: l81-epl2-test.ppd (EPL2, cupsModelNumber=17)"

# --- PPD Prueba C: CPCL (cupsModelNumber 19 = ZEBRA_CPCL) ---
sed -e 's|*cupsFilter:.*|*cupsFilter: "application/vnd.cups-raster 0 rastertolabel"|' \
    -e 's|*cupsModelNumber:.*|*cupsModelNumber: 19|' \
    -e 's|*Manufacturer:.*|*Manufacturer: "LuckJingle"|' \
    -e 's|*Product:.*|*Product: "(LuckJingle L81 CPCL)"|' \
    -e 's|*ModelName:.*|*ModelName: "LuckJingle L81 CPCL Test"|' \
    -e 's|*NickName:.*|*NickName: "LuckJingle L81 (CPCL Mode)"|' \
    -e 's|*ShortNickName:.*|*ShortNickName: "L81-CPCL"|' \
    "$SOURCE_PPD" > "${QUICK_PPD_DIR}/l81-cpcl-test.ppd"
ok "PPD generado: l81-cpcl-test.ppd (CPCL, cupsModelNumber=19)"

echo ""

# ───────────────────────────────────────
# 5. Detectar la impresora USB
# ───────────────────────────────────────
info "Buscando impresoras USB..."
echo ""

PRINTER_URI=""

info "URIs de dispositivos disponibles:"
cups_cmd lpinfo -v 2>/dev/null | grep -E "^(direct|usb)" | sed 's/^/  /' || warn "  No se pudieron listar (¿permisos?)"
echo ""

# Intentar detectar automáticamente la L81
DETECTED=$(cups_cmd lpinfo -v 2>/dev/null | grep -iE "usb.*\b(l81|a80|a81|yxwl|lujiang|luckjingle)\b" || true)
if [ -n "$DETECTED" ]; then
    PRINTER_URI=$(echo "$DETECTED" | head -1 | awk '{print $2}')
    ok "L81 detectada automáticamente: $PRINTER_URI"
else
    warn "L81 no detectada automáticamente por nombre."
    echo ""
    info "Dispositivos USB disponibles en CUPS:"
    cups_cmd lpinfo -v 2>/dev/null | grep "^direct usb:" | sed 's/^direct /  /' | cat -n || true
    echo ""
    echo -n "  Selecciona el número de tu impresora (o escribe la URI manualmente): "
    read -r SELECTION

    if [[ "$SELECTION" =~ ^[0-9]+$ ]]; then
        PRINTER_URI=$(cups_cmd lpinfo -v 2>/dev/null | grep "^direct usb:" | sed -n "${SELECTION}p" | awk '{print $2}')
    elif [[ "$SELECTION" =~ ^usb:// ]]; then
        PRINTER_URI="$SELECTION"
    else
        fail "Selección no válida"
        exit 1
    fi
fi

if [ -z "$PRINTER_URI" ]; then
    fail "No se pudo determinar la URI de la impresora."
    echo "  Conecta la L81 por USB y vuelve a ejecutar."
    echo "  También puedes especificar manualmente:"
    echo "    export PRINTER_URI='usb://...'"
    echo "    sudo bash $0"
    exit 1
fi
ok "URI seleccionada: $PRINTER_URI"
echo ""

# ───────────────────────────────────────
# 6. Menú de pruebas
# ───────────────────────────────────────
echo "============================================"
echo " Selecciona la prueba a ejecutar:"
echo "============================================"
echo ""

if [ "$CUPS_MODE" = "snap" ]; then
    echo "  1) Prueba con LPrint snap (ZPL genérico) — RECOMENDADO"
    echo "  2) Prueba ZPL  vía CUPS snap (cupsModelNumber=18)"
    echo "  3) Prueba EPL2 vía CUPS snap (cupsModelNumber=17)"
    echo "  4) Prueba CPCL vía CUPS snap (cupsModelNumber=19)"
    echo "  5) Ejecutar TODAS las pruebas en secuencia"
    echo "  6) Solo instalar PPDs (sin imprimir)"
else
    echo "  1) Prueba ZPL  (cupsModelNumber=18) — MÁS PROBABLE"
    echo "  2) Prueba EPL2 (cupsModelNumber=17)"
    echo "  3) Prueba CPCL (cupsModelNumber=19)"
    echo "  4) Prueba con LPrint snap (ZPL genérico)"
    echo "  5) Ejecutar TODAS las pruebas en secuencia"
    echo "  6) Solo instalar PPDs (sin imprimir)"
fi

echo ""
echo -n "  Opción [1]: "
read -r TEST_CHOICE
TEST_CHOICE=${TEST_CHOICE:-1}

install_and_test() {
    local PPD_FILE="$1"
    local QUEUE_NAME="$2"
    local DESC="$3"

    info "Instalando cola: $QUEUE_NAME ($DESC)"

    # Remover cola anterior si existe
    cups_cmd lpadmin -x "$QUEUE_NAME" 2>/dev/null || true

    # Instalar la cola — lpadmin acepta PPD externo incluso en snap CUPS
    cups_cmd lpadmin -p "$QUEUE_NAME" -E \
        -v "$PRINTER_URI" \
        -P "$PPD_FILE" \
        -D "$DESC" \
        -L "LuckJingle L81 Test"

    ok "Cola instalada: $QUEUE_NAME"

    # Crear página de prueba simple
    local TEST_FILE="/tmp/l81-test-${QUEUE_NAME}.txt"
    cat > "$TEST_FILE" << 'TESTPAGE'
================================
  LuckJingle L81 - Test Page
================================

  Driver Test - Linux Port
  Timestamp: __TIMESTAMP__

  If you can read this, the
  driver is working correctly.

  Protocol: __PROTOCOL__
  cupsModelNumber: __MODEL__
  CUPS Mode: __CUPSMODE__

================================
TESTPAGE
    sed -i "s/__TIMESTAMP__/$(date)/" "$TEST_FILE"
    sed -i "s/__PROTOCOL__/$DESC/" "$TEST_FILE"
    sed -i "s/__MODEL__/$QUEUE_NAME/" "$TEST_FILE"
    sed -i "s/__CUPSMODE__/$CUPS_MODE/" "$TEST_FILE"

    echo ""
    echo -n "  ¿Enviar página de prueba a $QUEUE_NAME? [s/N]: "
    read -r SEND
    if [[ "$SEND" =~ ^[sS] ]]; then
        cups_cmd lp -d "$QUEUE_NAME" "$TEST_FILE"
        ok "Trabajo enviado a $QUEUE_NAME"
        echo "  Verificar estado: $([ "$CUPS_MODE" = "snap" ] && echo "snap run cups.lpstat" || echo "lpstat") -p $QUEUE_NAME"
        if [ "$CUPS_MODE" = "snap" ]; then
            echo "  Ver log: sudo journalctl -u snap.cups.cupsd -f"
        else
            echo "  Ver log: sudo tail -f /var/log/cups/error_log"
        fi
        echo ""
        echo -n "  ¿Imprimió correctamente? [s/N]: "
        read -r RESULT
        if [[ "$RESULT" =~ ^[sS] ]]; then
            ok "ÉXITO con protocolo $DESC"
            return 0
        else
            warn "Fallo con protocolo $DESC"
            return 1
        fi
    fi
    return 2
}

test_lprint() {
    info "Probando con LPrint snap..."

    if ! snap list lprint 2>/dev/null | grep -q lprint; then
        info "Instalando LPrint snap..."
        snap install lprint
        snap connect lprint:raw-usb
        snap connect lprint:avahi-control 2>/dev/null || true
        # nota: lprint no tiene plug cups-control
        snap start lprint.lprint-server 2>/dev/null || true
        sleep 2
    fi

    ok "LPrint instalado"

    # Verificar interfaces
    info "Verificando interfaces snap..."
    for iface in raw-usb avahi-control; do
        if snap connections lprint 2>/dev/null | grep -q "$iface.*lprint"; then
            ok "  $iface: conectado"
        else
            warn "  $iface: NO conectado — conectando..."
            snap connect "lprint:$iface" 2>/dev/null || warn "  No se pudo conectar $iface"
        fi
    done

    # Asegurar que el servidor esté corriendo
    snap start lprint.lprint-server 2>/dev/null || true
    sleep 1

    info "Dispositivos detectados por LPrint:"
    snap run lprint devices 2>/dev/null | sed 's/^/  /' || echo "  (sin dispositivos)"

    info "Drivers ZPL/TSPL disponibles:"
    snap run lprint drivers 2>/dev/null | grep -iE "zpl|tspl" | sed 's/^/  /' || echo "  (no se pudo listar)"

    echo ""
    echo "  Para agregar la impresora con LPrint:"
    echo "    lprint add -d L81 -v \"$PRINTER_URI\" -m zpl_4inch-203dpi-dt"
    echo "    lprint submit -d L81 /ruta/a/archivo.png"
    echo ""
    echo "  Para probar con TSPL:"
    echo "    lprint add -d L81-tspl -v \"$PRINTER_URI\" -m tspl_4inch-203dpi-dt"
    echo ""
    echo "  Web UI: http://localhost:8000"
    echo ""
    echo -n "  ¿Agregar impresora L81 con driver ZPL automáticamente? [s/N]: "
    read -r AUTO_ADD
    if [[ "$AUTO_ADD" =~ ^[sS] ]]; then
        snap run lprint add -d L81 -v "$PRINTER_URI" -m zpl_4inch-203dpi-dt 2>/dev/null && \
            ok "Impresora L81 agregada con driver ZPL" || \
            fail "Error al agregar la impresora"
    fi
}

# ───────────────────────────────────────
# 7. Ejecutar prueba seleccionada
# ───────────────────────────────────────
if [ "$CUPS_MODE" = "snap" ]; then
    # Menú snap: LPrint primero
    case "$TEST_CHOICE" in
        1) test_lprint ;;
        2) install_and_test "${QUICK_PPD_DIR}/l81-zpl-test.ppd" "L81-ZPL" "ZPL Mode" ;;
        3) install_and_test "${QUICK_PPD_DIR}/l81-epl2-test.ppd" "L81-EPL2" "EPL2 Mode" ;;
        4) install_and_test "${QUICK_PPD_DIR}/l81-cpcl-test.ppd" "L81-CPCL" "CPCL Mode" ;;
        5)
            info "Ejecutando todas las pruebas en secuencia..."
            test_lprint
            echo ""
            install_and_test "${QUICK_PPD_DIR}/l81-zpl-test.ppd" "L81-ZPL" "ZPL Mode" || true
            echo ""
            install_and_test "${QUICK_PPD_DIR}/l81-epl2-test.ppd" "L81-EPL2" "EPL2 Mode" || true
            echo ""
            install_and_test "${QUICK_PPD_DIR}/l81-cpcl-test.ppd" "L81-CPCL" "CPCL Mode" || true
            ;;
        6)
            info "En modo snap, los PPDs se instalan al crear la cola con lpadmin."
            info "No es necesario copiarlos a un directorio del sistema."
            ok "Los PPDs de prueba están en: ${QUICK_PPD_DIR}/"
            ;;
        *) fail "Opción no válida"; exit 1 ;;
    esac
else
    # Menú deb: ZPL primero
    case "$TEST_CHOICE" in
        1) install_and_test "${QUICK_PPD_DIR}/l81-zpl-test.ppd" "L81-ZPL" "ZPL Mode" ;;
        2) install_and_test "${QUICK_PPD_DIR}/l81-epl2-test.ppd" "L81-EPL2" "EPL2 Mode" ;;
        3) install_and_test "${QUICK_PPD_DIR}/l81-cpcl-test.ppd" "L81-CPCL" "CPCL Mode" ;;
        4) test_lprint ;;
        5)
            info "Ejecutando todas las pruebas en secuencia..."
            install_and_test "${QUICK_PPD_DIR}/l81-zpl-test.ppd" "L81-ZPL" "ZPL Mode" || true
            echo ""
            install_and_test "${QUICK_PPD_DIR}/l81-epl2-test.ppd" "L81-EPL2" "EPL2 Mode" || true
            echo ""
            install_and_test "${QUICK_PPD_DIR}/l81-cpcl-test.ppd" "L81-CPCL" "CPCL Mode" || true
            ;;
        6)
            info "Instalando solo los PPDs..."
            CUPS_MODELDIR=""
            for dir in /usr/share/cups/model /usr/share/ppd; do
                [ -d "$dir" ] && CUPS_MODELDIR="$dir" && break
            done
            if [ -n "$CUPS_MODELDIR" ]; then
                mkdir -p "${CUPS_MODELDIR}/luckjingle" 2>/dev/null || true
                cp "${QUICK_PPD_DIR}"/*.ppd "${CUPS_MODELDIR}/luckjingle/" 2>/dev/null && \
                    ok "PPDs instalados en ${CUPS_MODELDIR}/luckjingle/" || \
                    warn "No se pudieron copiar los PPDs."
            else
                warn "Directorio de modelos CUPS no encontrado."
            fi
            echo "  Ahora puedes agregar la impresora desde http://localhost:631"
            ;;
        *) fail "Opción no válida"; exit 1 ;;
    esac
fi

echo ""
echo "============================================"
echo " Fin de la prueba"
echo "============================================"
echo ""
echo "  Comandos útiles:"
if [ "$CUPS_MODE" = "snap" ]; then
    echo "    snap run cups.lpstat -p                    # Ver colas"
    echo "    sudo journalctl -u snap.cups.cupsd -f      # Ver logs CUPS snap"
    echo "    snap run cups.cancel -a                    # Cancelar todos los trabajos"
    echo "    snap run cups.lpadmin -x L81-ZPL           # Eliminar cola ZPL"
    echo "    snap run cups.lpadmin -x L81-EPL2          # Eliminar cola EPL2"
    echo "    snap run cups.lpadmin -x L81-CPCL          # Eliminar cola CPCL"
    echo "    lprint devices                             # Dispositivos LPrint"
    echo "    lprint submit -d L81 archivo.png           # Imprimir vía LPrint"
else
    echo "    lpstat -p                         # Ver colas"
    echo "    sudo tail -f /var/log/cups/error_log  # Ver logs CUPS"
    echo "    cancel -a                         # Cancelar todos los trabajos"
    echo "    lpadmin -x L81-ZPL              # Eliminar cola ZPL"
    echo "    lpadmin -x L81-EPL2             # Eliminar cola EPL2"
    echo "    lpadmin -x L81-CPCL             # Eliminar cola CPCL"
fi
echo ""
