//
// Created by parallels on 12/6/23.
//

#ifndef LINUXSDK_PRINTER_SDK_ESC_H
#define LINUXSDK_PRINTER_SDK_ESC_H

#pragma once

#include "printersdk.h"


PRINTER_API int CALL_STACK PrinterInitialize(void* hPrinter);
PRINTER_API int CALL_STACK PrintSelfTest(void* hPrinter);
PRINTER_API int CALL_STACK FeedLine(void* hPrinter, int lines);
PRINTER_API int CALL_STACK PrintAndFeedLine(void* hPrinter);
PRINTER_API int CALL_STACK SetAlign(void* hPrinter, int align);
PRINTER_API int CALL_STACK PrintText(void* hPrinter, const char* data, int alignment, int textSize);
PRINTER_API int CALL_STACK PrintTextS(void* hPrinter, const char* data);
PRINTER_API int CALL_STACK SetRelativeHorizontal(void* hPrinter, int position);
PRINTER_API int CALL_STACK PrintBarCode(void* hPrinter, int bcType, const char* data, int width, int height, int alignment, int hriPosition);
PRINTER_API int CALL_STACK PrintSymbol(void* hPrinter, int type, const char* data, int errLevel, int width, int height, int alignment);
PRINTER_API int CALL_STACK CutPaperWithDistance(void* hPrinter, int distance);
PRINTER_API int CALL_STACK CutPaper(void* hPrinter, int cutMode);
PRINTER_API int CALL_STACK OpenCashDrawer(void* hPrinter, int pinMode, int onTime, int ofTime);
PRINTER_API int CALL_STACK SelectStandardMode(void* hPrinter);
PRINTER_API int CALL_STACK SetTextLineSpace(void* hPrinter, int lineSpace);
PRINTER_API int CALL_STACK SetCharacterPitch(void* hPrinter, int space);
PRINTER_API int CALL_STACK SetTextBold(void* hPrinter, int bold);
PRINTER_API int CALL_STACK SetTextFont(void* hPrinter, int font);
PRINTER_API int CALL_STACK SelectPageMode(void* hPrinter);
PRINTER_API int CALL_STACK SetPrintAreaInPageMode(void* hPrinter, int horizontal, int vertical, int width, int height);
PRINTER_API int CALL_STACK CancelPrintDataInPageMode(void* hPrinter);
PRINTER_API int CALL_STACK SelectPrintDirectionInPageMode(void* hPrinter, int direction);
PRINTER_API int CALL_STACK SetAbsolutePrintPosition(void* hPrinter, int position);
PRINTER_API int CALL_STACK SetAbsoluteVerticalPrintPositionInPageMode(void* hPrinter, int position);
PRINTER_API int CALL_STACK PrintAndReturnStandardMode(void* hPrinter);
PRINTER_API int CALL_STACK PositionNextLabel(void* hPrinter);
PRINTER_API int CALL_STACK PrintDataInPageMode(void* hPrinter);
PRINTER_API int CALL_STACK GetPrinterState(void* hPrinter, unsigned int* printerStatus);
PRINTER_API int CALL_STACK SetHorizontalAndVerticalMotionUnits(void* hPrinter, int horizontal, int vertical);
PRINTER_API int CALL_STACK SetCodePage(void* hPrinter, int codepage);
PRINTER_API int CALL_STACK SetInternationalCharacter(void* hPrinter, int characterSet);
PRINTER_API int CALL_STACK SetPrintSpeed(void* hPrinter, int speed);
PRINTER_API int CALL_STACK DefineNVImageCompatible(void* hPrinter, const char** fileNameList, int imageQty);
PRINTER_API int CALL_STACK PrintNVImageCompatible(void* hPrinter, int imgNo, int scaleMode);
PRINTER_API int CALL_STACK PrintDownloadedImageCompatible(void* hPrinter, int scaleMode);
PRINTER_API int CALL_STACK PrintImage(void* hPrinter, const char* imagePath, int scaleMode);
PRINTER_API int CALL_STACK PrintNVImage(void* hPrinter, unsigned char kc1, unsigned char kc2);
PRINTER_API int CALL_STACK EnableBlackMark(void* hPrinter, int enable);
PRINTER_API int CALL_STACK GoHomeWithBlackMark(void* hPrinter);
PRINTER_API int CALL_STACK SetBlackMarkDistance(void* hPrinter, int distance);
PRINTER_API int CALL_STACK SetBlackMarkHeight(void* hPrinter, int height);
PRINTER_API int CALL_STACK SetBlackMarkAdjust(void* hPrinter, int func, int direction, int offset);
PRINTER_API int CALL_STACK DefineUserDefinedCharacters(void* hPrinter, unsigned char* data, int size);
PRINTER_API int CALL_STACK DeleteUserDefinedCharacter(void* hPrinter, int n);
PRINTER_API int CALL_STACK EnableUserDefinedCharacter(void* hPrinter, bool enable);
PRINTER_API int CALL_STACK FirmwareUpgrade(void* hPrinter, const char* cFileName, void (*progressCallback)(float));
#endif //LINUXSDK_PRINTER_SDK_ESC_H
