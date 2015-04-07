/*
*------------------------------------------------------------------------------
* linearkeypad.h
*
* Header for a lead per key linear keypad module. 
* Port B is used as the keypad port.
* Maximum no of keys supported - 8.
* Debounce Count = 4.
*
*
*------------------------------------------------------------------------------
*/

#ifndef __LINEAR_KEYPAD__
#define __LINEAR_KEYPAD__

/*
*------------------------------------------------------------------------------
* INCLUDES
*------------------------------------------------------------------------------
*/
#include "board.h"
#include "typedefs.h"
#include "config.h"


/*
*------------------------------------------------------------------------------
* DEFINES
*------------------------------------------------------------------------------
*/



/*
*------------------------------------------------------------------------------
* ENUMERATIONS
*------------------------------------------------------------------------------
*/



/*-----------------------------------------------------------------------------
* LINEAR_KEYPAD_CONFIG - enumeration providing the configuration constants
* for the linear keypad
*------------------------------------------------------------------------------
*/

enum LINEAR_KEYPAD_CONFIG
{
	DEBOUNCE_COUNT = 2	
	
};



/*
*------------------------------------------------------------------------------
* PIN_STATE - enumeration for the states of the port pins.
*------------------------------------------------------------------------------
*/
typedef enum PIN_STATE
{
#ifdef __SIMULATION__
	PIN_CLOSED = 1,
	PIN_OPEN = 0
#else
	PIN_CLOSED = 0,
	PIN_OPEN = 1
#endif
}PIN_STATE;


/*
*------------------------------------------------------------------------------
* KEY_STATE - enumeration for the states of each individual key.
*------------------------------------------------------------------------------
*/
typedef enum KEY_STATE
{
	KEY_RELEASED,
	KEY_PRESSED
}KEY_STATE;

/*
*------------------------------------------------------------------------------
* KEYS- enumeration for the  keyS.
*------------------------------------------------------------------------------
*/
typedef enum KEY
{

	KEY0 = 0,
	KEY1,
	KEY2,
	KEY3,
	KEY4,

	MAX_KEYS
}KEY;





/*
*------------------------------------------------------------------------------
* Function declarations
*------------------------------------------------------------------------------
*/


/*
*------------------------------------------------------------------------------
*void LinearKeyPad_init(  )
*
*Function to initialize the linear keypad. 
* 1.Sets the direction of the keypad port i.e PORTB to input.
* 2. Initializes the state and debounceCount of the Key objects
* Input: keys -- Array of Key objects 
*		 noKeys -- number of Key objects
*		 Keys are assumed to start from index 0.
*
* output: the direction of keypad port is initialized. The Key objects
* 			are initialized.
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/

void LinearKeyPad_init(void );





/*
*------------------------------------------------------------------------------
*void LinearKeyPad_scan(  )
*
*Function to scan the linear keypad.
*
* Input: keys -- Array of Key objects 
*		 noKeys -- number of Key objects
*		 Keys are assumed to start from index 0.
*
* output: the state of each of the Key objects is updated according to the
* 		  pin state of the key pad  port.
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/

void LinearKeyPad_scan(void );


/*
*------------------------------------------------------------------------------
*void LinearKeyPad_reset( )
*
*Function to reset the linear keypad.
*
* Input: none
*
* output: the state and debounce count of the keys are reset.
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/


void LinearKeyPad_reset(void);





/*
*------------------------------------------------------------------------------
*UINT8 LinearKeyPad_getKeyState( UINT8 index )
*
*Function which returns the state of the key 
*
* Input: index - index of the key whose state is requested
*
*
* output: NONE
*
* return value: state of the key -- KEY_PRESSED/KEY_RELEASED
* 
*------------------------------------------------------------------------------
*/


UINT8 LinearKeyPad_getKeyState( UINT8 index );



#endif	//__LINEAR_KEYPAD__