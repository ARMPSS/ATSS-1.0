/***************************************************************************************************************************** 
Source code file:      Prg02.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      Timer1 Interrupts RF8 LED1 RF7 LED2 RF6 LED3 RA14 LED4
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/

#include <xc.h>
#include <libpic30.h>

uint16_t dSec, sec, min, hrs, day;
void initializeDisplayTime (void);

int main()
{
   initializeDisplayTime();
   while (1);
}

void initializeDisplayTime (void)
{
   dSec  = 0;
   sec   = 0;
   min   = 0;
   hrs   = 0;
   day   = 0;

   TRISA = 0x0000;					//  RA14 is set in output mode
   TRISF = 0x0000;					//  RF6, RF7, RF8 are set in output mode

   LATF = 0x01C0;					//  Set all LEDs LOW initially
   LATA = 0x4000;					//  All the LEDS are in common cathode configuration

   T1CON = 0x8020;					//  TON(1), TCKPS(10): Prescale 1:64, TSIDL(0), TGATE(0), TSYNC(0), TIECS(00)
   TMR1  = 0x0000;					//  Tdelay = 0.1 s = (PR1 + 1) * Prescale * (2/Fosc) = (PR1 + 1) * 64 * (2/32 M)
   PR1   = 2500 - 1;

   IFS0bits.T1IF = 0;					//  Clear Timer1 interrupt flag
   IEC0bits.T1IE = 1;					//  Enable Timer1 interrupt
   IPC0bits.T1IP = 4;					//  Set Timer1 interrupt priority to the default value of 4
   
}									
void  __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)	//  Timer1 Interrupt service sub routine
{
   IFS0bits.T1IF    = 0;				//  Clear Timer1 Interrupt Flag T1IF(0) 
   dSec++;
   if (dSec > 9)
   {
      dSec = 0;
      sec++;
      if (sec % 2)
         LATFbits.LATF6 = 0;				//  LED3 RF6 is turned ON once in two seconds
      else
         LATFbits.LATF6 = 1;				//  LED3 RF6 is turned OFF 
      if (sec > 59) 
      {
         sec = 0;
         min++;
         if (min % 2)
            LATFbits.LATF7 = 0;				//  LED2 RF7 is turned ON once in two minutes
         else
            LATFbits.LATF7 = 1;				//  LED2 RF7 is turned OFF 
         if (min > 59) 
         {
            min = 0;
            hrs++;
      	    if (hrs % 2)
               LATFbits.LATF8 = 0;			//  LED1 RF8 is turned ON once in two hours
            else
               LATFbits.LATF8 = 1;			//  LED1 RF8 is turned OFF
            if (hrs > 23) 
            {
               hrs = 0;							 
	       day++;
               if (hrs % 2)
                  LATAbits.LATA14 = 0;			//  LED4 RA14 is turned ON once in two days
               else
                  LATAbits.LATA14 = 1;			//  LED4 RA14 is turned OFF
               if (day > 30) day = 0;
            }  
         }         
      }
   }  
}