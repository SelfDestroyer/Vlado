/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define SS_DIR          TRISCbits.TRISC1
#define SS_LAT          LATCbits.LATC1
#define ASSERT_SS()     SS_LAT=0
#define DEASSERT_SS()   SS_LAT=1

#define DISPLAY_SECTION_A   (~(0x01))
#define DISPLAY_SECTION_B   (~(0x02))
#define DISPLAY_SECTION_C   (~(0x04))
#define DISPLAY_SECTION_D   (~(0x08))
#define DISPLAY_SECTION_E   (~(0x10))
#define DISPLAY_SECTION_F   (~(0x20))
#define DISPLAY_SECTION_G   (~(0x40))


#define BUTTON1             (PORTAbits.RA4)
#define BUTTON2             (PORTAbits.RA5)

#define CNT1                (PORTAbits.RA0)
#define CNT2                (PORTAbits.RA1)
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/

typedef struct {
    uint8_t buffer1[4];
    uint8_t buffer2[4];
    uint8_t digit;
}video_buffer_t;

typedef struct {
    void (*clear)(void);
    void (*integer)(uint16_t num);
}display_opts_t;

typedef union {
    uint8_t buttons;
    struct {
        unsigned btn1   : 1;
        unsigned btn2   : 1;
    };    
}buttons_t;

typedef struct {
    uint16_t position;
    volatile uint8_t *speed;
}motor_t;

typedef struct {
    buttons_t buttons;
    uint16_t position;
    motor_t motors[2];
}gd_t;

extern video_buffer_t video_buffer;
extern display_opts_t display;
extern gd_t gd;

void InitApp(void);         /* I/O and Peripheral Initialization */
void inline SPI_SendByte(uint8_t byte);
