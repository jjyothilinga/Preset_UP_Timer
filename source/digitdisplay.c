/*
*------------------------------------------------------------------------------
* digitdisplay.c
*
* Display module supporting 7 segment digit displays.
* Refreshing scheme is used to provide a stationary display.
* Data to be displayed is either a digit(0-9) or space(' ') in ascii
* Space represents clear i.e all segments of the 7 segment display
* turned off.
* Digit select lines are DIGIT_SELECT_A(LSB) , DIGIT_SELECT_B , 
* DIGIT_SELECT_C(MSB). These lines are assumed to be connected to
* a 3 to 8 decoder(74LS138) giving maximum of 8 digit selection.
* The data for each particular digit is put on DISPLAY_PORT, which
* is common to all the digits.
* 
*------------------------------------------------------------------------------
*/

typedef enum
{
	STATIC = 0,
	BLINK 
}DISPLAY_MODE;

#define MAX_DIGITS_ROW (6)

/*
*------------------------------------------------------------------------------
* INCLUDES
*------------------------------------------------------------------------------
*/

#include "digitdisplay.h"


/*------------------------------------------------------------------------------
* Structure declarations
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
* DigitDisplay - the Digit display structure. 
*------------------------------------------------------------------------------
*/

typedef struct _DigitDisplay
{
	DISPLAY_MODE mode;
	UINT8 buffer[2][MAX_DIGITS];	//buffer containing data to be displayed
	UINT8 noDigits;			//no of digits used in the application
	UINT8 digitIndex;	// index of the current digit to be displayed
	UINT16 blinkCount;	//counter to be used in blink mode
	UINT16 blinkPeriod;	//blink period represented in counts
	UINT8* dispBuffer; // pointer to current display buffer
	UINT16 blinkIndex; //counter used to blink perticular digit
	
}DigitDisplay;


/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/

static UINT8 SEVENSEGMENT[] ={0x3f,0x06,0x5b,0x4f,0x66,
							  0x6d,0x7d,0x07,0x7f,0x6f,0x00};

#pragma idata	DISPLAY_DATA
DigitDisplay digitDisplay = {0};
#pragma idata
/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/

static void writeToDisplayPort( UINT8 );
static UINT8 validate( UINT8 value );
/*
*------------------------------------------------------------------------------
* Public Functions
*------------------------------------------------------------------------------
*/




/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_init( UINT8 noDigits )
*
* Function to initialize the digit display module
* 
* Initializes digit index.
* 
*Input : number of digits used in the application
* return value: none.
* 
*------------------------------------------------------------------------------
*/

BOOL DigitDisplay_init( UINT8 noDigits )
{
	UINT8 i,j,k;
	if( noDigits > MAX_DIGITS )	//check limits 
		return FAILURE;

	digitDisplay.noDigits = noDigits;				//set no of digits 
	for( i = 0; i < digitDisplay.noDigits; i++)
	{
		digitDisplay.buffer[BLINK][i] = SEVENSEGMENT[10];		//clear buffer to be used  during blink mode
	}
	digitDisplay.dispBuffer = digitDisplay.buffer[STATIC];	//set initial display buffer to data(i.e. buffer[0])

#ifdef __DISPLAY_TEST__
	for( i = 0; i < 11 ; i++)
	{
		for(j = 0 ; j < noDigits; j++)
		{
			digitDisplay.buffer[STATIC][j] = SEVENSEGMENT[i];	
		}
		for( k = 0; k < 100; k++)
		{
			DigitDisplay_task();
			DelayMs(3);
		}
	
	}
#endif	//__DISPLAY_TEST__
	digitDisplay.digitIndex  = 0;
	
	return SUCCESS;
}

/*
*------------------------------------------------------------------------------
*void DigitDisplay_task( void )
*
* Task to refresh the display. 7 segment code corresponding
* to the data value in the displayBuffer[digitIndex] is 
* output on to the DISPLAY_PORT. If the data value is ' ' then
* all the segments are turned off.
* Input : none
* 
* return value: none.
* 
*------------------------------------------------------------------------------
*/

void DigitDisplay_task(void)
{
	switch(digitDisplay.mode)
	{
		case STATIC :
			writeToDisplayPort(digitDisplay.dispBuffer[digitDisplay.digitIndex]); 
			digitDisplay.digitIndex++;	
			if(digitDisplay.digitIndex >= digitDisplay.noDigits)
			{
				digitDisplay.digitIndex = 0;
			}
		break;

		case BLINK:

			writeToDisplayPort(digitDisplay.dispBuffer[digitDisplay.digitIndex]);
			digitDisplay.digitIndex++;	
			if(digitDisplay.digitIndex >= digitDisplay.noDigits)
			{
				digitDisplay.digitIndex = 0;
			}

			digitDisplay.blinkCount++;
			if( digitDisplay.blinkCount >= digitDisplay.blinkPeriod )
			{
				digitDisplay.blinkCount = 0;
				if( digitDisplay.dispBuffer == digitDisplay.buffer[STATIC] )
					digitDisplay.dispBuffer = digitDisplay.buffer[BLINK];
				else
					digitDisplay.dispBuffer = digitDisplay.buffer[STATIC];
			}
		break;

		default:
		break;
	}
				
		
	
}


