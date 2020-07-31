#include "ch552.h"
#include "sys.h"
#include "usb.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);
extern uint8_t FLAG;

__sbit __at (0xB3) LEDA;
__sbit __at (0xB4) LEDB;

__sbit __at (0xB0) CKP;
__sbit __at (0xB1) CKU;
__sbit __at (0xB2) CKD;
__sbit __at (0x91) EKY;
__sbit __at (0x94) K1;
__sbit __at (0x95) K2;
__sbit __at (0x96) K3;
__sbit __at (0x97) K4;

volatile __bit control = 1;

void main() {
    P1_MOD_OC &= ~(0xF0);
    P1_DIR_PU &= ~(0xF0);
    LEDA = 0; LEDB = 0;
    sysClockConfig();
    delay(5);

    LEDA = 0; LEDB = 1;
    usbDevInit();
    EA = 1;
    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
    UEP3_T_LEN = 0;

    FLAG = 1;

    LEDA = 1; LEDB = 0;
    delay(3000);
    LEDA = 0; LEDB = 1;
    usbReleaseAll();
    usbPushKeydata();
    requestHIDData();
    LEDA = 1; LEDB = 0;

    while (1) {
        EKY = 1;
        if (EKY == 0) {
            while (EKY == 0)
                EKY = 1;
            //control = !control;
            usbReleaseAll();
            usbSetKeycode(9, 41);                   // KEY_ESC
            usbPushKeydata();
            delay(100);
            usbReleaseAll();
            usbPushKeydata();
        }
        
        usbSetKeycode(0, 0);                        // NO CONTROL
        usbSetKeycode(9, 0);                        // NO ESCAPE

        usbSetKeycode(2, K1 != 0 ? 0 : 7);          // KEY_D
        usbSetKeycode(3, K2 != 0 ? 0 : 9);          // KEY_F
        usbSetKeycode(4, K3 != 0 ? 0 : 13);         // KEY_J
        usbSetKeycode(5, K4 != 0 ? 0 : 14);         // KEY_K
        
        CKP = 1; CKU = 1; CKD = 1;
        delay_us(10);
        if (control) {
            usbSetKeycode(6, CKP != 0 ? 0 : 40);    // KEY_ENTER
            usbSetKeycode(7, CKD != 0 ? 0 : 82);    // KEY_UP
            usbSetKeycode(8, CKU != 0 ? 0 : 81);    // KEY_DOWN
            usbSetKeycode(1, 0);                    // NO MEDIA
        } else {
            uint8_t val = 0;
            val |= CKP != 0 ? 0 : 0x01;             // KEY_PLAY
            val |= CKD != 0 ? 0 : 0x08;             // KEY_PREV
            val |= CKU != 0 ? 0 : 0x04;             // KEY_NEXT
            usbSetKeycode(1, val);
        }

        usbPushKeydata();

        if (hasHIDData()) {
            for (uint8_t i = 0; i < 32; i++)
                setHIDData(i, getHIDData(i));
            pushHIDData();
            requestHIDData();
            LEDA = !LEDA;
        }

        LEDB = control;
    }
    
}
