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
UINT8 displayBuffer[NO_OF_DIGITS] = {0};
UINT8 targetBuffer[NO_OF_DIGITS]  = {0};
UINT8 max[NO_OF_DIGITS] = {0x39,0x35,0x39,0x39};
UINT8 readTimeDateBuffer[6] = {0};
UINT8 writeTimeDateBuffer[] = {0X50, 0X59, 0X00, 0X03, 0x027, 0X12, 0X13};
extern BOOL portB_intFlag;
extern UINT8 portB_currentData;


void APP_conversion(void);
void APP_resetDisplayBuffer(void);
void APP_updateRTC(void);
void APP_resetTargetBuffer(void);


/*
*------------------------------------------------------------------------------
* app - the app structure. 
*------------------------------------------------------------------------------
*/
typedef struct _App
{
	APP_STATE state;

	UINT8 Input_Recieved;
	UINT8 incrementPBpressed;
	UINT8 digitIndexPBpressed;
	UINT8 setPBpressed;
	UINT8 blinkIndex;
	BOOL hooter_set;
	BOOL hooter_reset;

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

	for(i = 0 ; i < NO_OF_DIGITS;i++)
	{
		targetBuffer[i] = Read_b_eep (EEPROM_TARGET_ADDRESS + i);  
		Busy_eep();	
	}

	app.state = Read_b_eep (EEPROM_STATE_ADDRESS);  
	Busy_eep();


	switch(app.state)
	{
		case START:

				for(i = 0 ; i < NO_OF_DIGITS ;i++)
				{
					displayBuffer[i] = Read_b_eep (EEPROM_RTC_ADDRESS + i);  
					Busy_eep();	
				}
			//	APP_updateRTC();
				DigitDisplay_updateBuffer(displayBuffer);

		break;
		case SETTING:
		break;
		case STOP:
			
				for(i = 0 ; i < NO_OF_DIGITS ;i++)
				{
					displayBuffer[i] = Read_b_eep (EEPROM_RTC_ADDRESS + i);  
					Busy_eep();	
				}
				APP_updateRTC();
				DigitDisplay_updateBuffer(displayBuffer);

				
		break;

		default:
		break;
	}


	HOOTER = Read_b_eep (EEPROM_STATE_ADDRESS + 1);  
	Busy_eep();


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
			case START:
			CLOCK_LED = 1;

			if ((LinearKeyPad_getKeyState(START_PB) == TRUE)&& (app.Input_Recieved == FALSE))
			{
				//Reset buffer and RTC
				APP_resetDisplayBuffer();
				DigitDisplay_updateBuffer(displayBuffer);
				APP_updateRTC();

				//Turn off hooter
				HOOTER = RESET;
				app.hooter_reset = FALSE;

				app.Input_Recieved = TRUE;
				
				//Store the state
				Write_b_eep( EEPROM_STATE_ADDRESS , STOP);
				Busy_eep( );
		
				//Change the state to STOP state
				app.state = STOP;
				
			}
			else if ((LinearKeyPad_getKeyState(MODE_CHANGE_PB) == TRUE)&& (app.setPBpressed == FALSE))
			{
				//Reset Target buffer which is used to hold preset data
				APP_resetTargetBuffer();

				//Blink first digit in the display
				app.blinkIndex = 0;
				DigitDisplay_updateBuffer(targetBuffer);
				DigitDisplay_blinkOn_ind(500, app.blinkIndex);

				app.setPBpressed = TRUE;

				//Change state to setting state
				app.state = SETTING;
			
			}
			

			if(LinearKeyPad_getKeyState(START_PB) == FALSE)
			{
				app.Input_Recieved = FALSE;
			
			}

			if ((LinearKeyPad_getKeyState(MODE_CHANGE_PB) == 0)&& (app.setPBpressed == TRUE))
				app.setPBpressed = FALSE;

			break;

			case SETTING:

			CLOCK_LED = 1;

			// Code to handle Digit index PB
			if ((LinearKeyPad_getKeyState(DIGIT_INDEX_PB ) == TRUE) && (app.digitIndexPBpressed == FALSE ))
			{
				app.blinkIndex++;

				if(app.blinkIndex > (NO_OF_DIGITS - 1))
					app.blinkIndex = 0 ;

				DigitDisplay_blinkOn_ind(500, app.blinkIndex);

				app.digitIndexPBpressed = TRUE;
			}
			else if ((LinearKeyPad_getKeyState(DIGIT_INDEX_PB ) == FALSE) && (app.digitIndexPBpressed == TRUE ))
				app.digitIndexPBpressed = FALSE;

			// Code to handle increment PB
			if ((LinearKeyPad_getKeyState(INCREMENT_PB) == 1) && (app.incrementPBpressed == FALSE ))
			{
				targetBuffer[app.blinkIndex]++;
				if(targetBuffer[app.blinkIndex] > max[app.blinkIndex])
					targetBuffer[app.blinkIndex] = '0';			
						
				DigitDisplay_updateBuffer(targetBuffer);

				app.incrementPBpressed = TRUE;

			}
			else if ((LinearKeyPad_getKeyState(INCREMENT_PB) == FALSE) && (app.incrementPBpressed == TRUE ))
				app.incrementPBpressed = FALSE;

			
			if ((LinearKeyPad_getKeyState(MODE_CHANGE_PB) == TRUE)&& (app.setPBpressed == FALSE))
			{
				//Turn of blink
				DigitDisplay_blinkOff();

				//Display the preset value on the display
				DigitDisplay_updateBuffer(targetBuffer);
				CLOCK_LED = 1;
				app.setPBpressed = TRUE;

				//Store preset value in the EEPROM
				for(i = 0; i < NO_OF_DIGITS ; i++ )
				{
					Write_b_eep( EEPROM_TARGET_ADDRESS + i ,targetBuffer[i] );
					Busy_eep( );
				}

				//Store state in the EEPROM
				Write_b_eep( EEPROM_STATE_ADDRESS , START);
				Busy_eep( );
				//Switch state
				app.state = START;	
				
			}
			else if ((LinearKeyPad_getKeyState(MODE_CHANGE_PB) == FALSE)&& (app.setPBpressed == TRUE))
				app.setPBpressed = FALSE;
				
			break;
			
			case STOP:

				app.hooter_set = FALSE;
		
				// On start PB press change the state into START
				if((LinearKeyPad_getKeyState(START_PB) == TRUE) && (app.Input_Recieved == FALSE))
				{
					app.Input_Recieved = TRUE;
	
					Write_b_eep( EEPROM_STATE_ADDRESS , START);
					Busy_eep( );
	
					app.state = START;
					break;
				}
				else if( (LinearKeyPad_getKeyState(START_PB) == FALSE) && (app.Input_Recieved == TRUE))
				{
					app.Input_Recieved = FALSE;				
				}

				if((LinearKeyPad_getKeyState(HOOTER_OFF_PB) == TRUE) && (app.hooter_reset == TRUE))
				{
					HOOTER = RESET;
					Write_b_eep( EEPROM_STATE_ADDRESS + 1 , HOOTER);
					Busy_eep( );
				}

				//Read RTC data and store it in buffer
				ReadRtcTimeAndDate(readTimeDateBuffer);  
				//Convert RTC data in to ASCII
				// Separate the higher and lower nibble and store it into the display buffer 
				APP_conversion(); 
				//Update digit display buffer with the current data of RTC
				DigitDisplay_updateBuffer(displayBuffer);

				//Store the current RTC data into EEPROM
				for(i = 0 ; i <  NO_OF_DIGITS  ; i++)
				{
					Write_b_eep( (EEPROM_RTC_ADDRESS + i) , displayBuffer[i]);
					Busy_eep( );
				}

				//Check RTC data with preset value and set HOOTER
				if(app.hooter_reset == FALSE)
				{
					for(i = 0 ; i < NO_OF_DIGITS ; i++)
					{
						if(targetBuffer[i] > '0')
							if(targetBuffer[i] != displayBuffer[i])
								app.hooter_set = TRUE;
					}
				}

				//If RTC data matches preset value set HOOTER and store HOOTER state
				if((app.hooter_set == FALSE) && (app.hooter_reset == FALSE))
				{
					HOOTER = SET;
					app.hooter_reset = TRUE;
					Write_b_eep( EEPROM_STATE_ADDRESS + 1 , HOOTER);
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

/*	//Convert BCD2HEX
	temp1 = ( (readTimeDateBuffer[2] & 0XF0) >> 4) * 10; 
	temp1 += (readTimeDateBuffer[2] & 0X0F);
	temp += temp1;

	//Convert HEX2BCD
	temp2 = HEX2BCD(temp);
	
*/
	if((readTimeDateBuffer[2] == 0x01) && (readTimeDateBuffer[1] == 0X39) &&
		(readTimeDateBuffer[0] == 0X59))
	{
		//Store current data into EEPROM

		//Change the state
		Write_b_eep( EEPROM_STATE_ADDRESS , START);
		Busy_eep( );

		app.state = START;	
	}
			
	displayBuffer[0] = (readTimeDateBuffer[0] & 0X0F) + '0';        //Seconds LSB
	displayBuffer[1] = ((readTimeDateBuffer[0] & 0XF0) >> 4) + '0'; //Seconds MSB
	displayBuffer[2] = (readTimeDateBuffer[1] & 0X0F) + '0';        //Minute LSB
	displayBuffer[3] = ((readTimeDateBuffer[1] & 0XF0) >> 4) + temp; //Minute MSB

}


void APP_resetDisplayBuffer(void)
{
	int i ;
	for(i = 0; i < 4; i++)			//reset all digits
	{
		displayBuffer[i] = '0';
	}
}

	
void APP_resetTargetBuffer(void)
{
	int i ;
	for(i = 0; i < 4; i++)			//reset all digits
	{
		targetBuffer[i] = '0';
	}
}



void APP_updateRTC(void)
{
	writeTimeDateBuffer[0] = ((displayBuffer[1] - '0') << 4) | (displayBuffer[0] - '0'); //store seconds
	writeTimeDateBuffer[1] = ((displayBuffer[3] - '0') << 4) | (displayBuffer[2] - '0'); //store minutes

#if defined (MODE_12HRS)
	//Set 6th bit of Hour register to enable 12 hours mode.
	writeTimeDateBuffer[2] |= 0x40;
#endif
	WriteRtcTimeAndDate(writeTimeDateBuffer);  //update RTC
}



	