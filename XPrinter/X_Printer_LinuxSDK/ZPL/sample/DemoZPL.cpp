//
// Created by dan on 23-12-21.
//
#include <iostream>
#include "TestConnect.h"
#include "printer.sdk.zpl.h"

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
    int ret = ZPL_GetPrinterStatus(printer, &status);
    if (ret == ERROR_CM_SUCCESS) {
        printf("The printer status is %s\n", ParseStatus(status));
    } else {
        printf("Get Error, Code is: %d\n", ret);
    }
}

void PrintSample(void *printer) {
    int xPos = 40;
    printf("PrintSample\n");
    ZPL_StartFormat(printer);
    ZPL_Text(printer, xPos, 50, 13, 0, 59, 53, "FROM:");
    ZPL_Text(printer, 200, 40, 3, 0, 18, 30, "Company Name");
    ZPL_Text(printer, 200, 80, 3, 0, 18, 30, "Street, City");
    ZPL_Text(printer, 200, 120, 3, 0, 18, 30, "Phone");
    ZPL_GraphicBox(printer, xPos, 170, 500, 8, 4, 0);
    ZPL_Text(printer, xPos, 200, 13, 0, 59, 53, "SHIP TO:");
    ZPL_Text(printer, 200, 190, 3, 0, 18, 30, "Company Name");
    ZPL_Text(printer, 200, 230, 3, 0, 18, 30, "Street, City");
    ZPL_Text(printer, 200, 270, 3, 0, 18, 30, "Phone");
    ZPL_GraphicBox(printer, xPos, 320, 500, 8, 4, 0);
    ZPL_Text(printer, xPos, 340, 13, 0, 59, 53, "WEIGHT:");
    ZPL_Text(printer, 200, 340, 3, 0, 18, 30, "1kg/2,2lb");
    ZPL_BarCode128(printer, 80, 410, 0, 5, 150, 'Y', 'N', 'N', 'A', "12345678");
    ZPL_EndFormat(printer);
}

void PrintQRCode(void *printer) {
    printf("PrintQRCode\n");
    ZPL_StartFormat(printer);
    ZPL_QRCode(printer, 120, 5, 0, 2, 5, 'Q', '0', 'B', "Welcome to the world of printing!");
    ZPL_EndFormat(printer);
}

void PrintBarCode(void *printer) {
    printf("PrintBarCode\n");
    ZPL_StartFormat(printer);
    ZPL_BarCode128(printer, 120, 10, 0, 3, 80, 'Y', 'N', 'N', 'A', "123456");
    ZPL_EndFormat(printer);
}

void PrintImage(void *printer, char* path) {
    printf("PrintImage %s\n", path);
    ZPL_StartFormat(printer);
    ZPL_PrintImage(printer, 120, 10, path);
    ZPL_EndFormat(printer);
}

int main() {
    startTest(GetStatus, PrintSample, PrintQRCode, PrintBarCode, PrintImage);

    return 0;
}