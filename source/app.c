#include "app.h" 
#include "uart.h"
#include "string.h"
#include "linearkeypad.h"
#include "eep.h"


#define NO_OF_DIGITS	(0X04)


/*
*------------------------------------------------------------------------------
* Public Variables
* Buffer[0] = seconds, Buffer[1] = minutes, Buffer[2] = Hour,
* Buffer[3] = day, Buffer[4] = date, Buffer[5] = month, Buffer[6] = year
*------------------------------------------------------------------------------
*/
UINT8 txBuffer[6] = {0};
UINT8 displayBuffer[6] = {0};
UINT8 presetBuffer[NO_OF_DIGITS]  = {0};
UINT8 max[NO_OF_DIGITS] = {0x39,0x35,0x39,0x39};
UINT8 readTimeDateBuffer[6] = {0};
UINT8 writeTimeDateBuffer[] = {0X50, 0X59, 0X00, 0X03, 0x027, 0X12, 0X13};
extern BOOL portB_intFlag;
extern UINT8 portB_currentData;


void APP_conversion(void);
void APP_resetDisplayBuffer(void);
void APP_updateRTC(void);
void APP_resetPresetBuffer(void);


/*
*------------------------------------------------------------------------------
* app - the app structure. 
*------------------------------------------------------------------------------
*/
typedef struct _App
{
	APP_STATE state;				//maintain the state 
	UINT8 blinkIndex;				//index to blink the DIGIT
	UINT8 hooter_set;				//indicator for HOOTER on
	BOOL hooter_reset;				//indicator for HOOTER off
	UINT16 presetValue;

}APP;

#pragma idata app_data
APP app = {0};
#pragma idata


/*
*------------------------------------------------------------------------------
* void APP_init(void)
*
* Summary	: Initialize application
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/
void APP_init( void )
{
	UINT8 i;
	UINT16 temp;
	UINT16 temp1 = 60;

	//	writeTimeDateBuffer[2] = SetHourMode(0X09,1,1);
	//Set Date and Time
	//WriteRtcTimeAndDate(writeTimeDateBuffer);


	//Read the value of TARGET TIME from EPROM
	for(i = 0 ; i < NO_OF_DIGITS;i++)
	{
		presetBuffer[i] = Read_b_eep (EEPROM_PRESET_ADDRESS  + i);  
		Busy_eep();	
	}
	//Maintain the state from EPROM
	app.state = Read_b_eep (EEPROM_STATE_ADDRESS);  
	Busy_eep();


	//Read the STORED RTC value from EPROM
	for(i = 0 ; i < 6 ;i++)
	{
		displayBuffer[i] = Read_b_eep (EEPROM_RTC_ADDRESS + i);  
		Busy_eep();	
	}

	if(displayBuffer[3] > '6')
		displayBuffer[3] -='6';

	//update that value on RTC
	APP_updateRTC();

/*
	temp = ((presetBuffer[2] - '0') | (presetBuffer[3] - '0' << 4));

	app.presetValue  = (presetBuffer[0] - '0') | (presetBuffer[1] - '0' << 4);
	
	temp = app.presetValue * temp1;
*/

	//Maintain the HOOTER state from EPROM
	HOOTER = Read_b_eep (EEPROM_HOOTER_ADDRESS);  
	Busy_eep();
				
	app.hooter_reset = FALSE;

	CLOCK_LED = 1;


}


/*
*------------------------------------------------------------------------------
* void APP_task(void)
*
* Summary	: 
*
* Input		: None
*
* Output	: None
*------------------------------------------------------------------------------
*/


