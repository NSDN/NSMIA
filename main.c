#include "ch552.h"
#include "sys.h"
#include "usb.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);
extern uint8_t FLAG, Ready;

__sbit __at (0xB3) LEDA;
__sbit __at (0xB4) LEDB;

__sbit __at (0x91) EKY;
__sbit __at (0x94) K1;
__sbit __at (0x95) K2;
__sbit __at (0x96) K3;
__sbit __at (0x97) K4;

volatile __bit control = 0;

void main() {
    P1_MOD_OC &= ~(0xF0);
    P1_DIR_PU &= ~(0xF0);
    LEDA = 0; LEDB = 0;
    sysClockConfig();
    delay(5);

    usbDevInit();
    EA = 1;
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
    UEP3_T_LEN = 0;

    FLAG = 0;
    Ready = 0;

    LEDA = 1; LEDB = 1;
    uint8_t count = 0;
    while (!Ready) {
        delay(100);
        count += 1;
        if (count > 10)
            break;
    }
    LEDA = 1; LEDB = 0;
    usbReleaseAll();
    usbPushKeydata();
    delay(100);
    LEDA = 0; LEDB = 1;
    delay(100);
    LEDA = 1; LEDB = 0;

    while (1) {
        EKY = 1;
        if (EKY == 0) {
            while (EKY == 0)
                EKY = 1;
            control = !control;
            usbReleaseAll();
            usbPushKeydata();
        }
        
        if (control) {
            usbSetKeycode(2, K1 != 0 ? 0 : 7);
            usbSetKeycode(3, K2 != 0 ? 0 : 9);
            usbSetKeycode(4, K3 != 0 ? 0 : 13);
            usbSetKeycode(5, K4 != 0 ? 0 : 14);
            usbPushKeydata();
        }
        delay(1);

        LEDB = control;
    }
    
}
