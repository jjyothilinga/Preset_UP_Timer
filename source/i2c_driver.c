
/*
*------------------------------------------------------------------------------
* i2c_driver.c
*
* i2c driver module.
*
* (C)2008 Sam's Logic.
*
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: i2c_driver.c
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 11/12/2010
*------------------------------------------------------------------------------
*
* Revision 1.3 11/12/2010  Sam
* Demo Release
* Revision 1.2 01/12/2010 Sam
* Added i2c hardware module functionality with config option.
* Revision 1.0 07/07/2010 Sam
* First Release
* Revision 1.0 03/10/2008 Sam
* Initial revision
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

#include <delays.h>
#include "typedefs.h"
#include "config.h"
#include "i2c_driver.h"
#include "board.h"


/*
*------------------------------------------------------------------------------
* Private Defines
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Macros
*------------------------------------------------------------------------------
*/
#define 	SLAVE		(0xD0)
#define 	WRITE   	(0x00)
#define 	READ    	(0x01)
#define 	ERR_ACK 	(0x01)

/*
*------------------------------------------------------------------------------
* Private Data Types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Variables
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* Public Constants
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Constants (static)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* void InitializeI2C(void)
*
* Summary	: Initialize I2C hardware module in Master mode
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void InitializeI2C(void)
{
	// Disable SMBus inputs
  	SSPSTAT = 0x00;				
	//1 = Slew rate control disabled for Standard Speed mode (100 kHz and 1 MHz)
	SSPSTATbits.SMP = 1;		
	// See Rev. A4 Silicon Errata
	//SSPADD = 0x63;			
	SSPADD = 0x59;

	/*In I2C Master mode, the upper two bits
	of SSPCON1 are indicator bits and should be cleared
	initially. Also, while in Master mode, the SCL Release
	Control bit (CKP) is not used. The SSP Enable bit
	(SSPEN) must be set in order to enable the serial port*/
  	SSPCON1 = 0b00101000;             	
	// Clear sequence bits
  	SSPCON2 = 0x00;      

    // Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0;  
	//Clear Bit Collision flag
    PIR2bits.BCLIF = 0; 
           	
	// Configure SCL as an input
  	TRISCbits.TRISC3 = 1;
   	//DDRCbits.RC3 =1; 
   	// Configure SDA as an input	
	TRISCbits.TRISC4 = 1;				
	//DDRCbits.RC4 =1;				
}

/*
*------------------------------------------------------------------------------
* void StartI2C(void)
*
* Summary	: Send I2c start condition
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void StartI2C(void)
{

#ifdef USE_MSSP_I2C
	UINT16 timeOut = 1000;
  	// Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0; 
	// initiate bus start condition;
	SSPCON2bits.SEN = 1; 
	do
	{
		timeOut--;
	}while((0 == PIR1bits.SSPIF) && (timeOut !=0));
           
#else
	I2C_SCL_DIR = PORT_OUT;
	I2C_SDA_DIR = PORT_OUT;
    I2C_SDA = 1;
	I2C_SCL = 1;
	Delay10TCYx(5);
	I2C_SDA = 0;
	Delay10TCYx(5);
	I2C_SCL = 0;
	Delay10TCYx(5);
#endif
}

/*
*------------------------------------------------------------------------------
* void StopI2C(void)
*
* Summary	: Send I2c stop condition
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void StopI2C(void)
{
#ifdef USE_MSSP_I2C

	UINT16 timeOut = 1000;
  	// Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0; 

	// initiate bus stop condition
	SSPCON2bits.PEN = 1;  
	do
	{
		timeOut--;
	}while((0 == PIR1bits.SSPIF) && (timeOut !=0));
          
#else
	I2C_SCL_DIR = PORT_OUT;
	I2C_SDA_DIR = PORT_OUT;
	I2C_SDA = 0;
	Delay10TCYx(5);
	I2C_SCL = 1;
	Delay10TCYx(5);
	I2C_SDA = 1;
#endif
}

/*
*------------------------------------------------------------------------------
* void ResetI2C(void)
*
* Summary	: Send bus reset
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void ResetI2C(void)
{
#ifdef USE_MSSP_I2C
	UINT16 timeOut = 1000;
  	// Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0; 
	// initiate bus restart condition
	SSPCON2bits.RSEN = 1;     
	do
	{
		timeOut--;
	}while((0 == PIR1bits.SSPIF) && (timeOut !=0));      
#else
	UINT8 clockOut;
	I2C_SCL_DIR = PORT_OUT;
	I2C_SDA_DIR = PORT_IN;
	for(clockOut = 0;clockOut <= 10;clockOut++)
	{
		Delay10TCYx(5);
		I2C_SCL = 1;
		Delay10TCYx(5);
		I2C_SCL = 0;
	}
#endif
}
/*
*------------------------------------------------------------------------------
* INT8 WriteI2C(UINT8 Data)
*
* Summary	: Shift out a byte of data to I2C device
*
* Input		: UINT8 data - data to be shift out
*
* Output	: INT8 - Status byte for WCOL detection
*------------------------------------------------------------------------------
*/
INT8 WriteI2C(UINT8 Data)
{

#ifdef USE_MSSP_I2C
	UINT16 timeOut = 1000;
  	// Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0; 

	// write single byte to SSPBUF
  	SSPBUF = Data;           

	do
	{
		timeOut--;
	}while((0 == PIR1bits.SSPIF) && (timeOut !=0));

/*

	// test if write collision occurred
  	if(SSPCON1bits.WCOL)      
	{
		// if WCOL bit is set return negative #
   		return (-1);              
	}
  	else
  	{
		// wait until write cycle is complete         
    	while(SSPSTATbits.BF);   
		// if WCOL bit is not set return non-negative #
    	return ( 0 );              
  	}
*/
    return (0);              
#else
	UINT8 shiftIndex;
	I2C_SCL_DIR = PORT_OUT;
	I2C_SDA_DIR = PORT_OUT;
	for (shiftIndex=0;shiftIndex<8;shiftIndex++)
	{
        I2C_SDA = (Data & 0x80) ? 1:0;
		Delay10TCYx(3);
		I2C_SCL=1;
		Delay10TCYx(6);
		I2C_SCL=0;
		Delay10TCYx(2);
		Data<<=1;
	}
	// for testing
	I2C_SDA_DIR = PORT_IN;
	Delay10TCYx(2);
	// Rec ACK
  	I2C_SCL = 1;
	Delay10TCYx(6);
	I2C_SCL = 0;
	Delay10TCYx(6);
#endif
}

