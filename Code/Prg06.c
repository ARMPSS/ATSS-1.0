/***************************************************************************************************************************** 
Source code file:      Prg06.c
Author, version, date: PSS ver 1.0 01.11.24
Program function:      Motor position
Simulation:            PIC24FJ128GA308 MCU, MPLAB X IDE ver 6.05, XC16 ver 2.10 
******************************************************************************************************************************/
/*
SW1 switch on safety box for running (LED ON) the clutch (LED1) and brake (LED3) motors or stopping the motors (LED OFF)
SW2 switch near driver for running the clutch (LED2) and brake (LED4) motors in a clockwise (LED ON) or anticlockwise direction (LED OFF)

LED1: ON - clutch motor is running,       OFF - clutch motor is not running
LED2: ON - clutch motor is fully engaged, OFF - clutch motor is fully disengaged
LED3: ON - brake  motor is running,       OFF - brake  motor is not running 
LED4: ON - brake  motor is fully engaged, OFF - brake  motor is fully disengaged
*/

#include <xc.h>
#include <libpic30.h>

#define LED1 LATFbits.LATF8
#define LED2 LATFbits.LATF7
#define LED3 LATFbits.LATF6
#define LED4 LATAbits.LATA14
                                                                                                                                                       
#define spareOnOffSwitch LATDbits.LATD15               										
#define actuatorReleaseSwitch LATDbits.LATD14
									
#define INH1 LATBbits.LATB4						//  Motor driver inhibit pins
#define INH2 LATBbits.LATB6
#define INH3 LATBbits.LATB9
#define INH4 LATBbits.LATB14
									
#define IN1  LATBbits.LATB3						//  Motor input pins
#define IN2  LATBbits.LATB7
#define IN3  LATBbits.LATB10
#define IN4  LATBbits.LATB15

#define IS1  LATBbits.LATB5						//  Motor current sensor pins
#define IS2  LATBbits.LATB2
#define IS3  LATBbits.LATB8
#define IS4  LATBbits.LATB11

#define ON         	       1
#define OFF                0
#define BEGINNING      	   0					//  Fully disengaged  
#define MIDDLE            -1 					//  Fifty percent engaged
#define END        	       1					//  Fully engaged	

#define hallSensor1 LATFbits.LATF4
#define hallSensor2 LATFbits.LATF5
#define hallSensor3 LATFbits.LATF3
#define hallSensor4 LATFbits.LATF2

uint8_t spareOnOffSwitchNow;  
uint8_t actuatorReleaseSwitchNow; 
uint8_t motorsState; 
uint8_t motorsStateNow;
uint8_t clutchMotorState; 
uint8_t clutchMotorStateNow;
uint8_t clutchMotorPositionNow;
uint8_t bit0;
uint8_t bit1;
uint8_t bits10;
uint8_t bits32;
uint8_t bits3210;
uint8_t brakeMotorState; 
uint8_t brakeMotorStateNow;
uint8_t brakeMotorPositionNow;
uint8_t motorPosition;
uint16_t motorDegrees;
uint8_t motorPulsesCount;
uint8_t motorPulsesPerRevolution = 280;
uint8_t INH1Now;
uint8_t INH2Now;
uint8_t INH3Now;
uint8_t INH4Now;
uint8_t IN1Now;
uint8_t IN2Now;
uint8_t IN3Now;
uint8_t IN4Now;
uint8_t extInt1;
uint8_t extInt2;
uint8_t extInt3;
uint8_t extInt4;

void initializeMotors(void);
uint8_t turnOnMotors(void);
uint8_t getClutchMotorState(void);   				
uint8_t getBrakeMotorState(void);					 
uint8_t getMotorPosition();
void engageClutchMotor(void);
void disEngageClutchMotor(void);
void engageBrakeMotor(void);
void disEngageBrakeMotor(void);

