# LuckJingle L81 / A80 / A80H - Linux CUPS Driver

**Port del driver macOS de la impresora termica A4 LuckJingle L81 a Linux via CUPS.**

**Linux CUPS driver port for the LuckJingle L81 thermal A4 printer.**

---

## Descripcion / Description

### Espanol

Este proyecto porta el driver propietario de macOS de las impresoras termicas LuckJingle L81, A80 y A80H a Linux, utilizando el sistema de impresion CUPS.

El driver original de macOS (`rastertolabel`) es un binario Mach-O universal (x86_64 + ARM64) que convierte datos CUPS Raster a comandos **ZPL (Zebra Programming Language)**. Este proyecto reimplementa esa funcionalidad como codigo fuente C compilable en Linux.

### English

This project ports the proprietary macOS driver for the LuckJingle L81, A80, and A80H thermal printers to Linux, using the CUPS printing system.

The original macOS driver (`rastertolabel`) is a universal Mach-O binary (x86_64 + ARM64) that converts CUPS Raster data to **ZPL (Zebra Programming Language)** commands. This project reimplements that functionality as C source code compilable on Linux.

---

## Especificaciones del Hardware / Hardware Specs

| Propiedad / Property | Valor / Value |
|---|---|
| Resolucion / Resolution | 203 dpi |
| Ancho de impresion / Print width | ~210mm (A4) |
| Tipo / Type | Termica directa / Direct thermal |
| Conexion / Connection | USB (principal), Bluetooth 2.0/4.0 |
| Protocolo / Protocol | ZPL (Zebra Programming Language) compatible |
| Set de instrucciones / Instruction set | YPL (compatible ZPL/TSPL/CPCL) |
| Espacio de color / Color space | Escala de grises / Grayscale (8bpp) |
| Tamanos / Sizes | A4, Letter, Legal, A5, B5, etiquetas / labels |
| cupsModelNumber | 20 (personalizado / custom) |

---

## Hallazgos del Analisis Tecnico / Technical Analysis Findings

El analisis de ingenieria inversa del binario macOS revelo:

The reverse engineering analysis of the macOS binary revealed:

- El filtro usa **ZPL** como protocolo de salida, no EPL2 ni TSPL
- `cupsModelNumber: 20` es un valor personalizado (no esta en el CUPS estandar, donde 18=ZPL)
- La secuencia ZPL incluye: descarga de grafico (`~DGR:CUPS.GRF`), colocacion (`^XGR`), y limpieza (`^IDR`)
- Compresion ZPL run-length para optimizar la transferencia de datos
- Deteccion de lineas duplicadas (caracter `:` en ZPL)
- El binario fue firmado por Xiamen Angyin Information Technology Co., Ltd

### Secuencia ZPL / ZPL Sequence

```
StartPage:  ~SD{darkness}  ~DGR:CUPS.GRF,{bytes},{width},
OutputLine: {compressed hex data} or ':'
EndPage:    ^XA ^POI ^LH0,0 ^LL{h} ^PW{w} ^PR{s} ^MN{m} ^MT{t}
            ^JZ{r} ^PQ{c} ^FO0,0^XGR:CUPS.GRF,1,1^FS ^XZ
Cleanup:    ^XA ^IDR:CUPS.GRF^FS ^XZ
```

---

## Instalacion / Installation

### Requisitos / Requirements

**Debian / Ubuntu:**
```bash
sudo apt install build-essential cups libcups2-dev libcupsimage2-dev
```

**Fedora / RHEL:**
```bash
sudo dnf install gcc make cups-devel cups-libs
```

**Arch Linux:**
```bash
sudo pacman -S base-devel cups
```

### Instalacion Rapida / Quick Install

```bash
git clone https://github.com/davasquezg/L81-Linux-Port.git
cd L81-Linux-Port/linux-driver
./install.sh
```

El instalador detecta automaticamente si CUPS esta instalado como **snap** o **deb** y ofrece las opciones adecuadas.

The installer automatically detects whether CUPS is installed as **snap** or **deb** and offers the appropriate options.

### CUPS Snap (Ubuntu 23.10+) / Snap CUPS

Ubuntu 23.10+ usa CUPS como snap por defecto. El snap es de solo lectura, asi que no se pueden instalar filtros/PPDs directamente.

