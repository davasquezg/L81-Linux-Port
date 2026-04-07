/*
 * rastertolabel-l81.c - CUPS raster filter for LuckJingle L81/A80/A80H
 *                       thermal A4 printers.
 *
 * Converts CUPS raster data to ZPL (Zebra Programming Language) commands.
 * Based on reverse-engineering of the macOS rastertolabel binary and the
 * standard CUPS rastertolabel.c filter (Apache License 2.0).
 *
 * Copyright (c) 2024 LuckJingle L81 Linux Port Contributors
 * Licensed under the Apache License, Version 2.0
 *
 * Usage: rastertolabel-l81 job-id user title copies options [file]
 */

#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Model numbers */
#define LUCKJINGLE_YPL  20    /* cupsModelNumber for L81/A80/A80H */

/* Globals */
static int           ModelNumber;
static int           Page;
static int           Canceled;
static unsigned      DarknessSetting;
static const char   *MediaTracking;
static const char   *PrintRate;
static const char   *PrintMode;
static const char   *ErrorReprint;
static unsigned char *Buffer;
static unsigned char *LastBuffer;
static unsigned      BufSize;

/* ZPL compression lookup tables */
static const char ZPLCompressChars[] = "GHIJKLMNOPQRSTUVWXYghijklmnopqrstuvwxy";
/* G=1, H=2, ... Y=19, g=20, h=40, ... y=380, z=400 */


/*
 * ZPLCompress() - ZPL run-length encode a hex string.
 *
 * ZPL compression uses single-character codes for repeat counts:
 *   G-Y = 1-19 repeats
 *   g-y = 20-380 repeats (multiples of 20)
 *   z   = 400 repeats
 *
 * The input is a hex string (e.g., "FF00FF00...") and the output is
 * the compressed version written to stdout.
 */
static void
ZPLCompress(const char *hex, int length)
{
    int  i, count;
    char ch, last;

    if (length <= 0)
        return;

    last  = hex[0];
    count = 1;

    for (i = 1; i < length; i++)
    {
        ch = hex[i];
        if (ch == last)
        {
            count++;
        }
        else
        {
            /* Emit the run for 'last' */
            while (count >= 400)
            {
                putchar('z');
                count -= 400;
            }
            if (count >= 20)
            {
                /* g=20, h=40, i=60, ... y=380 */
                putchar(ZPLCompressChars[19 + (count / 20) - 1]);
                count %= 20;
            }
            if (count > 0)
            {
                /* G=1, H=2, ... Y=19 */
                putchar(ZPLCompressChars[count - 1]);
            }
            putchar(last);

            last  = ch;
            count = 1;
        }
    }

    /* Emit remaining run */
    while (count >= 400)
    {
        putchar('z');
        count -= 400;
    }
    if (count >= 20)
    {
        putchar(ZPLCompressChars[19 + (count / 20) - 1]);
        count %= 20;
    }
    if (count > 0)
    {
        putchar(ZPLCompressChars[count - 1]);
    }
    putchar(last);
}


/*
 * Setup() - Read PPD and configure options.
 */
static void
Setup(ppd_file_t *ppd, int num_options, cups_option_t *options)
{
    const char *val;

    if (ppd)
    {
        ModelNumber = ppd->model_number;
        ppdMarkDefaults(ppd);
        cupsMarkOptions(ppd, num_options, options);
    }
    else
    {
        ModelNumber = LUCKJINGLE_YPL;
    }

    /* Darkness: Low=10, Medium=20, High=30 */
    val = cupsGetOption("Darkness", num_options, options);
    if (val)
    {
        if (!strcmp(val, "Low"))
            DarknessSetting = 10;
        else if (!strcmp(val, "Medium"))
            DarknessSetting = 20;
        else
            DarknessSetting = 30;
    }
    else
    {
        DarknessSetting = 30; /* Default: High */
    }

    /* Media tracking */
    val = cupsGetOption("zeMediaTracking", num_options, options);
    MediaTracking = val ? val : "RollPaper";

    /* Print rate */
    val = cupsGetOption("zePrintRate", num_options, options);
    PrintRate = val ? val : "Default";

    /* Print mode (Direct/Thermal) */
    val = cupsGetOption("zePrintMode", num_options, options);
    PrintMode = val ? val : "Direct";

    /* Error reprint */
    val = cupsGetOption("zeErrorReprint", num_options, options);
    ErrorReprint = val ? val : "N";
}


/*
 * StartPage() - Begin a new page: send darkness and start graphic download.
 */