int main()
{
   initializeMotors();
   turnOnMotors();
   while (1)      
   {
      clutchMotorStateNow = getClutchMotorState(); 
      if (clutchMotorStateNow) 
		if (!IS1 & !IS2)							//  No current sensor error   
	       clutchMotorPositionNow = getMotorPosition(); 

      brakeMotorStateNow  = getBrakeMotorState(); 
      if (brakeMotorStateNow) 
		if (!IS3 & !IS4)  
           brakeMotorPositionNow   = getMotorPosition();

      if (clutchMotorStateNow)
         LATFbits.LATF6 = 0;						//  LED3 RF6 ON, clutch motor is running 
      else
         LATFbits.LATF6 = 1; 						//  LED3 RF6 OFF, clutch motor is not running 
  
      if (clutchMotorPositionNow)
         LATFbits.LATF7 = 0;						//  LED2 RF7 ON, clutch motor is fully engaged 
      else
         LATFbits.LATF7  = 1;						//  LED2 RF7 OFF, clutch motor is fully disengaged

      if (brakeMotorStateNow)
         LATFbits.LATF8 = 0;						//  LED1 RF8 ON, brake motor is running 
      else
         LATFbits.LATF8 = 1;						//  LED1 RF8 OFF, brake motor is not running  
 
      if (brakeMotorPositionNow)
         LATAbits.LATA14 = 0;						//  LED4 RA14 ON, brake motor is fully engaged 
      else
         LATAbits.LATA14 = 1;						//  LED4 RA14 OFF, brake motor is fully disengaged
   }
}
void initializeMotors(void)
{   
   ANSB  = 0x0000;							//  Digital mode
   TRISA = 0x0000;							//  Output mode(O):0, Input mode(I):1, RA14:LED4(O) 
   TRISB = 0x3927;							//  RB0:PGEC(I), RB1:PGED(I), RB2:IS2(I), RB3:IN1(O), RB4:INH1(O), RB5:IS1(I), RB6:INH2(O), RB7:IN2(O), RB8:IS3(I), RB9:INH3(O), RB10:IN3(O), RB11:IS4(I), RB12:CurrentSensorInput(I), RB13:RadarSensorInput(I), RB14:INH4(O), RB15:IN4(O)
   TRISF = 0x003C;							//  RF2:HallSensor4(I), RF3:HallSensor3(I), RF4:HallSensor1(I), RF5:HallSensor2(I)
   TRISD = 0xC110;							//  RD0:CAN CS(O), RD4:JetsonXavierTx(I), RD5:JetsonXavierRx(O), RD8:CAN SDI(I), RD9:CAN SDO(O), RD10:CAN SCL(I), RD11:CAN INT(O), RD14:spareOnOffSwitch(I), RD15:actuatorReleaseSwitch(I)

   LATF = 0x01C0;							//  Set all LEDs LOW initially
   LATA = 0x4000;							//  All the LEDS are in common cathode configuration

   PORTA = PORTB = PORTD = 0x0000;
   LATA  = LATB  = LATD  = 0x0000;

   RPINR0    = 0x2700; 							//  PPS Peripheral Pin Select INT1(39:0x27)-hallSensor1(39) 
   RPINR1    = 0x2928;							//  INT3(41:0x29)-hallSensor3(41) INT2(40:0x28)-hallSensor2(40)  
   RPINR2    = 0x002A;							//  INT4(42:0x2A)-hallSensor4(42) 
                                                                     	
   INTCON2   = 0x0000;							//  INT1-4 are +ve edge triggered
   IFS1	     = 0x0000;							//  Clear INT1,2 flags
   IFS3      = 0x0000;							//  Clear INT3,4 flags
   IEC1      = 0x2010;							//  Enable INT1,2
   IEC3      = 0x0060;							//  Enable INT3,4
   IPC5      = 0x0004;							//  Set INT1 priority as 4 (default)
   IPC7      = 0x0040;							//  Set INT2 priority as 4 (default) 
   IPC13     = 0x0440;							//  Set INT3,4 priority as 4 (default)

   LED1    = 1;									//  Set all LED OFF initially
   LED2    = 1;
   LED3    = 1;
   LED4    = 1;

   clutchMotorPositionNow = BEGINNING;
   brakeMotorPositionNow  = BEGINNING;
}
uint8_t turnOnMotors(void)
{
   spareOnOffSwitchNow      = spareOnOffSwitch;		
   actuatorReleaseSwitchNow = actuatorReleaseSwitch;	
     		
   if (spareOnOffSwitchNow)							//  RD15 Spare ON/OFF switch
   {
      if (actuatorReleaseSwitchNow)					//  RD14 actuatorReleaseSwitch	
      {
         while (clutchMotorPositionNow != END)
         {
            engageClutchMotor();					//  Motors run in clockwise direction
			if (!IS1 & !IS2)
               clutchMotorPositionNow   = getMotorPosition(); 
         }       
         while (brakeMotorPositionNow != END)
         {          			
            engageBrakeMotor();
            if (!IS3 & !IS4)		
               brakeMotorPositionNow   = getMotorPosition(); 
         }
      }  
      else
      {
         while (clutchMotorPositionNow != BEGINNING)
         {
            disEngageClutchMotor();					//  Motors run in anticlockwise direction
			if (!IS1 & !IS2)
               clutchMotorPositionNow   = getMotorPosition();
         }
         while (brakeMotorPositionNow != BEGINNING)
         {          			
            disEngageBrakeMotor(); 
			if (!IS3 & !IS4)
               brakeMotorPositionNow   = getMotorPosition(); 
         }                      
      }  									
   }
   else
   {
      INH1 = 0;								//  Clutch motor in SLEEP inactive mode
      INH2 = 0;  
      INH3 = 0;								//  Brake motor in SLEEP inactive mode
      INH4 = 0;
      clutchMotorStateNow = OFF;
      brakeMotorStateNow  = OFF; 
   }
}
uint8_t getClutchMotorState(void)
{									
   INH1Now = INH1;
   INH2Now = INH2;							//  RB4 is INH1, RB6 is INH2						
   IN1Now  = IN1;							//  RB3 is IN1, RB7 is IN2
   IN2Now  = IN2;
   if ((INH1Now) & (INH2Now)) 										//  Low INHIBIT puts BT8982A in SLEEP mode, clutch motor is in an OFF state  
   {   
      if (((IN1Now & (!IN2Now)) | ((!IN1Now) & IN2Now)))      		//  (IN1, IN2) = (0, 1) is CLOCKWISE, (IN1, IN2) = (1, 0) is ANTICLOCKWISE
         clutchMotorState = ON;					
      else
         clutchMotorState = OFF;
   }
   return clutchMotorState;  
}
uint8_t getBrakeMotorState(void)
{
   INH3Now = INH3;							//  RB10 is INH3, RB15 is INH4
   INH4Now = INH4;
   IN3Now  = IN3;							//  RB9 is IN3, RB14 is IN4
   IN4Now  = IN4;					
   if ((INH3Now) & (INH4Now))                                       	//  Low INHIBIT puts BT8982A in SLEEP mode, brake motor is in an OFF state
   {   
      if (((IN3 & (!IN4)) | ((!IN3) & IN4)))                            //  (IN3, IN4) = (0, 1) is CLOCKWISE, (IN3, IN4) = (1, 0) is ANTICLOCKWISE
         brakeMotorState = ON;						
      else
         brakeMotorState = OFF;
   }
   return brakeMotorState;  
}
uint8_t getMotorPosition()
{
   if ((extInt1 & extInt2) | (extInt3 & extInt4))
   {
      bit1 = hallSensor3; 
      bit0 = hallSensor4;
      bits10 = (bit1 << 1) | bit0;
      bits3210  = (bits32 << 2) | bits10;
      if (bits3210 == 0b1101 || bits3210 == 0b0100 || bits3210 == 0b0010 || bits3210 == 0b1011) motorPulsesCount--;
      if (bits3210 == 0b1110 || bits3210 == 0b0111 || bits3210 == 0b0001 || bits3210 == 0b1000) motorPulsesCount++;
      bits32 = bits10;

      motorDegrees = motorPulsesCount * 360 / motorPulsesPerRevolution;	//  degrees = edgeCount * 360 /(x * N), x = type of encoding (X1, X2, X4), x = 1 for X1
      switch (motorDegrees)						//  N is the pulse count for one revolution
      {
         case 0:
            motorPosition = BEGINNING;
            break;
         case 180:
            motorPosition = MIDDLE;
            break;
         case 360:
            motorPosition = END;
            break;
      }
   }
   return motorPosition;
}
void engageClutchMotor(void)
{
   INH1 = 1; 
   INH2 = 1;
   IN1  = 1; 
   IN2  = 0;								//  Clockwise motor  
}
void disEngageClutchMotor(void)
{
   INH1 = 1; 
   INH2 = 1;                           		//  Counterclockwise motor
   IN1  = 0;
   IN2  = 1;
}
void engageBrakeMotor(void)
{
   INH3 = 1; 
   INH4 = 1;
   IN3  = 1; 
   IN4  = 0;						 
}
void disEngageBrakeMotor(void)
{
   INH3 = 1; 
   INH4 = 1;
   IN3  = 0; 
   IN4  = 1;   
}
void  __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)	//  External Interrupt 1 service sub routine
{	
   IFS1bits.INT1IF = 0;							//  Clear External Interrupt Flag INT1IF(0) 
   extInt1         = 1;
}
void  __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void)	//  External Interrupt 2 service sub routine
{
   IFS1bits.INT2IF = 0;							//  Clear External Interrupt Flag INT2IF(0) 
   extInt2         = 1;
}
void  __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void)	//  External Interrupt 3 service sub routine
{
   IFS3bits.INT3IF = 0;							//  Clear External Interrupt Flag INT3IF(0) 
   extInt3         = 1;
}
void  __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void)	//  External Interrupt 4 service sub routine
{
   IFS3bits.INT4IF = 0;							//  Clear External Interrupt Flag INT4IF(0) 
   extInt4         = 1;
}								

