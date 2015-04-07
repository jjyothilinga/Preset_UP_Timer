#include "app.h" 
#include "uart.h"
#include "string.h"
#include "linearkeypad.h"
#include "eep.h"


#define NO_OF_DIGITS	(0X06)


/*
*------------------------------------------------------------------------------
* Public Variables
* Buffer[0] = seconds, Buffer[1] = minutes, Buffer[2] = Hour,
* Buffer[3] = day, Buffer[4] = date, Buffer[5] = month, Buffer[6] = year
*------------------------------------------------------------------------------
*/
UINT8 txBuffer[6] = {0};
UINT8 displayBuffer[6] = {0};
UINT8 targetBuffer[6]  = {0};
UINT8 max[6] = {0x39,0x35,0x39,0x35,0x39,0x39};
UINT8 readTimeDateBuffer[6] = {0};
UINT8 writeTimeDateBuffer[] = {0X50, 0X59, 0X72, 0X03, 0x027, 0X12, 0X13};
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
	UINT8 hooter;

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
//	WriteRtcTimeAndDate(writeTimeDateBuffer);

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

			if ((LinearKeyPad_getKeyState(0) == TRUE)&& (app.Input_Recieved == 0))
			{
				APP_resetDisplayBuffer();
				DigitDisplay_updateBuffer(displayBuffer);
				APP_updateRTC();
				
				app.Input_Recieved = 1;

				Write_b_eep( EEPROM_STATE_ADDRESS , STOP);
				Busy_eep( );
		
				app.state = STOP;
				
			}
			else if ((LinearKeyPad_getKeyState(1) == TRUE)&& (app.setPBpressed == 0))
			{
				APP_resetTargetBuffer();
				app.blinkIndex = 0;
				DigitDisplay_updateBuffer(targetBuffer);
				DigitDisplay_blinkOn_ind(500, app.blinkIndex);

				app.setPBpressed = 1;

				app.state = SETTING;
			
			}
			

			if(LinearKeyPad_getKeyState(0) == FALSE)
			{
				app.Input_Recieved = 0;
			
			}

			if ((LinearKeyPad_getKeyState(1) == 0)&& (app.setPBpressed == 1))
				app.setPBpressed = 0;

			break;

			case SETTING:

					CLOCK_LED = 0;

		//	DigitDisplay_blinkOn_ind(500, app.blinkIndex);

			// CODE TO HANDLE DIGIT INDEX PB
			if ((LinearKeyPad_getKeyState(DIGIT_INDEX ) == 1) && (app.digitIndexPBpressed == FALSE ))
			{
				app.blinkIndex++;

				if(app.blinkIndex > (NO_OF_DIGITS - 1))
					app.blinkIndex = 0 ;

				DigitDisplay_blinkOn_ind(500, app.blinkIndex);

				app.digitIndexPBpressed = TRUE;
			}
			else if ((LinearKeyPad_getKeyState(DIGIT_INDEX ) == 0) && (app.digitIndexPBpressed == TRUE ))
				app.digitIndexPBpressed = FALSE;

			// CODE TO HANDLE INCREMENT PB
			if ((LinearKeyPad_getKeyState(INCREMENT) == 1) && (app.incrementPBpressed == FALSE ))
			{
				targetBuffer[app.blinkIndex]++;
				if(targetBuffer[app.blinkIndex] > max[app.blinkIndex])
					targetBuffer[app.blinkIndex] = '0';			
						
				DigitDisplay_updateBuffer(targetBuffer);

				app.incrementPBpressed = TRUE;

			}
			else if ((LinearKeyPad_getKeyState(INCREMENT) == 0) && (app.incrementPBpressed == TRUE ))
				app.incrementPBpressed = FALSE;

			if ((LinearKeyPad_getKeyState(1) == TRUE)&& (app.setPBpressed == 0))
			{
				DigitDisplay_blinkOff();
				DigitDisplay_updateBuffer(targetBuffer);
				CLOCK_LED = 1;
				app.setPBpressed = 1;

				for(i = 0; i < NO_OF_DIGITS ; i++ )
				{
					Write_b_eep( EEPROM_TARGET_ADDRESS + i ,targetBuffer[i] );
					Busy_eep( );
				}
				Write_b_eep( EEPROM_STATE_ADDRESS , START);
				Busy_eep( );

				app.state = START;	
				
			}
			else if ((LinearKeyPad_getKeyState(1) == 0)&& (app.setPBpressed == 1))
				app.setPBpressed = 0;
				
			break;
			
			case STOP:
				app.hooter = 0;
		
				if((LinearKeyPad_getKeyState(0) == TRUE) && (app.Input_Recieved == 0))
				{
					app.Input_Recieved = 1;
	
					Write_b_eep( EEPROM_STATE_ADDRESS , START);
					Busy_eep( );
	
					app.state = START;
					break;
				}
				else if( (LinearKeyPad_getKeyState(0) == FALSE) && (app.Input_Recieved == 1))
				{
					app.Input_Recieved = 0;
				
				}


				ReadRtcTimeAndDate(readTimeDateBuffer);  //Read the data from RTC
				APP_conversion(); // Separate the higher and lower nibble and store it into the display buffer 
	
				DigitDisplay_updateBuffer(displayBuffer);

				for(i = 0 ; i <  NO_OF_DIGITS  ; i++)
				{
					Write_b_eep( (EEPROM_RTC_ADDRESS + i) , displayBuffer[i]);
					Busy_eep( );
				}


				for(i = 0 ; i < NO_OF_DIGITS ; i++)
				{
					if(targetBuffer[i] > '0')
						if(targetBuffer[i] != displayBuffer[i])
							app.hooter = 1;

				}


				if(app.hooter == 0)
				{
					HOOTER = 1;
					Write_b_eep( EEPROM_STATE_ADDRESS + 1 , HOOTER);
					Busy_eep( );
					Write_b_eep( EEPROM_STATE_ADDRESS , START);
					Busy_eep( );
					app.state = START;
				}
				else
				{ 
					HOOTER = 0;
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

	//Store the day & multiply with the hours
	temp = readTimeDateBuffer[3] - 1;
	temp *= 24;

	//Convert BCD2HEX
	temp1 = ( (readTimeDateBuffer[2] & 0XF0) >> 4) * 10; 
	temp1 += (readTimeDateBuffer[2] & 0X0F);
	temp += temp1;

	//Convert HEX2BCD
	temp2 = HEX2BCD(temp);
	

	if((temp == 0X99) && (readTimeDateBuffer[1] == 0X59) &&
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
	displayBuffer[3] = ((readTimeDateBuffer[1] & 0XF0) >> 4) + '0'; //Minute MSB

	displayBuffer[4] = (temp2 & 0X0F) + '0';		//Hour LSB
	displayBuffer[5] = ((temp2 & 0XF0) >> 4) + '0'; //Hour MSB 
}


void APP_resetDisplayBuffer(void)
{
	int i ;
	for(i = 0; i < 6; i++)			//reset all digits
	{
		displayBuffer[i] = '0';
	}
}

	
void APP_resetTargetBuffer(void)
{
	int i ;
	for(i = 0; i < 6; i++)			//reset all digits
	{
		targetBuffer[i] = '0';
	}
}



void APP_updateRTC(void)
{
	writeTimeDateBuffer[0] = ((displayBuffer[1] - '0') << 4) | (displayBuffer[0] - '0'); //store seconds
	writeTimeDateBuffer[1] = ((displayBuffer[3] - '0') << 4) | (displayBuffer[2] - '0'); //store minutes
	writeTimeDateBuffer[2] = ((displayBuffer[5] - '0') << 4) | (displayBuffer[4] - '0'); //store Hours

#if defined (MODE_12HRS)
	//Set 6th bit of Hour register to enable 12 hours mode.
	writeTimeDateBuffer[2] |= 0x40;
#endif
	WriteRtcTimeAndDate(writeTimeDateBuffer);  //update RTC
}



	