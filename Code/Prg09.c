/***************************************************************************************************************************** 
Source code file:      Prg09.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      I2C
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/
/*
   Read and Write 24LC16 Serial EEPROM (SEE) using I2C  
   Fscl = (Fcy / (I2C1BRG + 1 + (Fcy / 10000000))), Fcy = Fosc / 2 = 16 MHz, Fscl = 100 KHz, I2C1BRG = 157.4 = approx. 157 or 0x9D
*/

#include <xc.h>
#include <libpic30.h>
//#include <i2c.h>
//#include <timer.h>

#define BUS_FRQ     100000L 					//  100kHz
#define READ_CMD    1
#define WRITE_CMD   0

int i, r;

void initSEE (void);
int addressSEE (long add);
int iReadSEE (long address);					//  NOTE: Address must be an even value between 0x0000 and 0x7FE
void iWriteSEE (long address, int data);				 
 
main (void)
{  
   initSEE();   
     
   for( i = 0; i < 1024; i++)					//  Test filling all the 16-bit integers
   {
      iWriteSEE (i << 1, i);
      r = iReadSEE (i << 1);
      if (r != i) break;           
   }

   while( 1)
   {
      asm("nop");
      asm("nop");
      asm("nop");
   } 
}     
void initSEE( void)						//  Configure I2C for 7 bit address mode 100kHz
{  
// OpenI2C1 (I2C_ON | I2C_IDLE_CON | I2C_7BIT_ADD | I2C_STR_EN
//          | I2C_GCALL_DIS | I2C_SM_DIS | I2C_IPMI_DIS,
//          (FCY /(2*BUS_FRQ))-1);
   I2C1CON  = 0x8200;
   I2C1STAT = 0x0000;
   IdleI2C1();

// OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_256, -1);
   TMR1 = 0;
   while(TMR1 < 100);    
}   
int addressSEE( long add)					//  Send the address selection command, repeat if SEE busy  
{
   int cmd;
        
   cmd = 0xA0 | ((add >> 7) & 0xE);				//  Form SEE command + address MSB(3)
     
   while( 1)							//  WRITE() the Address MSB, try send command and repeat until ACK is received, while waiting for ACK
   { 
      StartI2C1(); 
      IdleI2C1();        
        
      MasterWriteI2C1(cmd + WRITE_CMD); 			//  Send command and address MSB(3)
      IdleI2C1();
        
      if (I2C1STATbits.ACKSTAT == 0) break;
                       
      StopI2C1();
      IdleI2C1();
   } 
   
   MasterWriteI2C1(add); 					//  Send address LSB
   IdleI2C1();

   return cmd;
}     
int iReadSEE(long add)						//  Random access read command sequence
{
   int cmd, r; 
       
   cmd = addressSEE(add);					//  Select address
    
   StopI2C1();
   IdleI2C1();
          
   StartI2C1(); 						//  Read command
   IdleI2C1();
   MasterWriteI2C1(cmd + READ_CMD);
   IdleI2C1();
    
   r = MasterReadI2C1();					//  Stream data in (will continue until NACK is sent)
    
   AckI2C1(); 
   IdleI2C1();      
   r |= (MasterReadI2C1() << 8);

   NotAckI2C1(); 						//  Terminate read sequence (send NACK then STOP)	
   IdleI2C1();
   StopI2C1(); 
   IdleI2C1();
    
   return r;
} 			
void iWriteSEE(long add, int v)					//  SEE write command sequence
{
   int cmd;     
    
   cmd = addressSEE(add);					//  Select address
             
   MasterWriteI2C1(v & 0xFF);  					//  Stream data out 
   IdleI2C1();  

   MasterWriteI2C1(v >> 8);
   IdleI2C1();  
    
   StopI2C1(); 							//  Terminate the command sequence
   IdleI2C1();    
} 
    

 