/*
*------------------------------------------------------------------------------
* UINT8 ReadI2C(BOOL ACK_Bit)
*
* Summary	: Shift in a byte of data from I2C device
*
* Input		: BOOL ackBit - acknowledge bit "ACK - 1", "NO_ACK - 0"
*
* Output	: UINT8 - read data
*------------------------------------------------------------------------------
*/
UINT8 ReadI2C(BOOL ackBit)
{
#ifdef USE_MSSP_I2C

	UINT16 timeOut = 1000;

	UINT8 Data;
  	// Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0;

	// enable master for 1 byte reception
  	SSPCON2bits.RCEN = 1;   

	do
	{
		timeOut--;
	}while((0 == PIR1bits.SSPIF) && (timeOut !=0));

	// read byte 
	Data = SSPBUF;
	
	// Clear SSP interrupt flag
 	PIR1bits.SSPIF = 0;

	if (ackBit == 1)
	{
		// set acknowledge bit state for ACK
  		SSPCON2bits.ACKDT = 0;           
		// initiate bus acknowledge sequence
  		SSPCON2bits.ACKEN = 1;           
	}	
	else
	{
		// set acknowledge bit for not ACK
  		SSPCON2bits.ACKDT = 1;          
		// initiate bus acknowledge sequence
  		SSPCON2bits.ACKEN = 1;          
	}
	timeOut = 1000;
	do
	{
		timeOut--;
	}while((0 == PIR1bits.SSPIF) && (timeOut !=0));

  	return Data; 


/* 
    
	// wait until byte received     
  	while(!SSPSTATbits.BF);      
	{
		// read byte 
		Data = SSPBUF;              
	}	

	if (ackBit == 1)
	{
		// set acknowledge bit state for ACK
  		SSPCON2bits.ACKDT = 0;           
		// initiate bus acknowledge sequence
  		SSPCON2bits.ACKEN = 1;           
	}	
	else
	{
		// set acknowledge bit for not ACK
  		SSPCON2bits.ACKDT = 1;          
		// initiate bus acknowledge sequence
  		SSPCON2bits.ACKEN = 1;          
	}

  	return Data; 
*/
#else    
	UINT8 Data=0,shiftIndex;

	I2C_SCL_DIR = PORT_OUT;
	I2C_SDA_DIR = PORT_OUT;

    //I2C_SDA = 1;
	I2C_SDA_DIR = PORT_IN;
	for (shiftIndex=0;shiftIndex<8;shiftIndex++)
	{
		I2C_SCL   = 1;
		Delay10TCYx(6);
		Data<<= 1;
		Data  = (Data | I2C_SDA);
		I2C_SCL   = 0;
		Delay10TCYx(6);

	}
	I2C_SDA_DIR = PORT_OUT;
 	if (ackBit == 1)
		I2C_SDA = 0; // Send ACK
	else
		I2C_SDA = 1; // Send NO ACK
	Delay10TCYx(2);
	I2C_SCL = 1;
	Delay10TCYx(6);
	I2C_SCL = 0;
	Delay10TCYx(6);
	return Data;
#endif
}

/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* UINT8 ReadByteI2C(UINT8 devAddr, UINT8 Addr)
*
* Summary	: Read 1 byte of data from addressed I2C device
*
* Input		: UINT8 devAddr - device(slave) address
*   		  UINT8 addr - address from where to read
*
* Output	: UINT8 - read data
*
*------------------------------------------------------------------------------
*/
UINT8 ReadByteI2C(UINT8 devAddr, UINT8 addr)
{
   	UINT8 data;
	StartI2C();
	WriteI2C(devAddr);
	WriteI2C(addr);
	StartI2C();
	WriteI2C(devAddr + 1);
	data = ReadI2C(NO_ACK);
	StopI2C();
   	return(data);
}

/*
*------------------------------------------------------------------------------
* void WriteByteI2C(UINT8 devAddr, UINT8 addr,UINT8 data)
*
* Summary	: Write 1 byte of data to addressed I2C device
*
* Input		: UINT8 devAddr - device(slave) address
*   		  UINT8 addr - address where to write
*      		  UINT8 - data to write
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void WriteByteI2C(UINT8 devAddr, UINT8 addr,UINT8 data)
{
	StartI2C();
	WriteI2C(devAddr);
	WriteI2C(addr);
	WriteI2C(data);
	StopI2C();
}

/*
*  End of i2c_driver.c
*/