/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateBuffer(UINT8 *buffer)
*
* Function to reset the digit index
*  
* Input : buffer containing the new values for the digits
* 
* output: the display buffer is updated with the new values
*
* return value: boolean indicating success or failure.
* 
*------------------------------------------------------------------------------
*/

BOOL DigitDisplay_updateBuffer(UINT8 *buffer)
{
	UINT8 i = 0;
	for ( i = 0 ; i < digitDisplay.noDigits ; i++)
	{

		if ( validate(buffer[i]) == FAILURE )
			return FAILURE;
	}
	for ( i = 0 ; i < digitDisplay.noDigits ; i++)
	{
		if( buffer[i] == ' ')
		{
			digitDisplay.buffer[STATIC][i] = SEVENSEGMENT[10];
		}
		else
		{
			digitDisplay.buffer[STATIC][i] = SEVENSEGMENT[buffer[i] - '0'];
		}
	}
	digitDisplay.digitIndex = 0;
	return SUCCESS;
}


/*
*------------------------------------------------------------------------------
*void DigitDisplay_updateBufferBinary(UINT8 *buffer)
*
* Function to update display buffer with binary data
*  
* Input : buffer containing the new values for the display
* 
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

BOOL DigitDisplay_updateBufferBinary(UINT8 *buffer)
{
	UINT8 i = 0;

	for ( i = 0 ; i < digitDisplay.noDigits ; i++)
	{
		digitDisplay.buffer[STATIC][i] = buffer[i] ;

	}
	digitDisplay.digitIndex = 0;
	
}



/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateDigit(UINT8 index , UINT8 value)
*
* Function to update a particular digit
*  
* Input : value - value of the digit
* 		  index - index of the digit to be updated.
*
* output: the display buffer is updated with the new value
*
* return value: boolean indicating success or failure.
* 
*------------------------------------------------------------------------------
*/

BOOL DigitDisplay_updateDigit(UINT8 index , UINT8 value)
{
	UINT8 i = 0;
	if(index > digitDisplay.noDigits )	//check if index is within limits
		return FAILURE;

	if ( validate(value) == FAILURE )
		return FAILURE;

	if( value == ' ')
	{
		digitDisplay.buffer[STATIC][index] = SEVENSEGMENT[10];
	}
	else
	{
		digitDisplay.buffer[STATIC][index] = SEVENSEGMENT[value - '0'];
	}

	return SUCCESS;
}


/*
*------------------------------------------------------------------------------
*void DigitDisplay_blinkOn(UINT16 blinkPeriod, UINT8 value)
*
* Function to switch on blink mode
*  
* Input : blinkPeriod - period of blink in millisecond
* 		  
*
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

void DigitDisplay_blinkOn(UINT16 blinkPeriod)
{
	digitDisplay.blinkPeriod = blinkPeriod / DISPLAY_REFRESH_PERIOD;	//convert period in milliseconds to period in count
	digitDisplay.blinkCount = 0;										//reset counter
	digitDisplay.mode = BLINK;											//set blink mode
}	



/*
*------------------------------------------------------------------------------
*void DigitDisplay_blinkOff()
*
* Function to switch off blink mode
*  
* Input : none
* 		  
*
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

void DigitDisplay_blinkOff()
{
	digitDisplay.dispBuffer = digitDisplay.buffer[STATIC]; //set current display buffer to data buffer
	digitDisplay.mode = STATIC;							   //set static mode
}	





/*
*------------------------------------------------------------------------------
*void DigitDisplay_clear(void)
*
* Function clear the digits
*  
* Input : none
* output: the display buffer is updated 
* return value: none
* 
*------------------------------------------------------------------------------
*/


void DigitDisplay_clear()
{
	UINT8 i;
	for( i = 0 ; i < digitDisplay.noDigits ; i++)
	{
		digitDisplay.buffer[STATIC][i] = SEVENSEGMENT[10];
	}
	digitDisplay.digitIndex = 0;
}






/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
*void writeToDisplayPort( UINT8 value )
*
* Function to output the seven segment value on to the 
* DISPLAY_PORT. digitIndex gives the digit to be 
* enabled. 
* Input : value to be output on to the DISPLAY_PORT
* 
* return value: none.
* 
*------------------------------------------------------------------------------
*/

