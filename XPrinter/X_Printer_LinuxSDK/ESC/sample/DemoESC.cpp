//
// Created by dan on 23-12-21.
//
#include <iostream>
#include "TestConnect.h"
#include "printer.sdk.esc.h"

using namespace std;

const char *ParseStatus(unsigned int status) {
    if (0x12 == status) {
        return "Ready";
    } else if ((status & 0b100) > 0) {
        return "Cover opened";
    } else if ((status & 0b1000) > 0) {
        return "Feed button has been pressed";
    } else if ((status & 0b100000) > 0) {
        return "Printer is out of paper";
    } else if ((status & 0b1000000) > 0) {
        return "Error condition";
    } else {
        return "Error";
    }
}

void GetStatus(void *printer) {
    printf("GetStatus\n");
    unsigned int status = 2;
    int ret = GetPrinterState(printer, &status);
    if (ret == ERROR_CM_SUCCESS) {
        printf("The printer status is %s\r\n", ParseStatus(status));
    } else {
        printf("Get Error, Code is: %d\r\n", ret);
    }
}

void PrintSample(void *printer) {
    PrinterInitialize(printer);
    SetRelativeHorizontal(printer, 180);
    PrintTextS(printer, "Las vegas,NV5208\r\n");
    PrintAndFeedLine(printer);
    PrintAndFeedLine(printer);
    PrintTextS(printer, "Ticket #30-57320             User:HAPPY\r\n");
    PrintTextS(printer, "Station:52-102          Sales Rep HAPPY\r\n");
    PrintTextS(printer, "10/10/2019 3:55:01PM\r\n");
    PrintTextS(printer, "---------------------------------------\r\n");
    PrintTextS(printer, "Item         QTY         Price    Total\r\n");
    PrintTextS(printer, "Description\r\n");
    PrintTextS(printer, "---------------------------------------\r\n");
    PrintTextS(printer, "100328       1           7.99      7.99\r\n");
    PrintTextS(printer, "MAGARITA MIX 7           7.99      3.96\r\n");
    PrintTextS(printer, "680015       1          43.99     43.99\r\n");
    PrintTextS(printer, "LIME\r\n");
    PrintTextS(printer, "102501       1          43.99     43.99\r\n");
    PrintTextS(printer, "V0DKA\r\n");
    PrintTextS(printer, "021048       1           3.99      3.99\r\n");
    PrintTextS(printer, "ORANGE 3200Z\r\n");
    PrintTextS(printer, "---------------------------------------\r\n");
    PrintTextS(printer, "Subtobal                          60.93\r\n");
    PrintTextS(printer, "8.1% Sales Tax                     3.21\r\n");
    PrintTextS(printer, "2% Concession Recov                1.04\r\n");
    PrintTextS(printer, "---------------------------------------\r\n");
    PrintTextS(printer, "Total                             66.18\r\n");
    PrintBarCode(printer, 73, "1234567890", 3, 150, 0, 2);
    CutPaperWithDistance(printer, 10);
}

void PrintQRCode(void *printer) {
    printf("PrintQRCode\n");
    PrinterInitialize(printer);

    PrintTextS(printer, "Example qrcode.\r\n");
    PrintSymbol(printer, 49, "123456789", 48, 10, 10, 1);
    SetAlign(printer, 0);

    PrintTextS(printer, "Example PDF417.\r\n");
    PrintSymbol(printer, 48, "123456789", 48, 10, 8, 1);
    SetAlign(printer, 0);
    CutPaperWithDistance(printer, 10);
}

void PrintBarCode(void *printer) {
    printf("PrintBarCode\n");
    PrinterInitialize(printer);

    PrintTextS(printer, "Example UPC_A.\r\n");
    PrintBarCode(printer, 65, "614141999996", 3, 150, 0, 2);

    PrintTextS(printer, "Example UPC_E.\r\n");
    PrintBarCode(printer, 66, "040100002931", 3, 150, 0, 2);

    PrintTextS(printer, "Example JAN13(EAN13).\r\n");
    PrintBarCode(printer, 67, "2112345678917", 3, 150, 0, 2);

    PrintTextS(printer, "Example JAN8(EAN8).\r\n");
    PrintBarCode(printer, 68, "21234569", 3, 150, 0, 2);

    PrintTextS(printer, "Example CODE39.\r\n");
    PrintBarCode(printer, 69, "12345678", 3, 150, 0, 2);

    PrintTextS(printer, "Example ITF.\r\n");
    PrintBarCode(printer, 70, "10614141999993", 3, 150, 0, 2);

    PrintTextS(printer, "Example CODABAR.\r\n");
    PrintBarCode(printer, 71, "B1234567890B", 3, 150, 0, 2);

    PrintTextS(printer, "Example CODE93.\r\n");
    PrintBarCode(printer, 72, "12345678", 3, 150, 0, 2);

    PrintTextS(printer, "Example barcode 128.\r\n");
    PrintBarCode(printer, 73, "1234567890", 3, 150, 0, 2);
    CutPaperWithDistance(printer, 10);
}

void PrintImage(void *printer, char *path) {
    printf("PrintImage %s\n", path);
    PrinterInitialize(printer);
    PrintImage(printer, path, 0);
    CutPaperWithDistance(printer, 10);
}

int main() {
    startTest(GetStatus, PrintSample, PrintQRCode, PrintBarCode, PrintImage);

    return 0;
}