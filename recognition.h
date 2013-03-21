#ifndef RECOGNITION_INCLUDED
#define RECOGNITION_INCLUDED

#ifndef _DEBUG
//#define WITHOUT_RTL
#define TCL_MEMORY_MANAGER
#endif

//#ifndef _DEBUG
//extern "C" int _fltused;
//#endif // !_DEBUG

//#define _DEBUG

#ifdef WITHOUT_RTL
#pragma comment(linker,"/ALIGN:4096")
#pragma comment(linker,"/ENTRY:EntryPoint")
#pragma comment(linker,"/SUBSYSTEM:WINDOWS")
#endif


// Поддержка отладки
#ifdef _DEBUG
#define MESSAGE_ASSERT(expression,string) TCL_MESSAGE_ASSERT(expression,string)
#define ASSERT(expression) CVL_MESSAGE_ASSERT(expression,"Отсутствует")
#include <crtdbg.h>
#else
#define MESSAGE_ASSERT(expression,string)
#define ASSERT(expression)
#endif

#include <tcl\tcl.h>
#include <twl\twl.h>
#include <tml\tml.h>
#include <tgl\tgl.h>

#include <conio.h>

#include <stdio.h>
#include <windows.h>
#include <initguid.h>
#include <SetupAPI.h>
#include <WinIoCtl.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <msxml.h>
#include <WinSock.h>


#define USER_ID 50001
#define WM_DNS_READY WM_TWLUSER+1 //сообщение от диалога о завершении работы
#define CAR_NUMBER_MAX_LENGTH 10





#pragma pack(push,1)

#include "resource.h"

#include "profile.h"

#include "helpwindow.h"
#include "analyzimage.h"

#include "imageprocessing.h"
#include "mainwindow.h"

#include "application.h"


#pragma pack(pop,1)

#endif // PROJECT4B_INCLUDED
