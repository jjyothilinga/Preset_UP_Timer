
/*
*------------------------------------------------------------------------------
* math_fun.c
*
* Math related function
*
* (C)2009 Sam's Logic.
*
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: math_fun.c
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 11/12/2010
*------------------------------------------------------------------------------
*
* Revision 1.2 11/12/2010  Sam
* Demo Release
* Revision 1.1 07/07/2010 Sam
* First Release
* Revision 1.0 20/04/2010 Sam
* Initial revision
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/


#include "board.h"
#include "config.h"
#include "typedefs.h"
#include "math_fun.h"
//#include "uart_driver.h"
#include "rtc_driver.h"
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
* Public Functions
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* UINT8 ConvertBCD2HEX(UINT16 bcd)
*
* Summary	: Convert BCD 1 byte to HEX 1 byte
*
* Input		: UINT16 bcd - bcd value
*
* Output	: UINT8 - converted HEX value
*
*------------------------------------------------------------------------------
*/
UINT8 ConvertBCD2HEX(UINT16 bcd)
{
	UINT8 temp;
	temp=((bcd>>8)*100)|((bcd>>4)*10)|(bcd&0x0f);
	return temp;
}

/*
*------------------------------------------------------------------------------
* UINT16 BCD2HEX(UINT8 val)
*
* Summary	: Convert BCD 1 byte to HEX 1 byte
*
* Input		: UINT8 bcd - bcd value
*
* Output	: UINT8 - converted HEX value
*
*------------------------------------------------------------------------------
*/
UINT16 BCD2HEX(UINT8 val)
{
   val -= (val/0x10)*0x06;
   return val;
}

/*
*------------------------------------------------------------------------------
* UINT8 HEX2BCD(UINT8 hexIn)
*
* Summary	: Convert 1 b yte HEX to 1 byte BCD
*
* Input		: UINT8 bcd - bcd value
*
* Output	: UINT8 - converted HEX value
*
*------------------------------------------------------------------------------
*/
UINT8 HEX2BCD(UINT8 hexIn)
{
	UINT8 temp1,temp2;
	temp1 = (hexIn / 10);
	temp2 = (hexIn % 10);
	return (((temp1 & 0x0F) << 4 ) | (temp2 & 0x0F));
}

/*
*------------------------------------------------------------------------------
*  void ConvertAndDisplay16BitValue(UINT16 inValue, UINT8 *pNewVal)
*
*  Summary	: Convert 16 bit hex value into 5 digit decimal value
*
*  Input	: inValue - 16 bit hex value
*			: *pNewVal - Pointer to return the converted decimal digits
*
*  Output	: None
*
*------------------------------------------------------------------------------
*/
void ConvertAndDisplay16BitValue(UINT16 inValue, UINT8 *pNewVal)
{
	UINT8 TempArray[5],loopCount;
	UINT32 Qut,Rem;

	Rem = inValue % 10;
	TempArray[4] = Rem;			// char 4
	Qut = inValue / 10;

	Rem = Qut % 10;
	TempArray[3] = Rem;			// char 3
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[2] = Rem;			// char 2
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[1] = Rem;			// char 1
	Qut = Qut / 10;
	TempArray[0] = Qut;			// char 0

	for(loopCount = 0; loopCount < 5; loopCount++)
	{
		*(pNewVal + loopCount) = TempArray[loopCount];
	}
}
/*
*------------------------------------------------------------------------------
*  void ConvertAndDisplay8BitValue(UINT8 inValue, UINT8 *pNewVal)
*
*  Summary	: Convert 8 bit hex value into 3 digit decimal value
*
*  Input	: inValue - 8 bit hex value
*			: *pNewVal - Pointer to return the converted decimal digits
*
*  Output	: None
*
*------------------------------------------------------------------------------
*/
void ConvertAndDisplay8BitValue(UINT8 inValue, UINT8 *pNewVal)
{
	UINT8 TempArray[3],loopCount;
	UINT16 Qut,Rem;

	Rem = inValue % 10;
	TempArray[2] = Rem;			// char 2
	Qut = inValue / 10;

	Rem = Qut % 10;
	TempArray[1] = Rem;			// char 1
	Qut = Qut / 10;

	TempArray[0] = Qut;			// char 0

	for(loopCount = 0; loopCount < 3; loopCount++)
	{
		*(pNewVal + loopCount) = TempArray[loopCount];
	}
}

