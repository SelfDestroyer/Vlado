/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>
#include <pic16f1615.h>       /* For true/false definition */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

video_buffer_t video_buffer = { 
    { 0xFF, 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF },
    1
};

gd_t gd = {
    0x00, 0x0000, 
    { {0x0000, &PWM3DCH}, {0x0000, &PWM4DCH}}
};




/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();
    
    while(1)
    {
        switch(gd.buttons.buttons) {
            case 0x01:
                if(gd.position < 9999)
                    gd.position += 1;
                break;
            case 0x02:
                if(gd.position > 0)
                    gd.position -= 1;
                break;
            case 0x03:
                gd.position = 0x00;
                break;
        }
        display.integer(gd.position);
        __delay_ms(50);
    }

}

