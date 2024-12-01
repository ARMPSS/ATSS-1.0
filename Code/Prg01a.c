/***************************************************************************************************************************** 
Source code file:      Prg01a.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      Turn ON LEDs using switches
                       RD15 spareOnOffSwitch RD14 actuatorReleaseSwitch RF8 LED1 RF7 LED2 RF6 LED3 RA14 LED4
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/

#include <xc.h>
#include <libpic30.h>

void initializeLeds (void);
void turnOnLeds (void);

int main()
{
   initializeLeds();
   while (1) turnOnLeds();
}

void initializeLeds (void)
{
   TRISF = 0x0000;					//  RF6 is set in output mode
   TRISD = 0xC000;					//  RD15, RD14 are set in input mode
   LATF  = 0x00C0;					//  Set LED2, LED3 LOW initially
}
void turnOnLeds (void)
{
   if (LATD & 0x8000) 
      LATF = ~0x0040;					//  Turn ON LED3 RF6
   else
      LATF = 0x0040;

   if (LATD & 0x4000) 
      LATF = ~0x0080;					//  Turn ON LED2 RF5
   else
      LATF = 0x0080;
}									