static void
StartPage(cups_page_header2_t *header)
{
    unsigned total_bytes;
    unsigned bytes_per_line;

    /* Calculate dimensions
     * cupsBytesPerLine is the raster width in bytes (1 bit per pixel after
     * thresholding, but we receive 8bpp grayscale and convert to 1bpp hex).
     * For ZPL ~DG, bytes_per_line = ceil(width_in_dots / 8).
     */
    bytes_per_line = (header->cupsWidth + 7) / 8;
    total_bytes    = bytes_per_line * header->cupsHeight;

    /* Allocate line buffers */
    BufSize = bytes_per_line;
    Buffer  = malloc(BufSize);
    LastBuffer = calloc(1, BufSize);

    if (!Buffer || !LastBuffer)
    {
        fputs("ERROR: Unable to allocate memory for raster buffer\n", stderr);
        exit(1);
    }

    /* Send ZPL darkness setting */
    printf("~SD%02u\n", DarknessSetting);

    /* Start graphic download:
     * ~DGR:CUPS.GRF,<total_bytes>,<bytes_per_line>,
     */
    printf("~DGR:CUPS.GRF,%u,%u,\n", total_bytes, bytes_per_line);
}


/*
 * OutputLine() - Convert one raster line to ZPL hex with compression.
 *
 * We receive 8-bit grayscale pixels. We threshold them to 1-bit (black/white),
 * convert to hex, and then run ZPL compression on the hex string.
 *
 * Duplicate lines are output as ':' (ZPL duplicate-line shorthand).
 */
static void
OutputLine(cups_page_header2_t *header, unsigned char *pixels)
{
    unsigned      i, byte;
    unsigned      bytes_per_line;
    unsigned      width;
    char         *hex;
    int           hex_len;
    int           threshold_val;
    const char   *threshold_str;
    unsigned      brightness;
    unsigned      contrast;
    const char   *val;

    bytes_per_line = (header->cupsWidth + 7) / 8;
    width          = header->cupsWidth;

    /* Get threshold/brightness/contrast from header options.
     * The PPD default threshold is 175 (out of 255).
     * Pixels darker than threshold become black (1 in ZPL).
     *
     * In ZPL: bit=1 means black, bit=0 means white.
     * In grayscale: 0=black, 255=white.
     * So pixel < threshold → black → bit=1.
     */
    threshold_val = 175;

    /* Use cupsRowCount as a way to pass threshold if set in Resolution */
    /* For now we use the static threshold; brightness/contrast are handled
     * by the CUPS rasterizer pipeline before we see the data. */

    /* Convert 8bpp grayscale to 1bpp packed, MSB first */
    memset(Buffer, 0, BufSize);

    for (i = 0; i < width; i++)
    {
        byte = i / 8;
        if (pixels[i] < threshold_val)
        {
            /* Pixel is dark → set bit (black in ZPL) */
            Buffer[byte] |= (0x80 >> (i % 8));
        }
    }

    /* Check for duplicate line */
    if (memcmp(Buffer, LastBuffer, bytes_per_line) == 0)
    {
        putchar(':');
        putchar('\n');
        return;
    }

    /* Save current line for next comparison */
    memcpy(LastBuffer, Buffer, bytes_per_line);

    /* Convert to hex string */
    hex_len = bytes_per_line * 2;
    hex     = malloc(hex_len + 1);
    if (!hex)
    {
        fputs("ERROR: Unable to allocate hex buffer\n", stderr);
        exit(1);
    }

    for (i = 0; i < bytes_per_line; i++)
    {
        sprintf(hex + i * 2, "%02X", Buffer[i]);
    }
    hex[hex_len] = '\0';

    /* Output with ZPL compression */
    ZPLCompress(hex, hex_len);
    putchar('\n');

    free(hex);
}


/*
 * EndPage() - Finish the page: send ZPL label format with graphic placement.
 */
