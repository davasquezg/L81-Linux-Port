# Makefile for LuckJingle L81/A80/A80H Linux CUPS Driver
#
# Build dependencies (Debian/Ubuntu):
#   sudo apt install build-essential cups libcups2-dev libcupsimage2-dev

CC ?= gcc
CFLAGS = -O2 -Wall -Wextra -I/usr/include/cups
LDFLAGS = -lcups -lcupsimage

PREFIX ?= /usr
FILTERDIR = $(PREFIX)/lib/cups/filter
PPDDIR = $(PREFIX)/share/cups/model/luckjingle

FILTER = rastertolabel-l81
SRC = src/rastertolabel-l81.c

.PHONY: all install uninstall clean

all: $(FILTER)

$(FILTER): $(SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

install: $(FILTER)
	install -d $(DESTDIR)$(FILTERDIR)
	install -d $(DESTDIR)$(PPDDIR)
	install -m 755 $(FILTER) $(DESTDIR)$(FILTERDIR)/
	install -m 644 ppd/a81-printer.ppd $(DESTDIR)$(PPDDIR)/
	install -m 644 ppd/a80-printer.ppd $(DESTDIR)$(PPDDIR)/
	install -m 644 ppd/a80h-printer.ppd $(DESTDIR)$(PPDDIR)/

uninstall:
	rm -f $(DESTDIR)$(FILTERDIR)/$(FILTER)
	rm -f $(DESTDIR)$(PPDDIR)/a81-printer.ppd
	rm -f $(DESTDIR)$(PPDDIR)/a80-printer.ppd
	rm -f $(DESTDIR)$(PPDDIR)/a80h-printer.ppd
	-rmdir $(DESTDIR)$(PPDDIR) 2>/dev/null || true

clean:
	rm -f $(FILTER)
