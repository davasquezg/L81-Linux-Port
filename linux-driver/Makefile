# Makefile for LuckJingle L81/A80/A80H Linux CUPS Driver
# Soporta CUPS deb y snap / Supports deb and snap CUPS
#
# Build dependencies (Debian/Ubuntu):
#   sudo apt install build-essential cups libcups2-dev libcupsimage2-dev

CC ?= gcc
CFLAGS = -O2 -Wall -Wextra -I/usr/include/cups
LDFLAGS = -lcups -lcupsimage

PREFIX ?= /usr

# Auto-detect CUPS mode (snap vs deb)
CUPS_IS_SNAP := $(shell snap list cups 2>/dev/null | grep -q cups && echo yes || echo no)

ifeq ($(CUPS_IS_SNAP),yes)
  # En snap, CUPS no permite instalar filtros directamente
  # Se compila igualmente para uso con legacy-printer-app
  FILTERDIR = $(PREFIX)/lib/cups/filter
  PPDDIR = $(PREFIX)/share/cups/model/luckjingle
else
  FILTERDIR = $(PREFIX)/lib/cups/filter
  PPDDIR = $(PREFIX)/share/cups/model/luckjingle
endif

FILTER = rastertolabel-l81
SRC = src/rastertolabel-l81.c

.PHONY: all install uninstall clean detect help

all: $(FILTER)

$(FILTER): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

install: $(FILTER)
ifeq ($(CUPS_IS_SNAP),yes)
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║  AVISO: CUPS está instalado como snap (solo lectura).       ║"
	@echo "║  No se pueden instalar filtros/PPDs directamente en snap.   ║"
	@echo "║                                                             ║"
	@echo "║  OPCIONES:                                                  ║"
	@echo "║  A) Usar LPrint snap (recomendado):                         ║"
	@echo "║     sudo snap install lprint                                ║"
	@echo "║     sudo snap connect lprint:raw-usb                        ║"
	@echo "║     lprint add -d L81 -v 'usb://...' -m zpl_4inch-203dpi-dt║"
	@echo "║                                                             ║"
	@echo "║  B) Prueba rápida con CUPS snap:                            ║"
	@echo "║     sudo bash quick-test.sh                                 ║"
	@echo "║                                                             ║"
	@echo "║  C) Instalando en rutas clásicas para legacy-printer-app... ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
endif
	install -d $(DESTDIR)$(FILTERDIR)
	install -d $(DESTDIR)$(PPDDIR)
	install -m 755 $(FILTER) $(DESTDIR)$(FILTERDIR)/
	install -m 644 ppd/a81-printer.ppd $(DESTDIR)$(PPDDIR)/
	install -m 644 ppd/a80-printer.ppd $(DESTDIR)$(PPDDIR)/
	install -m 644 ppd/a80h-printer.ppd $(DESTDIR)$(PPDDIR)/
ifeq ($(CUPS_IS_SNAP),yes)
	@echo ""
	@echo "  Filtro y PPDs instalados en rutas clásicas."
	@echo "  Para usarlos con CUPS snap, necesitas legacy-printer-app"
	@echo "  o bien usa LPrint snap directamente."
else
	@echo ""
	@echo "  Filtro y PPDs instalados correctamente."
	@echo "  Reiniciar CUPS: sudo systemctl restart cups"
endif

uninstall:
	rm -f $(DESTDIR)$(FILTERDIR)/$(FILTER)
	rm -f $(DESTDIR)$(PPDDIR)/a81-printer.ppd
	rm -f $(DESTDIR)$(PPDDIR)/a80-printer.ppd
	rm -f $(DESTDIR)$(PPDDIR)/a80h-printer.ppd
	-rmdir $(DESTDIR)$(PPDDIR) 2>/dev/null || true
ifeq ($(CUPS_IS_SNAP),yes)
	@echo "  Eliminando colas de impresión si existen..."
	-snap run cups.lpadmin -x L81-ZPL 2>/dev/null || true
	-snap run cups.lpadmin -x L81-EPL2 2>/dev/null || true
	-snap run cups.lpadmin -x L81-CPCL 2>/dev/null || true
	-snap run cups.lpadmin -x L81 2>/dev/null || true
endif

clean:
	rm -f $(FILTER)

# Mostrar información del entorno / Show environment info
detect:
	@echo "CUPS mode:     $(CUPS_IS_SNAP)"
ifeq ($(CUPS_IS_SNAP),yes)
	@echo "CUPS version:  $$(snap list cups 2>/dev/null | awk '/cups/{print $$2}')"
	@echo "CUPS type:     snap (read-only — use LPrint or legacy-printer-app)"
else
	@echo "CUPS version:  $$(dpkg -l cups 2>/dev/null | awk '/^ii/{print $$3}' | head -1)"
	@echo "CUPS type:     deb (classic — direct filter install supported)"
endif
	@echo "Filter dir:    $(FILTERDIR)"
	@echo "PPD dir:       $(PPDDIR)"
	@echo "Compiler:      $(CC)"
	@echo ""
	@echo "Para diagnóstico completo: bash diagnose.sh"

help:
	@echo "Targets disponibles / Available targets:"
	@echo "  make          — Compilar el filtro / Build the filter"
	@echo "  make install  — Instalar filtro y PPDs / Install filter and PPDs"
	@echo "  make uninstall— Desinstalar / Uninstall"
	@echo "  make detect   — Detectar entorno CUPS / Detect CUPS environment"
	@echo "  make clean    — Limpiar binarios / Clean binaries"
	@echo "  make help     — Mostrar esta ayuda / Show this help"
