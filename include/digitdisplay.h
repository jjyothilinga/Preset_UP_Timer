/*
*------------------------------------------------------------------------------
* digitdisplay.h
*
* Header for display module supporting 7 segment digit displays.
* Data to be displayed is either a digit(0-9) or space(' ')
* Space represents all clear i.e all segments of the 7 segment display
* turned off.
*------------------------------------------------------------------------------
*/


#ifndef __DIGIT_DISPLAY__
#define __DIGIT_DISPLAY__


/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/
#include "config.h"
#include "typedefs.h"
#include "board.h"

/*
*------------------------------------------------------------------------------
* DEFINES
*------------------------------------------------------------------------------
*/

#ifndef DISPLAY_PORT
#assert( " DISPLAY PORT UNDEFINED")
#endif

#define DISPLAY_REFRESH_PERIOD	2	//period of refresh in milliseconds


/*
*------------------------------------------------------------------------------
* ENUMERATIONS
*------------------------------------------------------------------------------
*/

typedef enum
{
	MAX_DIGITS = 8
}DIGITDISPLAY_CONFIG;


/*
*------------------------------------------------------------------------------
* Function declarations
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
*Input : none
* return value: none.
* 
*------------------------------------------------------------------------------
*/

BOOL DigitDisplay_init( UINT8 noDigits );


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

void DigitDisplay_task(void);


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

BOOL DigitDisplay_updateBuffer(UINT8 *buffer);

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

BOOL DigitDisplay_updateBufferBinary(UINT8 *buffer);




/*
*------------------------------------------------------------------------------
*BOOL DigitDisplay_updateDigit(UINT8 value , UINT8 index)
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

BOOL DigitDisplay_updateDigit(UINT8 index , UINT8 value);


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


void DigitDisplay_clear(void);

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
void DigitDisplay_blinkOn(UINT16 blinkPeriod);


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

void DigitDisplay_blinkOff(void);

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

void DigitDisplay_DotOn(UINT8 from, UINT8 length);


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

BOOL DigitDisplay_updateBufferPartial(UINT8 *buffer, UINT8 from, UINT8 length);

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

void DigitDisplay_blinkOn_ind(UINT16 blinkPeriod, UINT8 index);

#endif