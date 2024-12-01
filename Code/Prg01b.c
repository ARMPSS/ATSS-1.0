/***************************************************************************************************************************** 
Source code file:      Prg01b.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      Turn ON buzzer using switches
                       RD15 spareOnOffSwitch RD14 actuatorReleaseSwitch RF8 LED1 RF7 LED2 RF6 LED3 RA14 LED4
                       RC14 buzzerRelay RG8 powerOnRelay RE8 vBatSensorInput
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/

#include <xc.h>
#include <libpic30.h>

void initializeBuzzer (void);
void turnOnBuzzer (void);

int main()
{
   initializeBuzzer();
   while (1) turnOnBuzzer();
}

void initializeBuzzer (void)
{
   TRISC = 0x0000;					//  RC14 is set in output mode
   TRISD = 0x8000;					//  RD15 is set as input
}
void turnOnBuzzer (void)
{
   if (LATD & 0x8000) 
      LATC = 0x4000;					//  Turn ON Buzzer
   else
      LATC = ~0x4000;

}									
