//
// Created by dan on 23-12-21.
//

#ifndef ESCDEMO_TESTCONNECT_H
#define ESCDEMO_TESTCONNECT_H

void startTest(void(*GetStatus)(void*printer),
               void(*PrintSample)(void*printer),
               void(*PrintQRCode)(void*printer),
               void(*PrintBarCode)(void*printer),
               void(*PrintImage)(void*printer, char* path));

#endif //ESCDEMO_TESTCONNECT_H
