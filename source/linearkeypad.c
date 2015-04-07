/*
*------------------------------------------------------------------------------
* linearkeypad.c
*
* Lead per key linear keypad module. 
* Port B is used as the keypad port.
* Maximum no of keys supported - 8.
* Debounce Count = 4 .
* 
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* INCLUDES
*------------------------------------------------------------------------------
*/

#include "linearkeypad.h"

#include "config.h"

/*
*------------------------------------------------------------------------------
* Structure declarations
*------------------------------------------------------------------------------
*/


typedef struct _Key
{
	KEY_STATE state;
	UINT8 debounceCount;
}Key;



/*
*------------------------------------------------------------------------------
* Private Variables (static)
*------------------------------------------------------------------------------
*/

#pragma idata LKP_DATA
UINT8 portValue[MAX_KEYS] = { 0};
Key keys[MAX_KEYS]={0};
#pragma idata


/*
*------------------------------------------------------------------------------
* Private Function Prototypes (static)
*------------------------------------------------------------------------------
*/

static void readKeyPadPort(void);
static void updateKey(Key *key , UINT8 index);
void getSimulatedInput(void);


/*
*------------------------------------------------------------------------------
* Public Functions
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

void LinearKeyPad_init( )
{
	UINT8 i;
	LinearKeyPad_reset();
	for( i = 0; i < MAX_KEYS; i++)
	{
		portValue[i] = 0x0;
	}
}


/*
*------------------------------------------------------------------------------
*void LinearKeyPad_updateKey( Key *Keys , UINT8 keyIndex )
*
*Function to update the state of a single key
*
* Input: Keys -- Array of Key objects 
*		 keyIndex -- the index of the key whose state has to be updated
*		 Keys are assumed to start from index 0.
*
* output: the state of the Key object is updated according to the
* 		  pin state of the key pad  port.
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/

void LinearKeyPad_updateKey( Key *key , UINT8 keyIndex )
{
	readKeyPadPort();
	updateKey( key , keyIndex);

}	



/*
*------------------------------------------------------------------------------
*void LinearKeyPad_scan( Key *keys , UINT8 fromIndex , UINT8 toIndex )
*
*Function to scan a subset of the keys in the linear keypad.
*This scans the keys starting from "fromIndex" to "toIndex"
* both inclusive.
*
*
* Input: keys -- Array of Key objects 
*		 fromIndex -- starting index of the key.
*		 toIndex -- ending index of the key.
*
* output: the state of each of the Key objects is updated according to the
* 		  pin state of the key pad  port.
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/

void LinearKeyPad_scan_from_to( Key *keys , UINT8 fromIndex , UINT8 toIndex )
{
	UINT8 i;
	for( i = fromIndex ; i <= toIndex ; i++)
	{
		readKeyPadPort();
		updateKey( &keys[i] , i );
	}
}


/*
*------------------------------------------------------------------------------
*void LinearKeyPad_scan(  )
*
*Function to scan the linear keypad.
*
* Input: keys -- Array of Key objects 
*		 noKeys -- number of Key objects
*
* output: the state of each of the Key objects is updated according to the
* 		  pin state of the key pad  port.
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/


void LinearKeyPad_scan(  )
{
	UINT8 i = 0;


#ifdef __SIMULATION__
	getSimulatedInput();
#else
	readKeyPadPort();
#endif

	for( i = 0; i < MAX_KEYS ; i++)
	{
		updateKey( &keys[i] , i );
	}



}
		
	

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


UINT8 LinearKeyPad_getKeyState( UINT8 index )
{
	return keys[index].state;
	
}


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


void LinearKeyPad_reset(  )
{
	UINT8 i;
	for( i = 0 ; i < MAX_KEYS ; i++)
	{
		keys[i].state = KEY_RELEASED;
		keys[i].debounceCount = 0;
	}



}




/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/
#ifdef __SIMULATION__
 

static UINT8 portIndex = 0;
	
void getSimulatedInput()
{
/*	if( portIndex %3 == 0 )
	{
		portIndex++;
		return;
	}
*/
	if(portValue[portIndex] == 0)
		portValue[portIndex] = 1;
	else if( portValue[portIndex] == 1 )
		portValue[portIndex ]= 0;
	portIndex++;

	if(portIndex > MAX_KEYS)
		portIndex = 0;
}
#endif
/*
*------------------------------------------------------------------------------
*void readKeyPadPort(  )
*
*function to read the key pad port.
*
* Input: none
*
* output: the static variable portValue is updated with the keypad port value
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/

static void readKeyPadPort()
{
	portValue[0] = KEYPAD_PORT_0;
	portValue[1] = KEYPAD_PORT_1;
	portValue[2] = KEYPAD_PORT_2;
	portValue[3] = KEYPAD_PORT_3;


}


/*
*------------------------------------------------------------------------------
*void updateKey(  )
*
*function to read the key pad port.
*
* Input: none
*
* output: the static variable portValue is updated with the keypad port value
*
* return value: none.
* 
*------------------------------------------------------------------------------
*/

static void updateKey(Key *key , UINT8 index)
{

		
	
		switch( key->state)
		{
			case KEY_RELEASED:
				if( ( portValue[index] & 1 )  == PIN_CLOSED )
				{
					(key->debounceCount)++;
					if((key->debounceCount) >= DEBOUNCE_COUNT ) 
					{
						key->state = KEY_PRESSED;
						key->debounceCount = 0;
					}
				}
				else 
				{
						key->debounceCount = 0;
				}
				
					
			break;
			
			case KEY_PRESSED:
				if( ( portValue[index] & 1) == PIN_OPEN   )
				{
					(key->debounceCount)++;
					if((key->debounceCount) >= DEBOUNCE_COUNT ) 
					{
						key->state = KEY_RELEASED;
						key->debounceCount = 0;
					}
				}

				else 
				{
						key->debounceCount = 0;
				}
			break;

			default:
			break;
		}
}