Ubuntu 23.10+ uses CUPS as a snap by default. The snap is read-only, so custom filters/PPDs cannot be installed directly.

**Opcion A — LPrint snap (RECOMENDADO / RECOMMENDED):**

LPrint ya soporta ZPL y TSPL de forma nativa. Es la ruta mas rapida.

LPrint already supports ZPL and TSPL natively. This is the fastest path.

```bash
# Instalar LPrint / Install LPrint
sudo snap install lprint
sudo snap connect lprint:raw-usb
sudo snap connect lprint:avahi-control
# Iniciar servidor / Start server
sudo snap start lprint.lprint-server

# Detectar impresora / Detect printer
lprint devices

# Agregar con driver TSPL Rollo (match CMD:XPP,XL) / Add with TSPL Rollo driver
# La L81 reporta CMD:XPP,XL que coincide con el Rollo X1038
lprint add -d L81 -v 'usb://YXWL/L81?serial=...' -m tspl_rollo-x1038_203dpi

# Imprimir / Print
lprint submit -d L81 archivo.png

# Web UI
# http://localhost:8000

# Alternativos si TSPL Rollo no funciona:
# Alternatives if TSPL Rollo doesn't work:
lprint add -d L81-zpl -v 'usb://...' -m zpl_4inch-203dpi-dt
lprint add -d L81-tspl -v 'usb://...' -m tspl_203dpi
lprint add -d L81-cpcl -v 'usb://...' -m cpcl-203dpi
```

**Opcion B — Prueba rapida con CUPS snap / Quick test with snap CUPS:**

Es posible usar `lpadmin` del snap con un PPD externo que referencia el `rastertolabel` integrado del snap.

You can use the snap's `lpadmin` with an external PPD that references the snap's built-in `rastertolabel`.

```bash
cd linux-driver
sudo bash quick-test.sh
```

**Opcion C — legacy-printer-app (avanzado / advanced):**

Compilar el filtro y usar legacy-printer-app para exponerlo como una Printer Application IPP.

Compile the filter and use legacy-printer-app to expose it as an IPP Printer Application.

```bash
cd linux-driver
make
sudo make install
# Luego instalar y configurar legacy-printer-app
# Then install and configure legacy-printer-app
```

### Diagnostico / Diagnostics

Antes de instalar, ejecuta el script de diagnostico para verificar tu entorno:

Before installing, run the diagnostic script to check your environment:

```bash
cd linux-driver
bash diagnose.sh
```

Este script detecta:
- CUPS snap vs deb
- rastertolabel del sistema
- LPrint (snap o deb)
- Impresoras USB conectadas
- Dependencias de compilacion
- Colas de impresion configuradas

### Instalacion Manual (CUPS deb) / Manual Install (deb CUPS)

```bash
cd linux-driver

# Compilar / Build
make

# Instalar / Install (requiere root)
sudo make install

# Reiniciar CUPS / Restart CUPS
sudo systemctl restart cups
```

### Desinstalacion / Uninstall

```bash
cd linux-driver
./uninstall.sh
```

O manualmente / Or manually:
```bash
cd linux-driver
sudo make uninstall
sudo systemctl restart cups
```

---

## Agregar la Impresora / Adding the Printer

### Via Interfaz Web CUPS / Via CUPS Web Interface

1. Abrir / Open `http://localhost:631`
2. Ir a / Go to **Administration > Add Printer**
3. Seleccionar la impresora USB / Select your USB printer
4. Elegir / Choose **LuckJingle** como fabricante / as manufacturer
5. Seleccionar modelo / Select model: **A81**, **A80**, o/or **A80H**

### Via Linea de Comandos / Via Command Line

```bash
# Listar impresoras USB / List USB printers
lpinfo -v | grep usb

# Agregar impresora / Add printer (reemplazar URI / replace URI)
sudo lpadmin -p L81 -E \
  -v usb://Unknown/Printer \
  -P /usr/share/cups/model/luckjingle/a81-printer.ppd

# Establecer como predeterminada / Set as default
sudo lpadmin -d L81

# Imprimir prueba / Test print
echo "Hello World" | lpr -P L81
```

---

## Arquitectura / Architecture

