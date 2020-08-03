#include "ch552.h"
#include "sys.h"
#include "usb.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);
extern uint8_t FLAG;

__sbit __at (0xB3) LEDA;
__sbit __at (0xB4) LEDB;

__sbit __at (0xB0) CKP;
__sbit __at (0xB2) CKU;
__sbit __at (0xB1) CKD;
__sbit __at (0x91) EKY;
__sbit __at (0x94) K1;
__sbit __at (0x95) K2;
__sbit __at (0x96) K3;
__sbit __at (0x97) K4;

volatile __bit control = 0;
volatile uint8_t prevKey = 0;

void pressEscapeKey() {
    usbReleaseAll();
    usbSetKeycode(0, 1);                            // Report ID 1
    usbSetKeycode(9, 41);                           // KEY_ESC
    usbPushKeydata();
    delay(100);
    usbReleaseAll();
    usbSetKeycode(0, 1);                            // Report ID 1
    usbSetKeycode(9, 0);                            // KEY_ESC
    usbPushKeydata();
}

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

    FLAG = 1;

    LEDA = 1; LEDB = 0;
    delay(500);
    LEDA = 0; LEDB = 1;
    usbReleaseAll();
    usbPushKeydata();
    requestHIDData();

    while (1) {
        EKY = 1;
        delay_us(10);
        if (EKY == 0) {
            delay(10);
            EKY = 1;
            delay_us(10);
            if (EKY == 0) {
                while (EKY == 0) {
                    EKY = 1;
                    delay_us(10);
                }
                control = !control;
                //pressEscapeKey();
            }
        }
        
        if (!control) {
            usbReleaseAll();
            usbSetKeycode(0, 1);                    // Report ID 1
            usbSetKeycode(1, 0);                    // NO CONTROL
            usbSetKeycode(9, 0);                    // NO ESCAPE

            uint8_t val = 0;
            val |= (K1 != 0 ? 0 : 0x01);
            val |= (K2 != 0 ? 0 : 0x02);
            val |= (K3 != 0 ? 0 : 0x04);
            val |= (K4 != 0 ? 0 : 0x08);
            usbSetKeycode(2, K1 != 0 ? 0 : 7);      // KEY_D
            usbSetKeycode(3, K2 != 0 ? 0 : 9);      // KEY_F
            usbSetKeycode(4, K3 != 0 ? 0 : 13);     // KEY_J
            usbSetKeycode(5, K4 != 0 ? 0 : 14);     // KEY_K

            CKP = 1; CKU = 1; CKD = 1;
            delay_us(10);
            val |= (CKP != 0 ? 0 : 0x10);
            val |= (CKU != 0 ? 0 : 0x20);
            val |= (CKD != 0 ? 0 : 0x40);
            usbSetKeycode(6, CKP != 0 ? 0 : 40);    // KEY_ENTER
            usbSetKeycode(7, CKU != 0 ? 0 : 82);    // KEY_UP
            usbSetKeycode(8, CKD != 0 ? 0 : 81);    // KEY_DOWN

            if (val != prevKey) {
                prevKey = val;
                usbPushKeydata();
            }
        } else {
            usbReleaseAll();
            usbSetKeycode(0, 2);                    // Report ID 2
            usbSetKeycode(2, 0);
            uint8_t val = 0;
                 if (K1 == 0) val = 0xCD;           // KEY_PLAY
            else if (K2 == 0) val = 0xCC;           // KEY_STOP
            else if (K3 == 0) val = 0xB6;           // KEY_PREV
            else if (K4 == 0) val = 0xB5;           // KEY_NEXT
            if (val > 0) {
                usbSetKeycode(1, val);
                usbPushKeydata();
                delay(100);
                usbSetKeycode(1, 0);
                usbPushKeydata();
                delay(100);
            }
        }

        if (hasHIDData()) {
            for (uint8_t i = 0; i < 32; i++)
                setHIDData(i, getHIDData(i));
            pushHIDData();
            requestHIDData();
            LEDB = !LEDB;
        }

        LEDA = control;
    }
    
}
