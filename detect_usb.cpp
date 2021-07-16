#include "detect_usb.h"
#define ANSI
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT   0x0501
#include <windows.h>
#include <winuser.h>
#include <Dbt.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#define HID_CLASSGUID {0x4d1e55b2, 0xf16f, 0x11cf,{ 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}}
#define CLS_NAME "DUMMY_CLASS"
#define HWND_MESSAGE     ((HWND)-3)
namespace DetectUSB{
static void (*device_refresh_procedure)(void);
}
LRESULT message_handler(HWND__* hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
{
switch (uint)
{
case WM_NCCREATE:  
return true;
break;
case WM_CREATE:  
{
LPCREATESTRUCT params = (LPCREATESTRUCT) lparam;
GUID InterfaceClassGuid = *((GUID*)params->lpCreateParams);
DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
NotificationFilter.dbcc_classguid = InterfaceClassGuid;
HDEVNOTIFY dev_notify = RegisterDeviceNotification(hwnd, &NotificationFilter,
DEVICE_NOTIFY_WINDOW_HANDLE);
if(dev_notify == NULL)
{
throw std::runtime_error("Could not register for devicenotifications!");
}
break;
}
case WM_DEVICECHANGE:
{
PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR) lparam;
PDEV_BROADCAST_DEVICEINTERFACE lpdbv = (PDEV_BROADCAST_DEVICEINTERFACE) lpdb;
std::string path;
if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
{
path = std::string(lpdbv->dbcc_name);
switch (wparam)
{
case DBT_DEVICEARRIVAL:
std::cout << "new device connected: " << path << "\n";
DetectUSB::device_refresh_procedure();
break;
case DBT_DEVICEREMOVECOMPLETE:
std::cout << "device disconnected: " << path << "\n";
DetectUSB::device_refresh_procedure();
break;
}
}
break;
}
}
return 0L;
}
namespace DetectUSB{
int init(void (*DEVICE_REFRESH_PROCEDURE)(void))
{
device_refresh_procedure=DEVICE_REFRESH_PROCEDURE;
HWND hWnd = NULL;
WNDCLASSEX wx;
ZeroMemory(&wx, sizeof(wx));
wx.cbSize = sizeof(WNDCLASSEX);
wx.lpfnWndProc = reinterpret_cast<WNDPROC>(message_handler);
wx.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
wx.style = CS_HREDRAW | CS_VREDRAW;
wx.hInstance = GetModuleHandle(0);
wx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
wx.lpszClassName = CLS_NAME;
GUID guid = HID_CLASSGUID;
if (RegisterClassEx(&wx))
{
hWnd = CreateWindow(CLS_NAME, "DevNotifWnd", WS_ICONIC,
0, 0, CW_USEDEFAULT, 0, HWND_MESSAGE,
NULL, GetModuleHandle(0), (void*)&guid);
}
if(hWnd == NULL)
{
return 1;
}
std::cout << "waiting for new devices..\n";
return 0;
}
int terminate(){
return 0;
}
}