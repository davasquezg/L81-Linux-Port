//
// Created by dan on 23-12-21.
//
#include <iostream>
#include "TestConnect.h"
#include "printer.sdk.cpcl.h"

using namespace std;

const char *ParseStatus(unsigned int status) {
    if (status == 0) {
        return "Printer normal!";
    } else if ((status & 0b1) > 0) {
        return "The print head is opened！";
    } else if ((status & 0b10) > 0) {
        return "Paper jam！";
    } else if ((status & 0b100) > 0) {
        return "Out of paper！";
    } else if ((status & 0b1000) > 0) {
        return "Out of ribbon！";
    } else if ((status & 0b10000) > 0) {
        return "Print pause！";
    } else if ((status & 0b100000) > 0) {
        return "Printing！";
    } else if ((status & 0b1000000) > 0) {
        return "Cover opened！";
    } else {
        return "Other error！";
    }
}

void GetStatus(void *printer) {
    printf("GetStatus\n");
    int status;
    int ret = CPCL_GetPrinterStatus(printer, &status);
    if (ret == ERROR_CM_SUCCESS) {
        printf("The printer status is %s\n", ParseStatus(status));
    } else {
        printf("Get Error, Code is: %d\n", ret);
    }
}

void PrintSample(void *printer) {
    CPCL_AddLabel(printer, 0, 400, 1);
    CPCL_AddBox(printer, 10, 10, 510, 280, 5);
    CPCL_AddLine(printer, 10, 180, 510, 180, 4);
    CPCL_AddQRCode(printer, 0, 20, 20, 2, 6, 3, "QR CODE ABC123");
    CPCL_SetAlign(printer, 0);
    CPCL_AddText(printer, 0, "0", 0, 300, 20, "REVERSE");
    CPCL_AddInverseLine(printer, 290, 20, 390, 20, 24);
    CPCL_AddText(printer, 0, "0", 0, 170, 20, "FONT0");
    CPCL_AddText(printer, 0, "1", 0, 170, 60, "FONT1");
    CPCL_AddText(printer, 0, "2", 0, 170, 80, "FONT2");
    CPCL_AddText(printer, 0, "3", 0, 170, 120, "FONT3");
    CPCL_AddBarCode(printer, 0, 20, 1, 1, 50, 270, 80, "123456789");
    CPCL_AddText(printer, 0, "0", 0, 20, 210, "Hello World!");
    CPCL_AddLine(printer, 200, 200, 450, 200, 50);
    CPCL_Print(printer);
    printf("PrintSample\n");
}

void PrintQRCode(void *printer) {
    printf("PrintQRCode\n");
    CPCL_AddLabel(printer, 0, 400, 1);
    CPCL_AddQRCode(printer, 0, 20, 20, 2, 8, 3, "QR CODE ABC123");
    CPCL_Print(printer);
}

void PrintBarCode(void *printer) {
    CPCL_AddLabel(printer, 0, 800, 1);
    CPCL_AddBarCodeText(printer, 1, 7, 0, 0);
    CPCL_AddText(printer, 0, "0", 0, 0, 0, "Code 128");
    CPCL_AddBarCode(printer, 0, 20, 1, 1, 50, 0, 30, "123456789");
    CPCL_AddText(printer, 0, "0", 0, 0, 120, "UPC-E");
    CPCL_AddBarCode(printer, 0, 3, 1, 1, 50, 0, 150, "223456");
    CPCL_AddText(printer, 0, "0", 0, 0, 240, "EAN/JAN-13");
    CPCL_AddBarCode(printer, 0, 6, 1, 1, 50, 0, 270, "323456791234");
    CPCL_AddText(printer, 0, "0", 0, 0, 360, "Code 39");
    CPCL_AddBarCode(printer, 0, 12, 1, 1, 50, 0, 390, "72233445");
    CPCL_AddText(printer, 0, "0", 0, 250, 0, "UPC-A");
    CPCL_AddBarCode(printer, 0, 0, 1, 1, 50, 250, 30, "423456789012");
    CPCL_AddText(printer, 0, "0", 0, 250, 120, "EAN/JAN-8");
    CPCL_AddBarCode(printer, 0, 9, 1, 1, 50, 250, 150, "52233445");
    CPCL_AddText(printer, 0, "0", 0, 300, 360, "CODABAR");
    CPCL_AddBarCode(printer, 1, 22, 1, 1, 50, 300, 540, "A67859B");
    CPCL_AddText(printer, 0, "0", 0, 0, 480, "Code 93/Ext.93");
    CPCL_AddBarCode(printer, 0, 16, 1, 1, 50, 0, 510, "823456789");
    CPCL_AddBarCodeText(printer, 0, 7, 0, 0);
    CPCL_Print(printer);
    printf("PrintBarCode\n");
}

void PrintImage(void *printer, char* path) {
    printf("PrintImage %s\n", path);
    CPCL_AddLabel(printer, 0, 600, 1);
    CPCL_SetAlign(printer, 0);
    CPCL_AddImage(printer, 0, 40, 0, path);
    CPCL_Print(printer);
}

int main() {
    startTest(GetStatus, PrintSample, PrintQRCode, PrintBarCode, PrintImage);

    return 0;
}