/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <pic16f1615.h>        /* For true/false definition */

#include "user.h"
#include "system.h"


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

static void __GPIO_Init(void);
static void __PPS_Init(void);
static void __SPI_Init(void);
static void __PWM_Init(void);
static void __Timer5_Init(void);



void inline SPI_SendByte(uint8_t byte);
void Display_Clear(void);
void Display_PrintInteger(uint16_t num);
static uint8_t __Display_digit_to_mem(uint8_t digit);


display_opts_t display = {
    Display_Clear,
    Display_PrintInteger
};

static inline void __PPS_Unlock(void)
{
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 0;
}

static inline void __PPS_Lock(void)
{
    PPSLOCK = 0x55;
    PPSLOCK = 0xAA;
    PPSLOCKbits.PPSLOCKED = 1;
}

void InitApp(void)
{
    /* TODO Initialize User Ports/Peripherals/Project here */

    /* Setup analog functionality and port direction */
    __GPIO_Init();
    __PPS_Init();
    

    /* Initialize peripherals */
    __SPI_Init();
    __Timer5_Init();
    __PWM_Init();
    
    /* Enable interrupts */
    PIR5bits.TMR5IF = 0;
    PIE5bits.TMR5IE = 1;
    
    IOCAF = 0x00;
    IOCAN |= 0x33;
    IOCAP |= 0x33;
    
    INTCONbits.IOCIF = 0;
    INTCONbits.IOCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
}

static void __GPIO_Init(void)
{
    ANSELA = 0x00;
    ANSELC = 0x00;
    
    /* Configure SS */
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    
    LATCbits.LATC1 = 1;
    
    /* Configure buttons */
    TRISA |= 0x30;
    
    /* Configure count inputs */
    TRISA |= 0x03;
    
    /* Configure PWM outputs*/
    TRISC &= ~(0x18);
}

/**
 * 
 */
static void __PPS_Init(void)
{
    __PPS_Unlock();
    
    RC0PPS = 0x10;      // SCK
    RC2PPS = 0x11;      // SDO
    RC3PPS = 0x0E;      // PWM3_out
    RC4PPS = 0x0F;      // PWM4_out
    
    __PPS_Lock();
    
}
static void __SPI_Init(void)
{
    SSP1STAT = 0x40;
    SSP1CON1 = 0x22;     // SPI Master, FOSC/64
    SSP1ADD = 0x00;
}

void inline SPI_SendByte(uint8_t byte)
{
    SSP1CON1bits.WCOL = 0;      // Clear the Write Collision flag, to allow writing
    SSPBUF = byte;
    
    while(!SSP1STATbits.BF);    // Wait transmission
}

static void __Timer5_Init(void)
{
    T5CON = 0x01;
}

static void __PWM_Init(void)
{
    PWM3CON = 0;
    PWM4CON = 0;
    
    PR2 = 0xFF;
    
    PWM3DCH = 0;
    PWM3DCL &= ~(0xC0);
    
    PWM4DCH = 0;
    PWM4DCL &= ~(0xC0);
    
    T2CONbits.CKPS = 0x07;  // 1:64
    T2CONbits.ON = 1;       // Enable
    
    PWM3CONbits.EN = 1;
    PWM4CONbits.EN = 1;
    
    PWM3CONbits.OUT = 1;
    PWM4CONbits.OUT = 1;
    
#ifdef __SIM__
    PWM3DCH = 0x50;
#endif
    
}


static uint8_t __Display_digit_to_mem(uint8_t digit)
{
    switch(digit) {
        case 0:
            return DISPLAY_SECTION_A & 
                    DISPLAY_SECTION_B & 
                    DISPLAY_SECTION_C &
                    DISPLAY_SECTION_D & 
                    DISPLAY_SECTION_E & 
                    DISPLAY_SECTION_F;
        case 1:
            return DISPLAY_SECTION_B & 
                    DISPLAY_SECTION_C;
        case 2:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_B &
                    DISPLAY_SECTION_G &
                    DISPLAY_SECTION_E &
                    DISPLAY_SECTION_D;
        case 3:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_B &
                    DISPLAY_SECTION_C &
                    DISPLAY_SECTION_D &
                    DISPLAY_SECTION_G;
        case 4:
            return DISPLAY_SECTION_F &
                    DISPLAY_SECTION_B &
                    DISPLAY_SECTION_G &
                    DISPLAY_SECTION_C;
        case 5:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_F &
                    DISPLAY_SECTION_G &
                    DISPLAY_SECTION_C &
                    DISPLAY_SECTION_D;
        case 6:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_C &
                    DISPLAY_SECTION_D &
                    DISPLAY_SECTION_E &
                    DISPLAY_SECTION_F &
                    DISPLAY_SECTION_G;
        case 7:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_B &
                    DISPLAY_SECTION_C;
        case 8:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_B &
                    DISPLAY_SECTION_C &
                    DISPLAY_SECTION_D &
                    DISPLAY_SECTION_E &
                    DISPLAY_SECTION_F &
                    DISPLAY_SECTION_G;
        case 9:
            return DISPLAY_SECTION_A &
                    DISPLAY_SECTION_B &
                    DISPLAY_SECTION_C &
                    DISPLAY_SECTION_D &
                    DISPLAY_SECTION_F &
                    DISPLAY_SECTION_G;
        default:
            break;
    }
    return 0xFF;
}

void Display_Clear(void)
{
    video_buffer.buffer2[0] = 0xFF;
    video_buffer.buffer2[1] = 0xFF;
    video_buffer.buffer2[2] = 0xFF;
    video_buffer.buffer2[3] = 0xFF;
}
void Display_PrintInteger(uint16_t num)
{
    uint8_t *p = &video_buffer.buffer2[3];
 
    num %= 10000;
    Display_Clear();
    
    do {
        *p = __Display_digit_to_mem(num % 10);
        --p;
        num /= 10;
    }while(num > 0);
}