#if defined (COMMON_CATHODE)


static void writeToDisplayPort( UINT8 value )
{
	DIGIT_SEL_A = 1;		//switch off display
	DIGIT_SEL_B = 1;
	DIGIT_SEL_C = 1;
	DIGIT_SEL_D = 1;


	Delay10us(5);
	DISPLAY_PORT = ~value;
	switch( digitDisplay.digitIndex )
	{
		case 0:
			DIGIT_SEL_A = 0;
			
		break;
		case 1:
   			DIGIT_SEL_B = 0;

		break;
		case 2:
   			DIGIT_SEL_C = 0;

   		break;

		case 3:
   			DIGIT_SEL_D = 0;
		
   		break;



		default:
		break;
	}
	Delay10us(1);
}

#else

static void writeToDisplayPort( UINT8 value )
{
	DIGIT_SEL_A = 0;		//switch off display
	DIGIT_SEL_B = 0;
	DIGIT_SEL_C = 0;
	DIGIT_SEL_D = 0;

	Delay10us(1);
	DISPLAY_PORT = value;
	switch( digitDisplay.digitIndex )
	{
		case 0:
			DIGIT_SEL_A = 1;
			
		break;
		case 1:
   			DIGIT_SEL_B = 1;

		break;
		case 2:
   			DIGIT_SEL_C = 1;

   		break;

		case 3:
   			DIGIT_SEL_D = 1;
		
   		break;



		default:
		break;
	}
	Delay10us(1);
}

#endif

/*
*------------------------------------------------------------------------------
*void DigitDisplay_DotOn(UINT8 from, UINT8 length)
*
* Function to turn on dot in the display
* 
* 
* Input : form and length 
* 
* return value: none.
* 
*------------------------------------------------------------------------------
*/
void DigitDisplay_DotOn(UINT8 from, UINT8 length)
{
	UINT8 i, data;
	for( i = from; i < length+from; i++ )
	{
		data = digitDisplay.buffer[STATIC][i];
		digitDisplay.buffer[STATIC][i] = (data | 0X80);
	}	
} 


static BOOL validate( UINT8 value)
{
	BOOL result = FAILURE;
	if( value == ' ' )
	{
		result = SUCCESS;
	}
	else if( value >= '0' && value <= '9'  )
	{
		result = SUCCESS;
	}
	return result;
}
		
/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateBuffer(UINT8 *buffer)
*
* Function to reset the digit index
*  
* Input : buffer containing the new values for the digits,
*			from - zero based index from where the update has to happen
			length - number of digits that have to be updated
* 
* output: the display buffer is updated with the new values
*
* return value: boolean indicating success or failure.
* 
*------------------------------------------------------------------------------
*/

BOOL DigitDisplay_updateBufferPartial(UINT8 *buffer, UINT8 from, UINT8 length)
{
	UINT8 i = 0;
	for ( i = from ; i < length ; i++)
	{

		if ( validate(buffer[i]) == FAILURE )
			return FAILURE;
	}
	for ( i = from ; i < from+length ; i++)
	{
		if( buffer[i] == ' ')
		{
			digitDisplay.buffer[STATIC][i] = SEVENSEGMENT[10];
		}
		else
		{
			digitDisplay.buffer[STATIC][i] = SEVENSEGMENT[buffer[i] - '0'];
		}
	}
	digitDisplay.digitIndex = 0;
	return SUCCESS;
}

/*
*------------------------------------------------------------------------------
*void DigitDisplay_blinkOn_ind(UINT16 blinkPeriod, UINT8 index)
*
* Function to switch on blink individual segment mode
*  
* Input : blinkPeriod - period of blink in millisecond
* 		  index - The digit to be blinked
*
* output: none
*
* return value: none
* 
*------------------------------------------------------------------------------
*/

void DigitDisplay_blinkOn_ind(UINT16 blinkPeriod, UINT8 index)
{
	UINT8 i;
	digitDisplay.blinkPeriod = blinkPeriod /DISPLAY_REFRESH_PERIOD;	//convert period in milliseconds to period in count
	digitDisplay.blinkCount = 0;									//reset counter

	digitDisplay.blinkIndex = index;
	
	for( i = 0 ; i < digitDisplay.noDigits ; i++)
	{
		if ( i != digitDisplay.blinkIndex )
			digitDisplay.buffer[BLINK][i] = digitDisplay.buffer[STATIC][i];
	}

	digitDisplay.buffer[BLINK][digitDisplay.blinkIndex] = 0X00;	

		digitDisplay.mode = BLINK;										//set blink mode
}


