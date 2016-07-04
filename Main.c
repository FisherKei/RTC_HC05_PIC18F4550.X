#define     _XTAL_FREQ      20000000UL

#define     LCD_D4  PORTDbits.RD4
#define     LCD_D5  PORTDbits.RD5
#define     LCD_D6  PORTDbits.RD6
#define     LCD_D7  PORTDbits.RD7
#define     LCD_RS  PORTBbits.RB2
#define     LCD_RW  PORTBbits.RB3
#define     LCD_EN  PORTBbits.RB4

#define     BUFFER_SIZE     16
#define     DELAY_LIMIT     4000

// PIC18F4550 Configuration Bit Settings

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = OFF     // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF      // MCLR Pin Enable bit (RE3 input pin enabled; MCLR pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)


#include    <xc.h>
#include    "LCD.h"
#include    "UART.h"
#include    "I2C.h"
#include    "DS1307.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h> 


void    delay_ms(unsigned int duration)
{
    for(int i = 0; i < duration; ++i)
        __delay_ms(1);
}


unsigned char   uartBuffer[BUFFER_SIZE];
unsigned int    i = 0;
char isSetClock = 0;
void    clearBuffer(unsigned char buffer[], unsigned int bufferSize)
{
    for(int i = 0; i < bufferSize; ++i)
        buffer[i] = 0x00;
}


void    interrupt   UART_ISR(void)
{
    if (PIE1bits.RCIE && PIR1bits.RCIF) {
        if (i < BUFFER_SIZE) 
        {
            char c = UART_Read();
            if (c == 0x0D) 
            {
                isSetClock = 1;
                ds1307_setData(uartBuffer);
                clearBuffer(uartBuffer, BUFFER_SIZE);
                i = 0;
                LCD_Clear();
                LCD_Set_Cursor(1, 1);
                LCD_Write_String("Setting Clock...");
                delay_ms(500);
                PIR1bits.RCIF = 0x0;
                return;
            } else if(c == '\n')
            {
                isSetClock = 0;
                LCD_Clear();
                LCD_Set_Cursor(1, 1);
                LCD_Write_String("Received String:");
                LCD_Set_Cursor(2, 1);
                LCD_Write_String(uartBuffer);            
                clearBuffer(uartBuffer, BUFFER_SIZE);
                i = 0;
                delay_ms(1500);
                LCD_Clear();
                PIR1bits.RCIF = 0x0;
                return;
            }
            
            uartBuffer[i++] = c;
            PIR1bits.RCIF = 0x0;
        }
        else 
        {
            isSetClock = 0;
            LCD_Clear();
            LCD_Set_Cursor(1, 1);
            LCD_Write_String("Received String:");
            LCD_Set_Cursor(2, 1);
            LCD_Write_String(uartBuffer);            
            clearBuffer(uartBuffer, BUFFER_SIZE);
            i = 0;
            delay_ms(1500);
            LCD_Clear();
            PIR1bits.RCIF = 0x0;
        }
    }
}

void    showClock(char RTC_data[])
{
        LCD_Clear();
        LCD_Set_Cursor(1,1);
        LCD_Write_Char((RTC_data[2]/10) + 48);
        LCD_Write_Char((RTC_data[2]%10) + 48);
        LCD_Write_Char(':');
        LCD_Write_Char((RTC_data[1]/10)+48);
        LCD_Write_Char((RTC_data[1]%10)+48);
        LCD_Write_Char(':');
        LCD_Write_Char(RTC_data[0]/10+48);
        LCD_Write_Char(RTC_data[0]%10+48);
        LCD_Set_Cursor(2,1);
        
        if (RTC_data[3] == 0x01)
            LCD_Write_String("CN ");
        else 
        {
            LCD_Write_Char('T');
            LCD_Write_Char(RTC_data[3] + 48);
            LCD_Write_Char(':');
        }

        LCD_Write_Char((RTC_data[4] / 10) + 48);
        LCD_Write_Char((RTC_data[4] % 10) + 48);
        LCD_Write_Char('/');
        LCD_Write_Char((RTC_data[5] / 10) + 48);
        LCD_Write_Char((RTC_data[5] % 10) + 48);
        LCD_Write_Char('/');
        LCD_Write_Char(20 / 10 + 48);
        LCD_Write_Char(48);
        LCD_Write_Char(RTC_data[6] / 10 + 48);
        LCD_Write_Char(RTC_data[6] % 10 + 48);
}

void    showLoadScreen(void) 
{
    char s[10];
    int j;
    
    LCD_Clear();
    LCD_Set_Cursor(1,1);
    LCD_Write_String(" WELCOME TO RTC!");
    LCD_Set_Cursor(2,1);
    LCD_Write_String("  HC05 - DS1307");
    delay_ms(2000);
    LCD_Clear();
        
    LCD_Set_Cursor(2, 1);
    for (j = 0; j < 15; j++) {
        LCD_Write_Char(219);
    }
    int k = 1;
    for (j = 0; j <= 100; ++j) 
    {
        LCD_Set_Cursor(1, 1);
        LCD_Write_String("LOADING...");
        sprintf(s, "%u", (j / 10));
        LCD_Set_Cursor(1, 11);
        LCD_Write_String(s);
        sprintf(s, "%u", (j % 10));
        LCD_Set_Cursor(1, 12);
        LCD_Write_String(s);
        LCD_Write_Char(37);

        if (!(j % 7)) 
        {
            LCD_Set_Cursor(2, k++);
            LCD_Write_Char(255);
            delay_ms(40);
        } else
        {
            delay_ms(50);
        }
    }

    LCD_Clear();
    delay_ms(1000);
    LCD_Set_Cursor(1, 1);
    LCD_Write_String("  COMPLETED !!!");
    LCD_Set_Cursor(2, 1);
    LCD_Write_String("  SYSTEM READY");
    delay_ms(1000);
}



int main(void)
{
    TRISD = 0x00;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    LCD_Init();
    I2CInit();
    UART_Init(38400);
    RCIF = 0x0;
    RCIE = 0x1;
    PEIE = 0x1;
    INTCONbits.GIE = 0x1;
    LCD_RW = 0;
    
    unsigned char   RTC_data[8];
    
    showLoadScreen();
    
    while (1) 
    {
        if(isSetClock == 1)
        {
            delay_ms(900);
            ds1307_getData(RTC_data);
            showClock(RTC_data);
        }
        else
        {
            LCD_Clear();
            delay_ms(500);
            LCD_Write_String("Waiting data...");
            delay_ms(500);
        }
    }
    
    return  0;
}