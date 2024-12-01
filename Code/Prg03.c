/***************************************************************************************************************************** 
Source code file:      Prg03.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      ADC RF8 LED1 RF7 LED2 RF6 LED3 RA14 LED4
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/

#include <xc.h>
#include <libpic30.h>

uint16_t pos, pot;
void initializeAdc ();
uint16_t readAdc (void);

int main()
{
   initializeAdc();					//  Initialize ADC and analog inputs							
   while (1) 
   {
      pot = readAdc();					//  Select the POT input and convert
      pos = pot >> 8;					//  Reduce 12 bit value to a 4 bit value - dividing by  256 - right shift 8 times
      LATFbits.LATF6  = ~(pos & 0x0001);		//  Turn ON/OFF only one of the corresponding LEDs, LED3 RF6
      LATFbits.LATF7  = ~(pos & 0x0010);		//  LED2 RF7	
      LATFbits.LATF8  = ~(pos & 0x0100);		//  LED1 RF8
      LATAbits.LATA14 = ~(pos & 0x1000); 		//  LED4 RA14
   }  
}
   
void initializeAdc ()
{
   AD1CON1   = 0x8474;                                  //  ADON(1)-Turn ON ADC, MODE12(1)-12 bit operation, FORM(0,0)-Abs decimal unsigned right justified, SSRC(0111)-internal counter ands sampling and starts auto converting, ASAM(0)-manual sampling, SAMP(1)-start sampling after last conversion
   AD1CON2   = 0x0078;                                  //  CSCNA(0)-No scan, ALTS(0)-use only ch A for sampling, SMPI(11110)-interrupt after every 31st sample conversion
   AD1CON3   = 0x1E01;           			//  SAMC(1111)-Auto Sample time = 31*Tad, ADCS(00000001)-conversion clock-Tad = 2*Tcy = 125 ns (Fcy = Fosc/2 = 16 MHz)                                       		
   AD1CON4   = 0x0000;                                                            
   AD1CON5   = 0x0006;		                        //  ASEN(0)-No autoscan, ASINT(00)-no interrupt, WM(01)-convert and save, CM(00)-less than mode (valid match when value is less than that in buffer register)
   AD1CHS    = 0x0005;                                  //  CH0NA(000)-Vref-/AVss, CH0SA(00101)-channel 5 AN5
   ANCFG     = 0x0000;                                                            
   AD1CSSH   = 0x0000;
   AD1CSSL   = 0x0000;					   
   //AD1CHITH  = 0x0000;
   AD1CHITL  = 0x0000; 								
   AD1CTMENH = 0x0000;
   AD1CTMENL = 0x0000;

   TRISA = 0x0000;					//  RA14 is set in output mode
   TRISF = 0x0000;					//  RF6, RF7, RF8 are set in output mode

   LATF = 0x01C0;					//  Set all LEDs LOW initially
   LATA = 0x4000;					//  All the LEDS are in common cathode configuration   
}									
uint16_t readAdc ()
{					
   while (!AD1CON1bits.DONE);				//  Wait to complete conversion
   return ADC1BUF0;     				//  Return the conversion result
}