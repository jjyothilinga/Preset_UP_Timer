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
UINT8 max[NO_OF_DIGITS] = {0x39,0x35,0x39,0x39};
UINT8 readTimeDateBuffer[6] = {0};
UINT8 writeTimeDateBuffer[] = {0X50, 0X59, 0X00, 0X03, 0x027, 0X12, 0X13};

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
	UINT8 displayBuffer[6];			//store for display the rtc value 
	UINT8 presetBuffer[NO_OF_DIGITS];//store the preset value
	UINT8 prevDisplayBuffer[6]; 	//store rtc value for comparision
	UINT8 blinkIndex;				//index to blink the DIGIT
	UINT8 hooter_set;				//indicator for HOOTER on
	UINT8 hooter_reset;				//indicator for HOOTER off
	UINT16 presetValue;				// store
	UINT16 rtcValue;
	BOOL countFlag;
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

	//	writeTimeDateBuffer[2] = SetHourMode(0X09,1,1);
	//Set Date and Time
	//WriteRtcTimeAndDate(writeTimeDateBuffer);

	//Read the value of PRESET TIME from EPROM
	for(i = 0 ; i < NO_OF_DIGITS;i++)
	{
		app.presetBuffer[i] = Read_b_eep (EEPROM_PRESET_ADDRESS  + i);  
		Busy_eep();	
	}

	app.presetValue = (UINT16)(((app.presetBuffer[3]- '0' )* 10 )+ ( app.presetBuffer[2] - '0') )* 60 +(((app.presetBuffer[1]- '0' )* 10 )+ (app.presetBuffer[0] - '0'));
	
	//Maintain the state from EPROM
	app.state = Read_b_eep (EEPROM_STATE_ADDRESS);  
	Busy_eep();


	//Read the STORED RTC value from EPROM
	for(i = 0 ; i < 6 ;i++)
	{
		app.displayBuffer[i] = Read_b_eep (EEPROM_RTC_ADDRESS + i);  
		Busy_eep();	
	}

	//update that value on RTC
	APP_updateRTC();

	//Maintain the HOOTER state from EPROM
	HOOTER = Read_b_eep (EEPROM_HOOTER_ADDRESS);  
	Busy_eep();
	//Maintain the hooter_reset value from EPROM
	app.hooter_reset = Read_b_eep (EEPROM_HOOTER_ADDRESS + 1);  
	Busy_eep();
	if(HOOTER  == TRUE)
	{
		DigitDisplay_updateBuffer(app.presetBuffer);
		CLOCK_LED = 1;	
	}
	//ON DOT_CONTROL leds
	CLOCK_LED = 0;


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


		switch(app.state)
		{
			case HALT_STATE:
			
			//Check the keypress Status of COUNT_PB 
			if ((LinearKeyPad_getKeyState(COUNT_PB) == TRUE) && (app.countFlag == FALSE))
			{
				app.countFlag = TRUE;
				//Reset buffer and RTC
				APP_resetDisplayBuffer();
				DigitDisplay_updateBufferPartial(app.displayBuffer , 0, 4);
				APP_updateRTC();

				//Turn off hooter
				HOOTER = RESET;
				app.hooter_reset = FALSE;
				/*
				//Store the state
				Write_b_eep( EEPROM_STATE_ADDRESS , COUNT_STATE);
				Busy_eep( );
				//ON DOT_CONTROL leds
				CLOCK_LED = 1;
				//Change the state to STOP state
				app.state = COUNT_STATE;
				*/
				
			}
			else if ((LinearKeyPad_getKeyState(COUNT_PB) == FALSE) && (app.countFlag == TRUE))
			{
				app.countFlag = FALSE;	
				//Store the state
				Write_b_eep( EEPROM_STATE_ADDRESS , COUNT_STATE);
				Busy_eep( );
				//ON DOT_CONTROL leds
				CLOCK_LED = 1;
				//Change the state to STOP state
				app.state = COUNT_STATE;
			
			}
			//Check the keypress Status of MODE_CHANGE_PB
			else if (LinearKeyPad_getKeyState(MODE_CHANGE_PB) == TRUE)
			{
				//Reset Target buffer which is used to hold preset data
				APP_resetPresetBuffer();

				//Blink first digit in the display
				app.blinkIndex = 0;
				DigitDisplay_updateBuffer(app.presetBuffer);
				DigitDisplay_blinkOn_ind(500, app.blinkIndex);


				//Change state to setting state
				app.state = SETTING_STATE;
			
			}

			//Check the keypress Status of HOOTER_OFF_PB 
			else if((LinearKeyPad_getKeyState(HOOTER_OFF_PB) == TRUE) && (app.hooter_reset == TRUE))
				{
					HOOTER = RESET;
					Write_b_eep( EEPROM_HOOTER_ADDRESS, HOOTER);
					Busy_eep( );
					app.state = HALT_STATE;	
				}

			
			break;

			case SETTING_STATE:
			
				//ON DOT_CONTROL leds
			CLOCK_LED = ~CLOCK_LED;
			// Code to handle Digit index PB
			if (LinearKeyPad_getKeyState(DIGIT_INDEX_PB ) == TRUE) 
			{
				app.blinkIndex++;

				if(app.blinkIndex > (NO_OF_DIGITS -  1))
					app.blinkIndex = 0 ;

				DigitDisplay_blinkOn_ind(500, app.blinkIndex);

			}

			// Code to handle increment PB
			if (LinearKeyPad_getKeyState(INCREMENT_PB) == 1) 
			{
				app.presetBuffer[app.blinkIndex]++;
				if(app.presetBuffer[app.blinkIndex] > max[app.blinkIndex])
					app.presetBuffer[app.blinkIndex] = '0';			
						
				DigitDisplay_updateBuffer(app.presetBuffer);

			}
			
			if (LinearKeyPad_getKeyState(MODE_CHANGE_PB) == TRUE)
			{
				//Turn of blink
				DigitDisplay_blinkOff();

				//Display the preset value on the display
				DigitDisplay_updateBuffer(app.presetBuffer);


				//Store preset value in the EEPROM
				for(i = 0; i < NO_OF_DIGITS ; i++ )
				{
					Write_b_eep( EEPROM_PRESET_ADDRESS + i ,app.presetBuffer[i] );
					Busy_eep( );
				}

				app.presetValue = (UINT16)(((app.presetBuffer[3]- '0' )* 10 )+ ( app.presetBuffer[2] - '0') )* 60 +(((app.presetBuffer[1]- '0' )* 10 )+ (app.presetBuffer[0] - '0'));
				
				//Store state in the EEPROM
				Write_b_eep( EEPROM_STATE_ADDRESS , HALT_STATE);
				Busy_eep( );
				//Switch state
				CLOCK_LED = 1;
				app.state = HALT_STATE;	
				
			}

				
			break;
			
			case COUNT_STATE:

				app.hooter_set = FALSE;
	
				// On halt PB press change the state into HALT
			if ((LinearKeyPad_getKeyState(HALT_PB) == TRUE) && (app.countFlag == FALSE))
				{
					app.countFlag = TRUE;
				}

			if ((LinearKeyPad_getKeyState(HALT_PB) == FALSE) && (app.countFlag == TRUE))
				{
					app.countFlag = FALSE;				
					Write_b_eep( EEPROM_STATE_ADDRESS ,HALT_STATE);
					Busy_eep( );
	
					app.state = HALT_STATE;
					break;
				}

				// On hooter_off PB press change hooter state by checking condition
/*
				if((LinearKeyPad_getKeyState(HOOTER_OFF_PB) == TRUE) && (app.hooter_reset == TRUE))
				{
					HOOTER = RESET;
					Write_b_eep( EEPROM_HOOTER_ADDRESS, HOOTER);
					Busy_eep( );
				}
*/

				//Read RTC data and store it in buffer
				ReadRtcTimeAndDate(readTimeDateBuffer);  

				//Convert RTC data in to ASCII
				// Separate the higher and lower nibble and store it into the display buffer 
				APP_conversion(); 

				//Store the current RTC data into EEPROM

				for(i = 0 ; i < 6  ; i++)
				{
					if( app.displayBuffer[i] !=	app.prevDisplayBuffer[i] )
					{
						Write_b_eep( (EEPROM_RTC_ADDRESS + i) , app.displayBuffer[i]);
						Busy_eep( );
					}
				}

				//manipulate the MSB_min to display upto 99m:59s 
				if(readTimeDateBuffer[2] > 0)
				{
					app.displayBuffer[3] += '6';
				}
				else 
				{
					app.displayBuffer[3] +=  '0';
				}
				//calculate and store the rtc value in the form of SEC
				app.rtcValue = (UINT16)(((	app.displayBuffer[3]- '0' )* 10 )+ ( app.displayBuffer[2] - '0') )* 60 +((( app.displayBuffer[1]- '0' )* 10 )+ ( app.displayBuffer[0] - '0'));
			
				//check rtcValue and presetValue for turn ON hooter_set
				if((app.hooter_reset == FALSE) && ( app.rtcValue >= app.presetValue ) )
				{
					app.hooter_set = TRUE;

				}

				//check hooter_set for change the HOOTER STATE
				if( app.hooter_set == TRUE) 
				{
					HOOTER = SET;
					app.hooter_reset = TRUE;
					Write_b_eep( EEPROM_HOOTER_ADDRESS, HOOTER);
					Busy_eep( );
					Write_b_eep( (EEPROM_HOOTER_ADDRESS + 1) , app.hooter_reset);
					Busy_eep( );

				}

				//Update digit display buffer with the current data of RTC
				DigitDisplay_updateBufferPartial(app.displayBuffer , 0, 4);

				if(readTimeDateBuffer[2] > 0)
				{
					app.displayBuffer[3] -= '6';
				}
				else 
				{
					app.displayBuffer[3] -=  '0';
				}

				for(	i = 0 ;	i < 6 ;	i++)
				{
					app.prevDisplayBuffer[i] = app.displayBuffer[i];
				}

				if(((readTimeDateBuffer[2] == 0x01) && (readTimeDateBuffer[1] == 0X39) &&
					(readTimeDateBuffer[0] == 0X59)) || (app.hooter_set == TRUE) )
				{
			
					//Change the state
					Write_b_eep( EEPROM_STATE_ADDRESS , HALT_STATE);
					Busy_eep( );
			
					app.state = HALT_STATE;	

					return;
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
			
	app.displayBuffer[0] = (readTimeDateBuffer[0] & 0X0F) + '0';        //Seconds LSB
	app.displayBuffer[1] = ((readTimeDateBuffer[0] & 0XF0) >> 4) + '0'; //Seconds MSB
	app.displayBuffer[2] = (readTimeDateBuffer[1] & 0X0F) + '0';        //Minute LSB
	app.displayBuffer[3] = ((readTimeDateBuffer[1] & 0XF0) >> 4) ; 		//Minute MSB
	app.displayBuffer[4] = (readTimeDateBuffer[2] & 0X0F) + '0';        //Minute LSB
	app.displayBuffer[5] = ((readTimeDateBuffer[2] & 0X30) >> 4) ; 		//Minute MSB

}


void APP_resetDisplayBuffer(void)
{
	int i ;
	for(i = 0; i < 6; i++)			//reset all digits
	{
		app.displayBuffer[i] = '0';
	}
}

	
void APP_resetPresetBuffer(void)
{
	int i ;
	for(i = 0; i < NO_OF_DIGITS ; i++)			//reset all digits
	{
		app.presetBuffer[i] = '0';
	}
}



void APP_updateRTC(void)
{
	writeTimeDateBuffer[0] = ((app.displayBuffer[1] - '0') << 4) | (app.displayBuffer[0] - '0'); //store seconds
	writeTimeDateBuffer[1] = ((app.displayBuffer[3] - '0') << 4) | (app.displayBuffer[2] - '0'); //store minutes
	writeTimeDateBuffer[2] = ((app.displayBuffer[5] - '0') << 4) | (app.displayBuffer[4] - '0'); //store minutes

	WriteRtcTimeAndDate(writeTimeDateBuffer);  //update RTC
}



	