/*
*------------------------------------------------------------------------------
*  void ConvertAndDisplay32BitValue(UINT16 inValue, UINT8 *pNewVal)
*
*  Summary	: Convert 32 bit hex value into 5 digit decimal value
*
*  Input	: inValue - 32 bit hex value
*			: *pNewVal - Pointer to return the converted decimal digits
*
*  Output	: None
*
*------------------------------------------------------------------------------
*/
void ConvertAndDisplay32BitValue(UINT32 inValue, UINT8 *pNewVal)
{
	UINT8 TempArray[10],loopCount;
	UINT32 Qut,Rem;

	Rem = inValue % 10;
	TempArray[9] = Rem;			// char 10
	Qut = inValue / 10;

	Rem = Qut % 10;
	TempArray[8] = Rem;			// char 9
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[7] = Rem;			// char 8
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[6] = Rem;			// char 7
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[5] = Rem;			// char 7
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[4] = Rem;			// char 5
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[3] = Rem;			// char 4
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[2] = Rem;			// char 3
	Qut = Qut / 10;

	Rem = Qut % 10;
	TempArray[1] = Rem;			// char 2
	Qut = Qut / 10;

	TempArray[0] = Qut;			// char 1

	for(loopCount = 0; loopCount < 10; loopCount++)
	{
		*(pNewVal + loopCount) = TempArray[loopCount] | 0x30;
	//	PutcUARTRam(TempArray[loopCount] | 0x30);
	}
}
/*
*------------------------------------------------------------------------------
* UINT8 BCDtoBin(UINT8 val)

* Summary	:
*
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
UINT8 BCDtoBin(UINT8 val)		 // To read from RTC.
{
   val -= (val/0x10)*0x06;
   return val;
}
/*
*------------------------------------------------------------------------------
* UINT16 BinToBCD(UINT8 temp)

* Summary	:
*
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
UINT16 BinToBCD(UINT8 temp) 	     // To write to RTC
{
   temp += (temp/0x0A)*0x06;
   return temp;
}
/*
*------------------------------------------------------------------------------
* UINT16 ConvertUnPackedAsciiToPacked16BitHexValue(UINT8 *inArray, UINT8 numOfDigits)
*
* Summary	: Convert 5 digit Ascii array in to packed 16 bit hex value
*
* Input		: UINT8* inArray - input array with Ascii digits
*             UINT8 numOfDigits - number of digits in the array
*
* Output	: UINT16 - 16 bit packed hex value
*------------------------------------------------------------------------------
*/
UINT16 ConvertUnPackedAsciiToPacked16BitHexValue(UINT8 *inArray, UINT8 numOfDigits)
{
	if(numOfDigits < 6)
	{
		UINT8 index,baseIndex;
		//unsigned long int tempValue;
		UINT16 tempValue = 0;
		UINT16 baseValue = 0;
		// Find the base value, multiply the digits with base value and add them together.
		for(index = 0; index < numOfDigits; index++)
		{
			baseIndex = (numOfDigits - (index + 1));
			baseValue = 1;
			// Find the base value
			for(;baseIndex > 0; baseIndex--)
			{
				baseValue = (UINT16)(baseValue * (UINT16)10);
			}
			// Multiply the digit with the base value and add to the current value
			tempValue = (UINT16)(tempValue + (UINT16)(baseValue * (UINT16)(*(inArray + index) & 0x0F)));
		}
		return tempValue;
	}
	else
	{
		return 0;
	}
 }

/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/

/*
*  End of math_fun.c
*/
