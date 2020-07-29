#include "ch552.h"
#include "sys.h"
#include "usb.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);
extern uint8_t FLAG, Ready;

__sbit __at (0xB3) LEDA;
__sbit __at (0xB4) LEDB;

void main() {
    LEDA = 0; LEDB = 0;
    sysClockConfig();
    delay(500);

    usbDevInit();

    LEDB = 1; delay(100);
    LEDB = 0; delay(100);

    EA = 1;

    LEDB = 1; delay(100);
    LEDB = 0; delay(100);

    UEP1_T_LEN = 0;
    UEP2_T_LEN = 0;
    UEP3_T_LEN = 0;

    FLAG = 0;
    Ready = 0;

    if ((CLOCK_CFG & 0x04) != 0)
        LEDB = 1;

    delay(1000);

    while (1) {
        if (Ready) {
            usbHIDTestSend('Q');
            delay(500);
            LEDA = !LEDA;
            usbHIDTestSend('A');
            delay(500);
            LEDA = !LEDA;
            usbHIDTestSend('Q');
            delay(500);
            LEDA = !LEDA;
            usbHIDTestSend('X');
        }
        delay(100);
        LEDA = !LEDA;
    }
    
}
