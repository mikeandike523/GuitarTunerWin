#ifndef DETECT_USB_H
#define DETECT_USB_H
namespace DetectUSB{
int init(void (*device_refresh_procedure)(void));
int terminate();
}
#endif