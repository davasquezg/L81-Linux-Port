//
// Created by dan on 23-11-22.
//

#ifndef PRINTERSDK_H
#define PRINTERSDK_H
#define LINUX

#ifdef LINUX
#define PRINTER_API __attribute__ ( (visibility( "default" ) ) )
#define CALL_STACK
#define TCHAR char
#else
#include <tchar.h>

#ifdef EXPORT_CDECL
#define CALL_STACK __cdecl
#else
#define CALL_STACK //__stdcall
#endif
#ifndef PRINTER_API
#ifdef __cplusplus
#define PRINTER_API extern "C" __declspec( dllexport )
#else
#define PRINTER_API __declspec( dllexport )
#endif
#endif
#endif

#define ERROR_CM_SUCCESS					 0		/* No error.*/
#define ERROR_CM_INVALID_PARAMETER			-1
#define ERROR_CM_INVALID_HANDLE				-2
#define ERROR_CM_NOT_IMPLEMENTED			-3
#define ERROR_CM_INSUFFICIENT_MEMORY		-4
#define ERROR_IMAGE_LOAD_FAILED				-5
#define ERROR_IMAGE_INVALID_FORMAT			-6
#define ERROR_IO_INVALID_HANDLE				-7
#define ERROR_IO_OPEN_FAILED				-8
#define ERROR_IO_WRITE_FAILED				-9
#define ERROR_IO_WRITE_TIMEOUT				-10
#define ERROR_IO_READ_FAILED				-11
#define ERROR_IO_READ_TIMEOUT				-12
#define ERROR_IO_SERIAL_INVALID_BAUDRATE	-13
#define ERROR_IO_SERIAL_INVALID_HANDSHAKE	-14
#define ERROR_IO_EHTERNET_CONNECT_ABORT     -15
#define ERROR_IO_INVALID_USB_PATH	        -16
#define ERROR_IO_USB_DEVICE_NOT_FOUND	    -17
#define ERROR_IO_UPDATE_FAIL                -18
#define ERROR_CM_BUFFER_TOO_SMALL			-19
#define ERROR_FILE_SEND_FAILE				-20
#define ERROR_OPEN_REGISTRY_FAIL			-21
#define ERROR_LIST_PORTS_FAIL   			-22
#define ERROR_NOT_SUPPORT					-23

typedef struct _PRINTERINFO
{
    TCHAR		ModelName[32];
    TCHAR		PortNumber[260];
}PRINTERINFO, * PPRINTERINFO;

#ifdef __cplusplus
extern "C" {
#endif

PRINTER_API void* InitPrinter(const TCHAR* model);

PRINTER_API int ReleasePrinter(void* handle);

PRINTER_API int OpenPort(void* handle, const TCHAR* setting);
PRINTER_API int OpenPortSync(void* handle, const TCHAR* setting);

PRINTER_API int ClosePort(void* handle);

PRINTER_API int WriteData(void* handle, unsigned char* buffer, int size);
int WriteCharData(void* handle, unsigned char* buffer, int size);

PRINTER_API int ReadData(void* handle, unsigned char* buffer, unsigned int size);
PRINTER_API int ReadDataLoop(void* handle, unsigned char* buffer, unsigned int size);

PRINTER_API int ListPrinters(unsigned char* buffer, unsigned int size, unsigned int* needSize);
PRINTER_API int ListComPorts(unsigned char* buffer, unsigned int size, unsigned int* needSize);
PRINTER_API int Verifycmd(void* handle, unsigned char* expectedcmd, int size);
PRINTER_API const char* GetSdkVer();
PRINTER_API const char* GetCopyRight();
int Wchar2Char(char* charStr, const TCHAR* wcharStr);

#ifdef __cplusplus
}
#endif

#endif
