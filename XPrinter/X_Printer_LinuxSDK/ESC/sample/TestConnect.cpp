//
// Created by dan on 23-12-21.
//
#include <iostream>
#include <limits>
#include "TestConnect.h"
#include "printersdk.h"

using namespace std;

int inputIndex(unsigned int maxIndex) {
    int index = 0;
    printf("Please input index:");
    cin >> index;
    if (cin.fail() || index < 0 || index > maxIndex) {
        cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please try again." << std::endl;
        return inputIndex(maxIndex);
    }
    return index;
}

bool ConnectUsbDevice(void *printer) {
    unsigned int size = 0;
    ListPrinters(nullptr, size, &size);
    int perSize = sizeof(PRINTERINFO);
    if (size <= 0) {
        printf("Printer device not found\n");
        return false;
    }
    auto *buffer = (unsigned char *) malloc(size);
    ListPrinters(buffer, size, &size);
    int index = 0;
    if (size > perSize) {
        printf("-----------------------------------------\n");
        while (index < size) {
            auto pi = (PPRINTERINFO) (buffer + index);
            cout << (index / perSize) << "->" << "path:" << pi->PortNumber << " ModelName:" << pi->ModelName
                 << endl;
            index += perSize;
        }
        index = inputIndex(size / perSize - 1);
    }

    char setting[300];
    auto pi = (PPRINTERINFO) (buffer + index * perSize);
    sprintf(setting, "USB,%s", pi->PortNumber);
    free(buffer);
    return OpenPort(printer, setting) == ERROR_CM_SUCCESS;
}

bool ConnectComDevice(void *printer) {
    unsigned int size = 0;
    ListComPorts(nullptr, size, &size);
    int perSize = sizeof(PRINTERINFO);
    if (size < perSize) {
        printf("Printer device not found\n");
        return false;
    }
    auto *buffer = (unsigned char *) malloc(size);
    ListComPorts(buffer, size, &size);
    int index = 0;
    if (size > perSize) {
        printf("-----------------------------------------\n");
        while (index < size) {
            auto pi = (PPRINTERINFO) (buffer + index);
            cout << (index / perSize) << " -> " << "path:" << pi->PortNumber << " ModelName:" << pi->ModelName << endl;
            index += perSize;
        }
        printf("-----------------------------------------\n");
        index = inputIndex(size / perSize - 1);
    }

    int baud;
    printf("Please input baud:");
    cin >> baud;
    char setting[300];
    auto pi = (PPRINTERINFO) (buffer + index * perSize);
    sprintf(setting, "COM,%s,%d", pi->PortNumber, baud);
    free(buffer);
    return OpenPort(printer, setting) == ERROR_CM_SUCCESS;
}

bool ConnectNetDevice(void *printer) {
    char ip[20];
    printf("Please input ipaddress:");
    cin >> ip;
    char setting[300];
    sprintf(setting, "NET,%s", ip);
    return OpenPort(printer, setting) == ERROR_CM_SUCCESS;
}

bool ConnectLPT(void *printer) {
    char path[100];
    printf("Please input lpt path:");
    cin >> path;
    char setting[300];
    sprintf(setting, "LPT,%s", path);
    return OpenPort(printer, setting) == ERROR_CM_SUCCESS;
}

int PrintConnectMenu() {
    int index;
    printf("****************************************************\n");
    printf("*    1.USB    2.NET    3.COM    4.LPT    0.Exit    *\n");
    printf("****************************************************\n");
    printf("Please choose:");
    cin >> index;
    if (cin.fail() || index < 0 || index > 4) {
        cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please try again." << std::endl;
        return PrintConnectMenu();
    }

    return index;
}

int PrintSampleMenu() {
    int index;
    printf("****************************************************\n");
    printf("* 1.Get Status     2.Print Sample   3.Print QRCode *\n");
    printf("* 4.Print Barcode  5.Print Image    0.Exit         *\n");
    printf("****************************************************\n");
    printf("Please choose sample:");
    cin >> index;
    if (cin.fail() || index < 0 || index > 5) {
        cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid input. Please try again." << std::endl;
        return PrintSampleMenu();
    }
    return index;
}

void startTest(void(*GetStatus)(void *printer),
               void(*PrintSample)(void *printer),
               void(*PrintQRCode)(void *printer),
               void(*PrintBarCode)(void *printer),
               void(*PrintImage)(void *printer, char *path)) {
    void *printer = InitPrinter("");
    while (true) {
        int connectType = PrintConnectMenu();
        bool isConnect = false;
        if (connectType == 0) {
            ReleasePrinter(printer);
            break;
        } else if (connectType == 1) {
            isConnect = ConnectUsbDevice(printer);
        } else if (connectType == 2) {
            isConnect = ConnectNetDevice(printer);
        } else if (connectType == 3) {
            isConnect = ConnectComDevice(printer);
        } else if (connectType == 4) {
            isConnect = ConnectLPT(printer);
        }
        if (isConnect) {
            while (true) {
                int type = PrintSampleMenu();
                if (type == 0) {
                    ClosePort(printer);
                    break;
                } else if (type == 1) {
                    if (connectType == 4) {
                        printf("Parallel port does not support read operations\n");
                    } else {
                        GetStatus(printer);
                    }
                } else if (type == 2) {
                    PrintSample(printer);
                } else if (type == 3) {
                    PrintQRCode(printer);
                } else if (type == 4) {
                    PrintBarCode(printer);
                } else if (type == 5) {
                    char path[100];
                    printf("Please input image path:");
                    cin >> path;
                    PrintImage(printer, path);
                }
            }
        } else {
            printf("Connect Fail!\n");
        }
    }


}