static void
EndPage(cups_page_header2_t *header)
{
    unsigned  label_length;
    unsigned  print_width;
    int       speed;
    const char *mn_cmd;
    const char *mt_cmd;
    const char *jz_cmd;
    int       copies;

    label_length = header->cupsHeight;
    print_width  = header->cupsWidth;
    copies       = header->NumCopies > 0 ? header->NumCopies : 1;

    /* Map print rate */
    speed = 4; /* Default speed */
    if (PrintRate && strcmp(PrintRate, "Default") != 0)
    {
        speed = atoi(PrintRate);
        if (speed < 1 || speed > 14)
            speed = 4;
    }

    /* Map media tracking:
     * RollPaper  → ^MNN (continuous/no tracking)
     * FoldPaper  → ^MNN (continuous)
     * LabelPaper → ^MNY (non-continuous/gap)
     * TattooPaper → ^MNM (mark sensing)
     */
    if (!strcmp(MediaTracking, "LabelPaper"))
        mn_cmd = "Y";
    else if (!strcmp(MediaTracking, "TattooPaper"))
        mn_cmd = "M";
    else
        mn_cmd = "N";

    /* Media type */
    if (PrintMode && !strcmp(PrintMode, "Thermal"))
        mt_cmd = "T";
    else
        mt_cmd = "D";

    /* Error reprint */
    if (ErrorReprint && !strcmp(ErrorReprint, "Y"))
        jz_cmd = "Y";
    else
        jz_cmd = "N";

    /* --- ZPL Label Format --- */
    printf("^XA\n");                                 /* Start format */
    printf("^POI\n");                                /* Print Orientation Inverted */
    printf("^LH0,0\n");                              /* Label Home */
    printf("^LL%u\n", label_length);                 /* Label Length */
    printf("^LT0\n");                                /* Label Top offset */
    printf("^PW%u\n", print_width);                  /* Print Width */
    printf("^PR%d,%d,%d\n", speed, speed, speed);    /* Print Rate */
    printf("^MN%s\n", mn_cmd);                       /* Media Tracking */
    printf("^MT%s\n", mt_cmd);                       /* Media Type */
    printf("^JZ%s\n", jz_cmd);                       /* Reprint on error */
    printf("^PQ%d, 0, 0, N\n", copies);             /* Print Quantity */
    printf("^FO0,0^XGR:CUPS.GRF,1,1^FS\n");         /* Place graphic */
    printf("^XZ\n");                                 /* End format */

    /* Cleanup: delete stored graphic */
    printf("^XA\n");
    printf("^IDR:CUPS.GRF^FS\n");
    printf("^XZ\n");

    /* Free buffers */
    free(Buffer);
    free(LastBuffer);
    Buffer     = NULL;
    LastBuffer = NULL;
}


/*
 * CancelJob() - SIGTERM handler.
 */
static void
CancelJob(int sig)
{
    (void)sig;
    Canceled = 1;
}


/*
 * main() - Standard CUPS filter entry point.
 *
 * Usage: rastertolabel-l81 job user title copies options [file]
 */
int
main(int argc, char *argv[])
{
    int                  fd;
    cups_raster_t       *ras;
    cups_page_header2_t  header;
    ppd_file_t          *ppd;
    int                  num_options;
    cups_option_t       *options;
    unsigned             y;
    unsigned char       *pixels;

    /* Validate arguments */
    if (argc < 6 || argc > 7)
    {
        fputs("Usage: rastertolabel-l81 job user title copies options [file]\n",
              stderr);
        return 1;
    }

    /* Open input: file or stdin */
    if (argc == 7)
    {
        fd = open(argv[6], O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "ERROR: Unable to open \"%s\": %s\n",
                    argv[6], strerror(errno));
            return 1;
        }
    }
    else
    {
        fd = 0; /* stdin */
    }

    /* Install signal handler */
    signal(SIGTERM, CancelJob);

    /* Open PPD */
    ppd = ppdOpenFile(getenv("PPD"));

    /* Parse options */
    num_options = cupsParseOptions(argv[5], 0, &options);

    /* Setup from PPD + options */
    Setup(ppd, num_options, options);

    /* Open raster stream */
    ras = cupsRasterOpen(fd, CUPS_RASTER_READ);
    if (!ras)
    {
        fputs("ERROR: Unable to open raster stream\n", stderr);
        if (ppd) ppdClose(ppd);
        if (fd != 0) close(fd);
        return 1;
    }

    /* Process pages */
    Page = 0;

    while (!Canceled && cupsRasterReadHeader2(ras, &header))
    {
        if (header.cupsBytesPerLine == 0 || header.cupsHeight == 0)
            continue;

        Page++;
        fprintf(stderr, "PAGE: %d %d\n", Page, header.NumCopies);

        /* Allocate pixel row buffer */
        pixels = malloc(header.cupsBytesPerLine);
        if (!pixels)
        {
            fputs("ERROR: Unable to allocate pixel buffer\n", stderr);
            break;
        }

        /* Start the page (send ~SD and ~DG header) */
        StartPage(&header);

        /* Process each raster line */
        for (y = 0; y < header.cupsHeight && !Canceled; y++)
        {
            if (cupsRasterReadPixels(ras, pixels, header.cupsBytesPerLine) < 1)
                break;

            OutputLine(&header, pixels);
        }

        /* End the page (send ^XA..^XZ label format) */
        EndPage(&header);

        free(pixels);
    }

    /* Cleanup */
    cupsRasterClose(ras);
    if (ppd) ppdClose(ppd);
    if (fd != 0) close(fd);
    cupsFreeOptions(num_options, options);

    return Page == 0 ? 1 : 0;
}