```
                        CUPS 2.x Filter Chain
                        =====================

 +-----------+     +------------+     +------------------+     +----------+
 | Documento |     |            |     |                  |     | Impresora|
 | PDF/PS    | --> | pdftopdf   | --> | pdftoraster /    | --> | L81      |
 | Document  |     |            |     | gstoraster       |     | Printer  |
 +-----------+     +------------+     +------------------+     +----------+
                                             |
                                             v
                                   +-------------------+
                                   | rastertolabel-l81 |
                                   | (este driver /    |
                                   |  this driver)     |
                                   +-------------------+
                                             |
                                             v
                                   +-------------------+
                                   | CUPS Raster       |
                                   | (8bpp grayscale)  |
                                   |        |          |
                                   |        v          |
                                   | Threshold to 1bpp |
                                   | Hex conversion    |
                                   | ZPL compression   |
                                   | ~DGR download     |
                                   | ^XGR placement    |
                                   +-------------------+
                                             |
                                             v
                                      ZPL Commands
                                      to USB/BT
```

### Archivos Instalados / Installed Files

| Archivo / File | Ubicacion / Location |
|---|---|
| `rastertolabel-l81` | `/usr/lib/cups/filter/` |
| `a81-printer.ppd` | `/usr/share/cups/model/luckjingle/` |
| `a80-printer.ppd` | `/usr/share/cups/model/luckjingle/` |
| `a80h-printer.ppd` | `/usr/share/cups/model/luckjingle/` |

---

## Solucion de Problemas / Troubleshooting

### La impresora no aparece / Printer not showing up

**CUPS deb (clasico):**
```bash
# Verificar que CUPS este corriendo / Check CUPS is running
systemctl status cups

# Verificar conexion USB / Check USB connection
lsusb | grep -i print
lpinfo -v | grep usb

# Ver logs de CUPS / View CUPS logs
sudo tail -f /var/log/cups/error_log
```

**CUPS snap:**
```bash
# Verificar que CUPS snap este corriendo / Check snap CUPS is running
snap services cups

# Verificar conexion USB / Check USB connection
lsusb | grep -i print
snap run cups.lpinfo -v | grep usb

# Ver logs de CUPS snap / View snap CUPS logs
sudo journalctl -u snap.cups.cupsd -f

# Verificar interfaces snap / Check snap interfaces
snap connections cups
snap connections lprint  # si usas LPrint
```

**LPrint snap:**
```bash
# Verificar dispositivos / Check devices
lprint devices

# Verificar servidor / Check server
snap services lprint

# Reiniciar servidor / Restart server
sudo snap restart lprint.lprint-server

# Conectar interfaz USB si no detecta / Connect USB if not detecting
sudo snap connect lprint:raw-usb
```

### Error de compilacion / Build errors

```bash
# Verificar dependencias / Check dependencies
dpkg -l | grep -E "libcups|cups-dev"

# En caso de error con cupsimage / If cupsimage error
sudo apt install libcupsimage2-dev
```

### CUPS snap no encuentra el filtro / Snap CUPS can't find filter

El snap de CUPS es de solo lectura. No se pueden instalar filtros personalizados directamente.
Usa LPrint snap o legacy-printer-app en su lugar.

The CUPS snap is read-only. Custom filters cannot be installed directly.
Use LPrint snap or legacy-printer-app instead.

### Impresion en blanco / Blank printing

- Verificar que el papel esta cargado correctamente / Check paper is loaded correctly
- Intentar ajustar Media Tracking en opciones de impresion / Try adjusting Media Tracking in print options
- Para papel continuo usar "RollPaper" / For continuous paper use "RollPaper"
- Para etiquetas usar "LabelPaper" / For labels use "LabelPaper"

### Impresion muy clara u oscura / Too light or dark

Ajustar Darkness en opciones de impresion / Adjust Darkness in print options:
- **Low**: Para impresion ligera / For light printing
- **Medium**: Balance / Balanced
- **High**: Para impresion oscura / For dark printing (default)

---

## Migracion a CUPS 3.0 / CUPS 3.0 Migration Path

CUPS 3.0 elimina los PPDs y filtros clasicos en favor de **Printer Applications** basadas en IPP Everywhere y el framework PAPPL.

