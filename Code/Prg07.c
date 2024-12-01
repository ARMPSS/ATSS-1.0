/***************************************************************************************************************************** 
Source code file:      Prg07.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      UART
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/
/*
  Baud rate = Fcy / (4 * (U1BRG + 1)), Instruction cycle frequency, Fcy = Fosc/2, Fosc = 32 M, BRGH = 1, HiSpeed.
  Desired baud rate = 115,200 = 32 M / (8 * U1BRG + 1)), U1BRG = 33.7(34 approx.)
  Calaculated baud rate = 32 M / (8* (34 + 1)) = 114,285
  Error % = (Calculated baud rate - Desired baud rate) * 100 / Calculated baud rate = (115,200 - 114,285) * 100 / 115,200 = 0.7 %

  Connect the serial cable to PC either directlt or through USB converter. Configure Hyperterminal (or Terraterm, Putty, Realterm) for the same communication parameters:
  115,200 baud rate, 8 bit data, no parity and select CTS/RTS hardware for flow of control on the available COM port. Click Connect to start emulation 
  
  A prompt is sent from the MCU to PC. The character typed in the PC is received by the MCU. The received character is sent back to the PC from the MCU as an echo 
*/

#include <xc.h>
#include <libpic30.h>

uint8_t c;						
void initializeU1(void);
uint8_t getU1(void);
uint8_t putU1(uint8_t);

int main()
{
   initializeU1();							//  Initialize the UART1 serial port
   putU1('>');								//  Prompt
   while (1)
   {
      c = getU1();							//  Wait for the character
      putU1(c);								//  Echo the character
   }           
}
void initializeU1(void)
{    
   ANSG    = 0x0000;							//  RG6, RG7 in digital mode
   TRISG   = 0x0000;							//  RG6, RG7 in output mode
   TRISC   = 0x000A;							//  RC3, RC1 in input mode
   RPINR18 = 0x2826;							//  Peripheral Pin Select Input U1CTS(RC3 pin 5 RPI40)40 = 0x28, U1RX(RC1 pin 4 RPI38) 38 = 0x26
   RPOR10  = 0x0400;							//  Peripheral Pin Select Output RP21 pin is U1RTS (Output function 4) - RG6 pin 6
   RPOR13  = 0x0003;							//  Peripheral Pin Select Output RP26 pin is U1TX (Output function 3) - RG7 pin 7
   U1BRG   = 0x0022;							//  0x22 = 34
   U1MODE  = 0x8008;							//  UARTEN(1), UEN(00)-U1Tx, U1Rx are enabled & used, U1CTS, U1RTS, BCLK1 are latch controlled, ABAUD(0)-disable measurement, BRGH(1)-HiSpeed, PDSEL(00)-8 bit data, no parity, STSEL(0)-1 stop bit
   U1STA   = 0x0400;							//  UTXEN(1)-Tx enabled, 
}
uint8_t getU1(void)
{
   LATGbits.LATG6 = 0;							//  Assert RTS, request to send
   while (!U1STAbits.URXDA);						//  Wait
   LATGbits.LATG6 = 1;
   return U1RXREG;							//  Read from Rx buffer   
}
uint8_t putU1(uint8_t c)
{									
   while (LATFbits.LATF1);						//  Wait for CTS, clear to send
   while (U1STAbits.UTXBF);						//  Wait while Tx buffer full
   U1TXREG = c;
   return c;    
}