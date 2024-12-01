/***************************************************************************************************************************** 
Source code file:      Prg01.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      Blinking LEDs RF8 LED1 RF7 LED2 RF6 LED3 RA14 LED4
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/

#include <xc.h>
#include <libpic30.h>
							//  Instruction cycle Fcy = Fosc/2 
uint16_t DELAY = 16000;                            	//  Tdelay = 256 ms = (2/Fosc) * PreScale * DELAY; DELAY = 16000 for Fosc = 32 MHz, prescale = 256 
void initializeLeds (void);
void blinkLeds (void);

int main()
{
   initializeLeds();
   while (1) blinkLeds();
}

void initializeLeds (void)
{
   TRISA = 0x0000;					//  RA14 is set in output mode
   TRISF = 0x0000;					//  RF6, RF7, RF8 are set in output mode

   LATF = 0x01C0;					//  Set all LEDs LOW initially
   LATA = 0x4000;					//  All the LEDS are in common cathode configuration

   T1CON = 0x8030;					//  TON(1), TCKPS(11): Prescale 1:256, TSIDL(0), TGATE(0), TSYNC(0), TCS(0)
   TMR1  = 0x0000;
}
void blinkLeds (void)
{
   LATF = ~0x01C0;					//  All the four LEDs are turned ON
   LATA = ~0x4000;
   delay();                                   		//  Quarter of a second delay
   LATF = 0x01C0;					//  All the four LEDs are turned OFF
   LATA = 0x4000;
   delay();
}									
void delay(void)
{
    while (TMR1 < DELAY);
    TMR1 = 0;
}