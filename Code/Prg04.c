/***************************************************************************************************************************** 
Source code file:      Prg04.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      PWM RF8 LED1 RF7 LED2 RF6 LED3 RA14 LED4
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/
/*
  PWM period = (PR3 + 1) * Tcy * timer prescale = (1 / 40 K) = (PR3 + 1) * (1/16 M) * 1; PR3 = 399
  Duty cycle = 50% (200)
*/
#include <xc.h>
#include <libpic30.h>

uint16_t count = 0;
void initializePwm (void);

int main()
{
   initializePwm();
   while (1);
}

void initializePwm (void)
{
   TRISF = 0x0000;					//  RF6 is set in output mode
   LATF = 0x0040;					//  Set RF6 LED3 LOW initially

   ANSB    = 0x0000;					//  Pin 20 RB0(RP0) set in digital mode 
   TRISB   = 0x0000;					//  Pin 20 RB0(RP0) set in output mode
   RPOR0   = 0x0012;					//  Peripheral Pin Select-RP0(pin 20) defined as PWM output OC1 Output compare(Function 18)
   T3CON   = 0x8000;         				//  Enable TMR3-TON(1), TCKPS(00)-1:1, T32(0)-16 bit timer, TCS(0)-internal clock-instr cycle = Fosc/2 = 16 MHz
   PR3     = 400 - 1;            			//  Set period for given bitrate
   IFS0bits.T3IF   = 0;              			//  Clear interrupt flag
   IEC0bits.T3IE   = 1;              			//  Enable TMR3 interrupt
   IPC2bits.T3IP   = 4;					//  TRIP<2:0> (100) default value
   OC1R    = OC1RS = 200;     				//  Inititalize at 50% duty cycle   
   OC1CON1 = 0x0406;  					//  OCTSEL(001)-T3, OCM(110)-edge aligned PWM mode on OC1  
   OC1CON2 = 0x000D;					//  SYNCSEL(01101)-T3 synchronous trigger mode, OCTRIG(0)- synchronous mode
}									
void __attribute__((interrupt, shadow, no_auto_psv)) _T3Interrupt(void)	//  Clear T3 interrupt flag
{    
    IFS0bits.T3IF = 0;
    count++;
    if (count > 40000) 					//  PWM period = (1/40K) = 25 us
    {
       count = 40000;
       if (count % 2)
          LATFbits.LATF6 = 0;				//  LED3 RF6 is turned ON once in two seconds
       else
          LATFbits.LATF6 = 1;				//  LED3 RF6 is turned OFF
    }
} 