CUPS 3.0 removes classic PPDs and filters in favor of **Printer Applications** based on IPP Everywhere and the PAPPL framework.

### Opciones futuras / Future options:

1. **LPrint** (recomendado / recommended): Printer Application de Michael Sweet que ya soporta ZPL. La L81 podria agregarse como un driver ZPL personalizado.
   - https://www.msweet.org/lprint/

2. **pappl-retrofit**: Permite envolver drivers clasicos CUPS (PPD + filtro) en una Printer Application sin reescribir.
   - https://github.com/OpenPrinting/pappl-retrofit

3. **Driver PAPPL nativo**: Escribir un driver nativo usando el SDK de PAPPL para maximo control y compatibilidad.

```
CUPS 3.0 Architecture:
+----------+     +---------------------------+     +---------+
| App      | --> | LPrint / PAPPL            | --> | Printer |
| (IPP)    |     | Printer Application       |     | (USB)   |
+----------+     |  - IPP Everywhere server  |     +---------+
                 |  - ZPL driver integrado   |
                 |  - Auto-deteccion USB     |
                 +---------------------------+
```

---

## Estructura del Repositorio / Repository Structure

```
L81-Linux-Port/
|-- linux-driver/              <-- Driver Linux (este proyecto)
|   |-- src/
|   |   +-- rastertolabel-l81.c    Filtro CUPS (C source)
|   |-- ppd/
|   |   |-- a81-printer.ppd       PPD para L81/A81
|   |   |-- a80-printer.ppd       PPD para A80
|   |   +-- a80h-printer.ppd      PPD para A80H
|   |-- Makefile                  Sistema de compilacion (snap-aware)
|   |-- install.sh                Instalador (snap + deb)
|   |-- uninstall.sh              Desinstalador (snap + deb)
|   |-- diagnose.sh               Diagnostico del entorno
|   +-- quick-test.sh             Pruebas rapidas sin compilar
|
|-- A4Drv_LuckJingleMac_Clean/    Driver macOS original (limpio)
|   |-- PPDs/                     PPDs originales de macOS
|   |-- A80/Filter/               Binario rastertolabel (Mach-O)
|   +-- A80H/Filter/              Binario rastertolabel (Mach-O)
|
|-- Original_Mac_Drv/             Paquete .pkg original de macOS
|
|-- XPrinter/                     SDK y driver Linux de XPrinter (referencia)
|   |-- XPrinter_Linux_Drv/       Binarios multi-arquitectura
|   +-- X_Printer_LinuxSDK/       SDK con headers C++ (ESC/TSPL/ZPL/CPCL)
|
+-- README.md                     Este archivo
```

---

## Contribuir / Contributing

Las contribuciones son bienvenidas. / Contributions are welcome.

1. Fork el repositorio / Fork the repo
2. Crear una rama / Create a branch: `git checkout -b feature/mi-mejora`
3. Hacer commit / Commit: `git commit -m "Descripcion del cambio"`
4. Push: `git push origin feature/mi-mejora`
5. Crear Pull Request / Create PR

### Areas donde se necesita ayuda / Areas where help is needed:

- Pruebas en hardware real / Testing on real hardware
- Soporte ARM64 (Raspberry Pi, etc.)
- Integracion con LPrint para CUPS 3.0
- Mejoras en la compresion ZPL / ZPL compression improvements
- Soporte Bluetooth / Bluetooth support
- Paquetes .deb y .rpm / Debian and RPM packages

---

## Licencia / License

El codigo del filtro CUPS esta basado en `rastertolabel.c` de OpenPrinting CUPS y se distribuye bajo la **Apache License 2.0**.

The CUPS filter code is based on `rastertolabel.c` from OpenPrinting CUPS and is distributed under the **Apache License 2.0**.

Los archivos PPD son adaptaciones de los PPDs originales de YXWL/LuckJingle.

PPD files are adaptations of the original YXWL/LuckJingle PPDs.

---

## Creditos / Credits

- **LuckJingle / YXWL / Xiamen Angyin** - Hardware y driver macOS original
- **OpenPrinting / Apple** - CUPS y `rastertolabel.c` original
- **Michael Sweet** - CUPS, LPrint, PAPPL
- **XPrinter** - SDK de referencia y binarios Linux
