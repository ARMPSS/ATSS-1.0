/***************************************************************************************************************************** 
Source code file:      Prg08.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      SPI
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/
/*
   Read the 25LC256 EEPROM status register using SPI1  
*/

#include <xc.h>
#include <libpic30.h>

#define SEE_WRSR    1       				//  Write status register, 25LC256 Serial EEPROM commands   
#define SEE_WRITE   2       				//  Write command
#define SEE_READ    3       				//  Read command
#define SEE_WDI     4       				//  Write disable
#define SEE_STAT    5       				//  Read status register
#define SEE_WEN     6       				//  Write enable

uint8_t i;

void initializeSpi1(void);
int WriteSpi1( int data);				 

main()
{    
   initializeSpi1(); 
   while(1)
   {   
      LATCbits.LATC1  = 0;				//  Enable the EEROM, Send a Write Enable command
      WriteSpi1(SEE_WEN);    				//  Write enable command
      LATCbits.LATC1  = 1;				//  Disable the EEROM
        
      LATCbits.LATC1  = 0;				//  Enable the EEROM, Check the Serial EEPROM status
      WriteSpi1(SEE_STAT);   				//  Send a READ STATUS command
      i    = WriteSpi1(0);      			//  Send/receive the data by sending a dummy command
      LATCbits.LATC1  = 1;				//  Disable the EEROM
    }    
} 
void initializeSpi1(void)
{
   SPI1CON1 = 0x0120;	  				//  Select mode MODE16(0)-8 bit data, CKE(1)-serial outut changes when clock changes from active to idle, CKP(0)-0 is low 1 is high, MEN(1)-Master enable, SPRE(00)-8:1 prescale, PPRE(00)-64:1
   SPI1STAT = 0x8000;	  				//  Enable the peripheral SPIEN(1)

   TRISC = 0x0000;					//  RC1 is connected to CS (active low) of EEPROM and made an output
   LATCbits.LATC1  = 1;					//  Disable the EEROM initially
}
int WriteSpi1( int data)				//  Send one byte of data and receive one back at the same time
{
   SPI1BUF = data;                 			//  Write to buffer for TX
   while( !SPI1STATbits.SPIRBF);   			//  Wait transfer completion
   return SPI1BUF;                 			//  Read the received value
} 
    