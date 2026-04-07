//
// Created by parallels on 12/11/23.
//

#ifndef LINUXSDK_PRINTER_SDK_CPCL_H
#define LINUXSDK_PRINTER_SDK_CPCL_H

#pragma once

#include "printersdk.h"

PRINTER_API int CALL_STACK CPCL_AddLabel(void* handle, int offSet, int height, int qty);
PRINTER_API int CALL_STACK CPCL_SetAlign(void* handle, int align);
PRINTER_API int CALL_STACK CPCL_AddText(void* handle, int rotate, const char* fontType, int fontSize, int xPos, int yPos, const TCHAR* data);
PRINTER_API int CALL_STACK CPCL_AddBarCode(void* handle, int rotate, int type, int width, int ratio, int height, int xPos, int yPos, const TCHAR* data);
PRINTER_API int CALL_STACK CPCL_AddBarCodeText(void* handle, int enable, int fontType, int fontSize, int offSet);
PRINTER_API int CALL_STACK CPCL_AddQRCode(void* handle, int rotate, int xPos, int yPos, int model, int unitWidth, int eccLevel, const TCHAR* data);
PRINTER_API int CALL_STACK CPCL_AddPDF417(void* handle, int rotate, int xPos, int yPos, int xDots, int yDots, int columns, int eccLevel, const TCHAR* data);
PRINTER_API int CALL_STACK CPCL_AddBox(void* handle, int xPos, int yPos, int endXPos, int endYPos, int thickness);
PRINTER_API int CALL_STACK CPCL_AddLine(void* handle, int xPos, int yPos, int endXPos, int endYPos, int thickness);
PRINTER_API int CALL_STACK CPCL_AddImage(void* handle, int rotate, int xPos, int yPos, const TCHAR* imagePath);
PRINTER_API int CALL_STACK CPCL_AddImageData(void* handle, int rotate, int widthBytes, int height, int xPos, int yPos, const char* data);
PRINTER_API int CALL_STACK CPCL_AddInverseLine(void* handle, int x, int y, int xEnd, int yEnd, int width);

/* Setting */
PRINTER_API int CALL_STACK CPCL_SetFontSize(void* handle, int width, int height);
PRINTER_API int CALL_STACK CPCL_SetDensity(void* handle, int density);
PRINTER_API int CALL_STACK CPCL_SetSpeed(void* handle, int speed);
PRINTER_API int CALL_STACK CPCL_SetTextSpacing(void* handle, int spacing);
PRINTER_API int CALL_STACK CPCL_SetLeftMargin(void* handle, int margin);
PRINTER_API int CALL_STACK CPCL_SetTextBold(void* handle, int bold);
PRINTER_API int CALL_STACK CPCL_SetTextUnderline(void* handle, int underline);

/* control */
PRINTER_API int CALL_STACK CPCL_Abort(void* handle);
PRINTER_API int CALL_STACK CPCL_Print(void* handle);
PRINTER_API int CALL_STACK CPCL_NextLabelPos(void* handle); /* FORM */
PRINTER_API int CALL_STACK CPCL_PreFeed(void* handle, int distance);
PRINTER_API int CALL_STACK CPCL_PostFeed(void* handle, int distance);
PRINTER_API int CALL_STACK CPCL_GetPrinterStatus(void* handle, int* Status);
PRINTER_API int CALL_STACK CPCL_AddImage1(void* handle, int rotate, int xPos, int yPos, const TCHAR* imagePath);

#endif //LINUXSDK_PRINTER_SDK_CPCL_H