void APP_task( void )
{
	UINT8 i;
	unsigned long compare;

		switch(app.state)
		{
			case HALT_STATE:
			
			//Check the keypress Status of START PB 
			if (LinearKeyPad_getKeyState(COUNT_PB) == TRUE)
			{
				//Reset buffer and RTC
				APP_resetDisplayBuffer();
				DigitDisplay_updateBufferPartial(displayBuffer , 0, 4);
				APP_updateRTC();

				//Turn off hooter
				HOOTER = RESET;
				app.hooter_reset = FALSE;
				
				//Store the state
				Write_b_eep( EEPROM_STATE_ADDRESS , COUNT_STATE);
				Busy_eep( );
		
				//Change the state to STOP state
				app.state = COUNT_STATE;
				
			}
			else if (LinearKeyPad_getKeyState(MODE_CHANGE_PB) == TRUE)
			{
				//Reset Target buffer which is used to hold preset data
				APP_resetPresetBuffer();

				//Blink first digit in the display
				app.blinkIndex = 0;
				DigitDisplay_updateBuffer(presetBuffer);
				DigitDisplay_blinkOn_ind(500, app.blinkIndex);


				//Change state to setting state
				app.state = SETTING_STATE;
			
			}
			
			break;

			case SETTING_STATE:

			// Code to handle Digit index PB
			if (LinearKeyPad_getKeyState(DIGIT_INDEX_PB ) == TRUE) 
			{
				app.blinkIndex++;

				if(app.blinkIndex > (NO_OF_DIGITS - 1))
					app.blinkIndex = 0 ;

				DigitDisplay_blinkOn_ind(500, app.blinkIndex);

			}

			// Code to handle increment PB
			if (LinearKeyPad_getKeyState(INCREMENT_PB) == 1) 
			{
				presetBuffer[app.blinkIndex]++;
				if(presetBuffer[app.blinkIndex] > max[app.blinkIndex])
					presetBuffer[app.blinkIndex] = '0';			
						
				DigitDisplay_updateBuffer(presetBuffer);

			}
			
			if (LinearKeyPad_getKeyState(MODE_CHANGE_PB) == TRUE)
			{
				//Turn of blink
				DigitDisplay_blinkOff();

				//Display the preset value on the display
				DigitDisplay_updateBuffer(presetBuffer);

				//Store preset value in the EEPROM
				for(i = 0; i < NO_OF_DIGITS ; i++ )
				{
					Write_b_eep( EEPROM_PRESET_ADDRESS + i ,presetBuffer[i] );
					Busy_eep( );
				}

				//Store state in the EEPROM
				Write_b_eep( EEPROM_STATE_ADDRESS , HALT_STATE);
				Busy_eep( );
				//Switch state
				app.state = HALT_STATE;	
				
			}

				
			break;
			
			case COUNT_STATE:

				app.hooter_set = 0;
			
	
				// On start PB press change the state into START
				if(LinearKeyPad_getKeyState(HALT_PB) == TRUE)
				{
	
					Write_b_eep( EEPROM_STATE_ADDRESS ,HALT_STATE);
					Busy_eep( );
	
					app.state = HALT_STATE;
					break;
				}


				if((LinearKeyPad_getKeyState(HOOTER_OFF_PB) == TRUE) && (app.hooter_reset == TRUE))
				{
					HOOTER = RESET;
					Write_b_eep( EEPROM_HOOTER_ADDRESS, HOOTER);
					Busy_eep( );
				}


				//Read RTC data and store it in buffer
				ReadRtcTimeAndDate(readTimeDateBuffer);  
				//Convert RTC data in to ASCII
				// Separate the higher and lower nibble and store it into the display buffer 
				APP_conversion(); 
				//Update digit display buffer with the current data of RTC
				DigitDisplay_updateBufferPartial(displayBuffer , 0, 4);


			//	compare = readTimeDateBuffer[0] + readTimeDateBuffer[1]*60 + readTimeDateBuffer[2]*60;	
			
				for(i = 0 ; i < 4 ; i++)
				{
					if(displayBuffer[i] >= presetBuffer[i])
					{
						++app.hooter_set;
					}
				}
				
				if(( app.hooter_set == 4) && (app.hooter_reset == FALSE))
				{
					HOOTER = SET;
					app.hooter_reset = TRUE;
					Write_b_eep( EEPROM_HOOTER_ADDRESS, HOOTER);
					Busy_eep( );
				}

				app.hooter_set = 0;
				//Store the current RTC data into EEPROM
				for(i = 0 ; i < 6  ; i++)
				{
					Write_b_eep( (EEPROM_RTC_ADDRESS + i) , displayBuffer[i]);
					Busy_eep( );
				}


#if defined (RTC_DATA_ON_UART)
				for(i = 0; i < 7; i++)			
				{
					txBuffer[i] = readTimeDateBuffer[i];  //store time and date 
				}
				
				COM_txBuffer(txBuffer, 7);
#endif	

				break;			
		}
			
}		


void APP_conversion(void)
{

	volatile UINT8 temp = 0, temp1, temp2;
	UINT8 i;



	if(readTimeDateBuffer[2] > 0)
	{
		temp = '6';
	}
	else 
		temp = '0';

	if((readTimeDateBuffer[2] == 0x01) && (readTimeDateBuffer[1] == 0X39) &&
		(readTimeDateBuffer[0] == 0X59))
	{
		//Store current data into EEPROM

		//Change the state
		Write_b_eep( EEPROM_STATE_ADDRESS , HALT_STATE);
		Busy_eep( );

		app.state = HALT_STATE;	
	}
			
	displayBuffer[0] = (readTimeDateBuffer[0] & 0X0F) + '0';        //Seconds LSB
	displayBuffer[1] = ((readTimeDateBuffer[0] & 0XF0) >> 4) + '0'; //Seconds MSB
	displayBuffer[2] = (readTimeDateBuffer[1] & 0X0F) + '0';        //Minute LSB
	displayBuffer[3] = ((readTimeDateBuffer[1] & 0XF0) >> 4) + temp; //Minute MSB

}


void APP_resetDisplayBuffer(void)
{
	int i ;
	for(i = 0; i < 6; i++)			//reset all digits
	{
		displayBuffer[i] = '0';
	}
}

	
void APP_resetPresetBuffer(void)
{
	int i ;
	for(i = 0; i < NO_OF_DIGITS ; i++)			//reset all digits
	{
		presetBuffer[i] = '0';
	}
}



void APP_updateRTC(void)
{
	writeTimeDateBuffer[0] = ((displayBuffer[1] - '0') << 4) | (displayBuffer[0] - '0'); //store seconds
	writeTimeDateBuffer[1] = ((displayBuffer[3] - '0') << 4) | (displayBuffer[2] - '0'); //store minutes
	writeTimeDateBuffer[2] = ((displayBuffer[5] - '0') << 4) | (displayBuffer[4] - '0'); //store minutes

#if defined (MODE_12HRS)
	//Set 6th bit of Hour register to enable 12 hours mode.
	writeTimeDateBuffer[2] |= 0x40;
#endif
	WriteRtcTimeAndDate(writeTimeDateBuffer);  //update RTC
}



	