//
// Created by parallels on 12/11/23.
//

#ifndef LINUXSDK_PRINTER_SDK_ZPL_H
#define LINUXSDK_PRINTER_SDK_ZPL_H

#pragma once

#include "printersdk.h"

PRINTER_API int CALL_STACK ZPL_StartFormat(void* handle);

PRINTER_API int CALL_STACK ZPL_EndFormat(void* handle);

PRINTER_API int CALL_STACK ZPL_ScalableFontText(void* handle, int xPos, int yPos, char fontName, int orientation, int fontWidth, int fontHeight, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_SetUserFontName(void* handle, const TCHAR* text, char alias);

PRINTER_API int CALL_STACK ZPL_Text(void* handle, int xPos, int yPos, int fontNum, int orientation, int fontWidth, int fontHeight, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_Text_Block(void* handle, int xPos, int yPos, int fontNum, int orientation, int fontWidth, int fontHeight, int textblockWidth, int textblockHeight, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_FieldHexadecimalIndicator(void* handle, char symbol);

PRINTER_API int CALL_STACK ZPL_AztecBarcode(void* handle, int xPos, int yPos, int orientation, int dpi, char extChannel, int eccLevel, char menuSymbol, int symbols, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_BarCode11(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char digit, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_BarCode25(void* handle, char type, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char digit, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_BarCode39(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char digit, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_BarCode49(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char mode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_PlanetCode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_Pdf417(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, int securityLevel, int columns, int rows, char truncate, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_CodeEan8(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_UpceCode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_BarCode93(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char digit, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_BarCode128(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char checkDigit, char mode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_CodeEan13(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_MicroPdf417(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, int mode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_AnsiCodebar(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char startChar, char stopChar, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_LogMarsBarcode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char lineAboveCode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_MsiBarcode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAbovecode, char checkDigit, char insertCheck, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_PlesseyBarcode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char checkDigit, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_QRCode(void* handle, int xPos, int yPos, int orientation, int model, int dpi, char eccLevel, char input, char charMode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_UpcExtensions(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_UpcaBarcode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, char digit, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_DataMatrixBarcode(void* handle, int xPos, int yPos, int orientation, int codeHeight, int level, int columns, int rows, int formatId, int aspectRatio, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_SetChangeFontEncoding(void* handle, int encodeType);

PRINTER_API int CALL_STACK ZPL_SetChangeCaret(void* handle, char charactor);

PRINTER_API int CALL_STACK ZPL_PostalBarcode(void* handle, int xPos, int yPos, int orientation, int moduleWidth, int codeHeight, char line, char lineAboveCode, int postalType, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_SetChangeDelimiter(void* handle, char charactor);

PRINTER_API int CALL_STACK ZPL_SetChangeDefaultFont(void* handle, char fontName, int width, int height);

PRINTER_API int CALL_STACK ZPL_SetChangeTilde(void* handle, char charactor);

PRINTER_API int CALL_STACK ZPL_GraphicBox(void* handle, int xPos, int yPos, int width, int height, int thickness, int rounding);

PRINTER_API int CALL_STACK ZPL_GraphicCircle(void* handle, int xPos, int yPos, int diameter, int thickness);

PRINTER_API int CALL_STACK ZPL_GraphicDiagonalLine(void* handle, int xPos, int yPos, char orientation, int width, int height, int thickness);

PRINTER_API int CALL_STACK ZPL_GraphicEllipse(void* handle, int xPos, int yPos, int width, int height, int thickness);

PRINTER_API int CALL_STACK ZPL_PrintImage(void* handle, int xPos, int yPos, const TCHAR* imgName);

PRINTER_API int CALL_STACK ZPL_GraphicSymbol(void* handle, int xPos, int yPos, int orientation, int width, int height, const char symbol);

PRINTER_API int CALL_STACK ZPL_HostStatusReturn(void* handle, unsigned char* statuString);

PRINTER_API int CALL_STACK ZPL_SetDiagnosticsMode(void* handle, int isEnable);

PRINTER_API int CALL_STACK ZPL_SetPowerOnReset(void* handle);

PRINTER_API int CALL_STACK ZPL_SetLabelHome(void* handle, int xPos, int yPos);

PRINTER_API int CALL_STACK ZPL_SetLabelLength(void* handle, int length);

PRINTER_API int CALL_STACK ZPL_SetLabelReversePrint(void* handle, char enable);

PRINTER_API int CALL_STACK ZPL_SetLabelShift(void* handle, int shift);

PRINTER_API int CALL_STACK ZPL_SetLabelTop(void* handle, int top);

PRINTER_API int CALL_STACK ZPL_SetPrintMode(void* handle, char mode, char prePeelSelect);

PRINTER_API int CALL_STACK ZPL_SetMediaTracking(void* handle, char mediaType, int offset);

PRINTER_API int CALL_STACK ZPL_SetMediaType(void* handle, char type);

PRINTER_API int CALL_STACK ZPL_SlewToHomePosition(void* handle, int type);

PRINTER_API int CALL_STACK ZPL_SetPrintingMirrorImage(void* handle, char enable);

PRINTER_API int CALL_STACK ZPL_SetPrintOrientation(void* handle, int orientation);

PRINTER_API int CALL_STACK ZPL_SetPrintQuantity(void* handle, int totalQuantity, int pauseAndCutValue, int replicatesOfEachSerialNumber, char overridePauseCount);

PRINTER_API int CALL_STACK ZPL_SetPrintRate(void* handle, int printSpeed, int slewSpeed, int backfeedSpeed);

PRINTER_API int CALL_STACK ZPL_SetPrintWidth(void* handle, int width);

PRINTER_API int CALL_STACK ZPL_SetSerialCommunications(void* handle, int baudRate, int wordLength, char parity, int stopBits, char protocolMode);

PRINTER_API int CALL_STACK ZPL_SetPrintDarkness(void* handle, int darkness);

PRINTER_API int CALL_STACK ZPL_SetSerializationField(void* handle, int xPos, int yPos, const TCHAR* mask, const TCHAR* increment, const TCHAR* text);

PRINTER_API int CALL_STACK ZPL_SetTearOffAdjustPosition(void* handle, int position);

PRINTER_API int CALL_STACK ZPL_PrintConfigurationLabel(void* handle);

PRINTER_API int CALL_STACK ZPL_GetPrinterIpAddress(void* handle, char* ipAddress);

PRINTER_API int CALL_STACK ZPL_GetPrinterStatus(void* handle, int* Status);

PRINTER_API int CALL_STACK ZPL_GetPrinterOdometer(void* handle, char* meters);

PRINTER_API int CALL_STACK ZPL_GetLabelLength(void* handle, char* length);

PRINTER_API int CALL_STACK ZPL_GetLabelWidth(void* handle, char* width);

PRINTER_API int CALL_STACK ZPL_GetPrinterSeriesNumber(void* handle, char* sn);

PRINTER_API int CALL_STACK ZPL_GetPrinterMacAddress(void* handle, char* macAddress);

PRINTER_API int CALL_STACK ZPL_GetPrinterName(void* handle, char* name);

PRINTER_API int CALL_STACK ZPL_GetPrinterFirmwareVersion(void* handle, char* version);

PRINTER_API int CALL_STACK ZPL_GetPrinterDpi(void* handle, char* dpi);
PRINTER_API int CALL_STACK ZPL_GetPrinterModel(void* handle, char* model);

PRINTER_API int CALL_STACK ZPL_LearnLabel(void* handle);

PRINTER_API int CALL_STACK ZPL_SetReprintAfterError(void* handle, const char* pEnable);

PRINTER_API int CALL_STACK ZPL_PrintDirectoryLabel(void* handle, const char* device, const char* name, const char* type);

PRINTER_API int CALL_STACK ZPL_SetWriteQuery(void* handle, int type);
PRINTER_API int CALL_STACK ZPL_SetPrintDefaultGateway(void* handle, const TCHAR* gateway);
PRINTER_API int CALL_STACK ZPL_SetPrinterBluetoothSSID(void* handle, const TCHAR* ssid);
PRINTER_API int CALL_STACK ZPL_SetPrinterBluetoothPIN(void* handle, const TCHAR* pin);
PRINTER_API int CALL_STACK ZPL_SetPrinterSleepTime(void* handle, int time);
PRINTER_API int CALL_STACK ZPL_SetPrinterShutdownTime(void* handle, int time);
PRINTER_API int CALL_STACK ZPL_FirmwareUpgrade(void* handle, const TCHAR* cFileName, void (*callback)(float));
PRINTER_API int CALL_STACK ZPL_FontDownload(void* handle, const TCHAR* cFileName, void (*callback)(float));
PRINTER_API int CALL_STACK ZPL_SetPrintNetSetting(void* handle, const char* ipaddress, const char* mask, const char* gateway);
PRINTER_API int CALL_STACK ZPL_OpenNetDhcp(void* handle);
PRINTER_API int CALL_STACK ZPL_WifiConfig(void* handle, int dhcp, const char* ipAddress, const char* mask, const char* gateway, const char* ssid, const char* password);
#endif //LINUXSDK_PRINTER_SDK_ZPL_H
