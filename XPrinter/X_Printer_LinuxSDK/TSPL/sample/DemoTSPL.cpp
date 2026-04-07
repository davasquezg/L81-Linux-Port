//
// Created by dan on 23-12-21.
//
#include <iostream>
#include "TestConnect.h"
#include "printer.sdk.tspl.h"

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
    unsigned int status;
    int ret = TSPL_GetPrinterStatus(printer, &status);
    if (ret == ERROR_CM_SUCCESS) {
        printf("The printer status is %s\n", ParseStatus(status));
    } else {
        printf("Get Error, Code is: %d\n", ret);
    }
}

void PrintSample(void *printer) {
    printf("PrintSample\n");
    TSPL_Setup(printer, 4, 8, 60, 30, 1, 2, 4);
    TSPL_ClearBuffer(printer);
    TSPL_Direction(printer, 0, 0);
    TSPL_Box(printer, 6, 6, 384, 235, 5);
    TSPL_Box(printer, 16, 16, 376, 225, 5);
    TSPL_BarCode(printer, 30, 30, 7, "ABCDEFGH", 100);
    TSPL_QrCode(printer, 265, 30, 4, 1, 0, 0, 1, 2, "test qrcode");
    TSPL_Text(printer, 200, 144, "3", "Test EN");
    TSPL_Text(printer, 38, 165, "3", "Test EN", 0, 1, 2);
    TSPL_Bar(printer, 200, 183, 166, 30);
    TSPL_Bar(printer, 334, 145, 30, 30);
    TSPL_Print(printer, 1, 1);
}

void PrintQRCode(void *printer) {
    printf("PrintQRCode\n");
    TSPL_Setup(printer, 5, 4, 76, 30, 1, 0, 0);
    TSPL_ClearBuffer(printer);
    TSPL_QrCode(printer, 265, 30, 8, 1, 0, 0, 1, 2, "test qrcode");
    TSPL_Print(printer, 1, 1);
}

void PrintBarCode(void *printer) {
    printf("PrintBarCode\n");
    TSPL_Setup(printer, 5, 4, 76, 30, 1, 0, 0);
    TSPL_ClearBuffer(printer);
    TSPL_BarCode(printer, 30, 30, 7, "ABCDEFGH", 100, 2, 0, 2, 2);
    TSPL_Print(printer, 1, 1);
}

void PrintImage(void *printer, char *path) {
    printf("PrintImage %s\n", path);
    TSPL_Setup(printer, 2, 8, 60, 80, 1, 2, 0);
    TSPL_ClearBuffer(printer);
    TSPL_Image(printer, 10, 50, 0, path);
    TSPL_Print(printer, 1, 1);
}

int main() {
    startTest(GetStatus, PrintSample, PrintQRCode, PrintBarCode, PrintImage);

    return 0;
}