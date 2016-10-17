/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#endif

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "user.h"
#include "system.h"


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

static void __GPIO_Init(void);
static void __PPS_Init(void);
static void __SPI_Init(void);
static void __Timer5_Init(void);

void inline SPI_SendByte(uint8_t byte);

void Display_Clear(void);
void Display_PrintInteger(uint16_t num);
static uint8_t __Display_digit_to_mem(uint8_t digit);

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
    
    /* Enable interrupts */
    PIR5bits.TMR5IF = 0;
    PIE5bits.TMR5IE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
}

static void __GPIO_Init(void)
{
    /* Configure SS */
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    
    LATCbits.LATC1 = 1;
}
static void __PPS_Init(void)
{
    __PPS_Unlock();
    
    RC0PPS = 0x10;      // SCK
    RC2PPS = 0x11;      // SDO
    
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
