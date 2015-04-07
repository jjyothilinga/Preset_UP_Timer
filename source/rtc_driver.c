
/*
*------------------------------------------------------------------------------
* rtc_driver.c
*
* rtc driver module.
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
* File				: rtc_driver.c
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 11/12/2010
*------------------------------------------------------------------------------
*
* Revision 1.2 11/12/2010  Sam
* Demo Release
* Revision 1.1 07/07/2010 Sam
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
#include <p18f4520.h>
#include "board.h"
#include "config.h"
#include "typedefs.h"
//#include "schedular.h"
#include "i2c_driver.h"
#include "rtc_driver.h"
#include "math_fun.h"

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

#define DEV_ADDR_RTC			(0xD0)
#define CMD_WRITE_RTC			(0xD0)
#define CMD_READ_RTC			(0xD1)

#define RTC_REG_SEC				(0x00)
#define RTC_REG_MIN				(0x01)
#define RTC_REG_HOR				(0x02)
#define RTC_REG_DAY				(0x03)
#define RTC_REG_DATE			(0x04)
#define RTC_REG_MONT			(0x05)
#define RTC_REG_YEAR			(0x06)
#define RTC_REG_CTR				(0x07)
#define RTC_EEPROM				(0X08)


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

UINT8 RegistersRTC[7];  // Buffer for second,minute,.....,year
BOOL TimeClockUpdateRequired = FALSE;
DATE_TIME stRtcRegs;
BOOL TimeClockMode;		//24 hr / 12 hr mode


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
const rom UINT8 * DayStr[7]  =  {{"Sun"},
				   				{"Mon"},
				   				{"Tue"},
				   				{"Wed"},
				   				{"Thu"},
				   				{"Fri"},
				   				{"Sat"}};


const rom UINT8 * MonthStr[12] ={{"Jan"},
								{"Feb"},
								{"Mar"},
								{"Apr"},
								{"May"},
								{"Jun"},
								{"Jul"},
								{"Aug"},
								{"Sep"},
								{"Oct"},
								{"Nov"},
								{"Dec"}};

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
* INT8 * Int2Day(UINT8 day)
*
* Summary	: Convert date (BCD) to string of Day
*
* Input		: UINT8 day - day in bcd number form
*
* Output	: INT8* - pointer to the converted string form of Day
*
* Note		: 1=Sanday, 2=Monday and so on..
*------------------------------------------------------------------------------
*/
const rom INT8* Int2Day(UINT8 day)
{
	return (const rom INT8*)DayStr[day-1];
}

/*
*------------------------------------------------------------------------------
* INT8* Int2Month(UINT8 month)
*
* Summary	: Convert month (BCD) to string of Month
*
* Input		: UINT8 month - month in bcd number form
*
* Output	: INT8* - pointer to the converted string form of month
*
* Note		: 0x01=January, 0x02=February, ..0x12 = December
*------------------------------------------------------------------------------
*/
const rom INT8* Int2Month(UINT8 month)
{
	return (const rom INT8*)MonthStr[ConvertBCD2HEX(month)-1];
}



/*
*------------------------------------------------------------------------------
* void ReadRtcTimeAndDate(UINT8 * buff)
*
* Summary	: Read time and date related registers from RTC
*
* Input		: UINT8 *buff - buffer pointer to receive the data
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void ReadRtcTimeAndDate(UINT8 * buff)
{
	StartI2C();
	WriteI2C(DEV_ADDR_RTC);
	WriteI2C(RTC_REG_SEC);
	StartI2C();
	WriteI2C(DEV_ADDR_RTC + 1);
	*(buff+0)=ReadI2C(ACK);		// Second
	*(buff+1)=ReadI2C(ACK);		// Minute
	*(buff+2)=ReadI2C(ACK);		// hour
	*(buff+3)=ReadI2C(ACK);		// Day
	*(buff+4)=ReadI2C(ACK);		// date
	*(buff+5)=ReadI2C(ACK);		// month
	*(buff+6)=ReadI2C(NO_ACK);	// year
	StopI2C();
}

/*
*------------------------------------------------------------------------------
* void WriteRtcTimeAndDate(UINT8 *buff)
*
* Summary	: Write time and date related values into RTC
*
* Input		: UINT8 *buff - pointer to the buffer having data to be write.
*
* Output	: None
*
*------------------------------------------------------------------------------
*/

void WriteRtcTimeAndDate(UINT8 *buff)
{
	StartI2C();
	WriteI2C(DEV_ADDR_RTC);
	WriteI2C(RTC_REG_SEC);
	WriteI2C(*(buff+0));
	WriteI2C(*(buff+1));
	WriteI2C(*(buff+2));
	WriteI2C(*(buff+3));
	WriteI2C(*(buff+4));
	WriteI2C(*(buff+5));
	WriteI2C(*(buff+6));
	StopI2C();
}

/*
*------------------------------------------------------------------------------
* void SetHourMode(UINT8 hour, BOOL mode)
*
* Summary	: Set the rtc hour value depending on the hour mode(12/24)
*
* Input		: UINT8 hour - value to set hour field
*			  BOOL mode - 12 /24 hour mode
*			  BOOL amPm - am/pm if bit , if given hour mode is 12 hour
*
* Output	: UINT8 - hour with mode set
*
*------------------------------------------------------------------------------
*/
UINT8 SetHourMode(UINT8 hour, BOOL mode,BOOL amPm)
{
	if(CLOCK_MODE_12HR == mode)
	{
		hour = CLOCK_SET_12HR_MODE(hour);
		if(CLOCK_HOUR_PM == amPm)
		{
			hour = CLOCK_SET_PM(hour);
		}
		else
		if(CLOCK_HOUR_AM == amPm)
		{
			hour = CLOCK_SET_AM(hour);
		}
	}
	else
	if(CLOCK_MODE_24HR == mode)
	{
		hour = CLOCK_SET_24HR_MODE(hour);
	}
	
	
	return hour;
}

/*
*------------------------------------------------------------------------------
* void ConvertTime24to12Format(DATE_TIME *pstRtcData24,DATE_TIME *pstRtcData12)
*
* Summary	: Convert 24 hr format time to 12 hr format
*
* Input		: DATE_TIME *pstRtcData24 - pointer to 24 hr formated data
*			  DATE_TIME *pstRtcData12 - pointer to get converted 12 hr formated data
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void ConvertTime24to12Format(DATE_TIME *pstRtcData24,DATE_TIME *pstRtcData12)
{
	pstRtcData12->mMinute	= pstRtcData24->mMinute;
	pstRtcData12->mSeconds	= pstRtcData24->mSeconds;
	if(pstRtcData24->mHour >= 12)
	{
		pstRtcData12->mHour 	= pstRtcData24->mHour - 12;
		pstRtcData12->mAmPm     = CLOCK_HOUR_PM;
		if(12 == pstRtcData24->mHour)
		{
			pstRtcData12->mHour 	= 12;
		}
	}
	else
	if(pstRtcData24->mHour < 12)
	{
		pstRtcData12->mHour 	= pstRtcData24->mHour;
		pstRtcData12->mAmPm     = CLOCK_HOUR_AM;
		if(0 == pstRtcData24->mHour)
		{
			pstRtcData12->mHour 	= 12;
		}
	}
/*
		PutcUARTRam('t');
		PutcUARTRam(':');
		PutcUARTRam(((BinToBCD(pstRtcData12->mHour) & 0xF0)>>4) | 0x30);
		PutcUARTRam((BinToBCD(pstRtcData12->mHour) & 0x0F) | 0x30);
		PutcUARTRam(':');
		PutcUARTRam(((BinToBCD(pstRtcData12->mMinute) & 0xF0)>>4) | 0x30);
		PutcUARTRam((BinToBCD(pstRtcData12->mMinute) & 0x0F) | 0x30);
		PutcUARTRam(':');
		PutcUARTRam(((BinToBCD(pstRtcData12->mSeconds) & 0xF0)>>4) | 0x30);
		PutcUARTRam((BinToBCD(pstRtcData12->mSeconds) & 0x0F) | 0x30);
		if(	CLOCK_HOUR_PM == pstRtcData12->mAmPm )
		{
			PutcUARTRam('P');
			PutcUARTRam('M');
		}
		else
		if(	CLOCK_HOUR_AM == pstRtcData12->mAmPm )
		{
			PutcUARTRam('A');
			PutcUARTRam('M');
		}
*/
}

/*
*------------------------------------------------------------------------------
* void ConvertTime12to24Format(DATE_TIME *pstRtcData12,DATE_TIME *pstRtcData24)
*
* Summary	: Convert 12 hr format time to 24 hr format
*
* Input		: DATE_TIME *pstRtcData24 - pointer to 24 hr formated data
*			  DATE_TIME *pstRtcData12 - pointer to get converted 12 hr formated data
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void ConvertTime12to24Format(DATE_TIME *pstRtcData12,DATE_TIME *pstRtcData24)
{
	pstRtcData24->mMinute	= pstRtcData12->mMinute;
	pstRtcData24->mSeconds	= pstRtcData12->mSeconds;

	if(CLOCK_HOUR_PM == pstRtcData12->mAmPm)
	{
		pstRtcData24->mHour 	= pstRtcData12->mHour + 12;
		if(12 == pstRtcData12->mHour)
		{
			pstRtcData24->mHour 	= 12;
		}
	}
	else

	if(CLOCK_HOUR_AM == pstRtcData12->mAmPm)
	{
		pstRtcData24->mHour 	= pstRtcData12->mHour;
		if(12 == pstRtcData12->mHour)
		{
			pstRtcData24->mHour 	= 0;
		}
	}
/*
		PutcUARTRam('m');
		PutcUARTRam(':');
		PutcUARTRam(((BinToBCD(pstRtcData24->mHour) & 0xF0)>>4) | 0x30);
		PutcUARTRam((BinToBCD(pstRtcData24->mHour) & 0x0F) | 0x30);
		PutcUARTRam(':');
		PutcUARTRam(((BinToBCD(pstRtcData24->mMinute) & 0xF0)>>4) | 0x30);
		PutcUARTRam((BinToBCD(pstRtcData24->mMinute) & 0x0F) | 0x30);
		PutcUARTRam(':');
		PutcUARTRam(((BinToBCD(pstRtcData24->mSeconds) & 0xF0)>>4) | 0x30);
		PutcUARTRam((BinToBCD(pstRtcData24->mSeconds) & 0x0F) | 0x30);
*/
}

/*
*------------------------------------------------------------------------------
* UINT8 ConvertHour12to24Format(UINT8 hour12,UINT8 amPm)
*
* Summary	: Convert 12 hr format hour to 24 hr format
*
* Input		: UINT8 hour12 - hour in 12hr format
*			  UINT8 amPm - hour in 24hr format
*
* Output	: None
*
*------------------------------------------------------------------------------
*/

UINT8 ConvertHour12to24Format(UINT8 hour12,UINT8 amPm)
{
	UINT8 hour24 = 0;
	if(CLOCK_HOUR_PM == amPm)
	{
		hour24 	= hour12 + 12;
		if(12 == hour12)
		{
			hour24 	= 12;
		}
	}
	else
	if(CLOCK_HOUR_AM == amPm)
	{
		hour24 	= hour12;
		if(12 == hour12)
		{
			hour24 	= 0;
		}
	}
	else
	if(0xAA == amPm)
	{
		hour24 = 0xAA;
	}
	return hour24;
}

/*
*------------------------------------------------------------------------------
* void StoreSystemTime(void)
*
* Summary	: Set new RTC time
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void StoreSystemTime(UINT8 *databuffer)
{
	StartI2C();
	WriteI2C(DEV_ADDR_RTC);
	WriteI2C(RTC_REG_SEC);
	WriteI2C(((*(databuffer + 4) & 0x0F)<<4) | (*(databuffer + 5) & 0x0F)); //sec
	WriteI2C(((*(databuffer + 2) & 0x0F)<<4) | (*(databuffer + 3) & 0x0F)); //min
	WriteI2C(((*(databuffer + 0) & 0x0F)<<4) | (*(databuffer + 1) & 0x0F)); //hour
	StopI2C();
}

/*
*------------------------------------------------------------------------------
* void StoreSystemDate(void)
*
* Summary	: Set new RTC date
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void StoreSystemDate(UINT8 *databuffer)
{
	StartI2C();
	WriteI2C(DEV_ADDR_RTC);
	WriteI2C(RTC_REG_DATE);
	WriteI2C(((*(databuffer + 0) & 0x0F)<<4) | (*(databuffer + 1) & 0x0F)); //date
	WriteI2C(((*(databuffer + 2) & 0x0F)<<4) | (*(databuffer + 3) & 0x0F)); //month
	WriteI2C(((*(databuffer + 4) & 0x0F)<<4) | (*(databuffer + 5) & 0x0F)); //year
	StopI2C();
}

/*
*------------------------------------------------------------------------------
* void StoreSystemDay(UINT8 day)
*
* Summary	: Set new day
*
* Input		: UINT8 day - value related to day
*
* Output	: None
*
* Nonte		: Day value must be between 1 to 7
*------------------------------------------------------------------------------
*/
void StoreSystemDay(UINT8 day)
{
	StartI2C();
	WriteI2C(DEV_ADDR_RTC);
	WriteI2C(RTC_REG_DAY);
	WriteI2C(day); //day
	StopI2C();
}

/*
*------------------------------------------------------------------------------
* void InitializeRtc(void)
*
* Summary	: Initialize the I2C module and RTC
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/
void InitializeRtc(void)
{
	UINT8 tempVar;
	// Read seconds register contents
	tempVar = ReadByteI2C(DEV_ADDR_RTC,RTC_REG_SEC);
	// Enable oscillator (bit 7=0)
	tempVar = tempVar & 0x7F;
	WriteByteI2C(DEV_ADDR_RTC,RTC_REG_SEC,tempVar);

	// set RTC control register to o/p 1Hz sqr wave o/p
	WriteByteI2C(DEV_ADDR_RTC,RTC_REG_CTR,0x90);
/*
	ReadRtcTimeAndDate((UINT8*)&stRtcRegs);
	stRtcRegs.mSeconds	= 0x00;							// second = 00
	stRtcRegs.mMinute  	= 0x29;							// minute = 29
	stRtcRegs.mHour  	= 0x21;							// hour = 21 ,24-hour mode(bit 6=0)
	stRtcRegs.mDay		= 0x01;							// Day = 1 or sunday
	stRtcRegs.mDate		= 0x12;							// Date = 12
	stRtcRegs.mMonth	= 0x10;							// month = October
	stRtcRegs.mYear		= 0x08;							// year = 08 or 2008
	WriteRtcTimeAndDate((UINT8*)&stRtcRegs);			// Set RTC
*/
#ifdef TIME_DEBUG
	stRtcRegs.mSeconds	= 0;							// second = 00
	stRtcRegs.mMinute  	= 50;							// minute = 29
	stRtcRegs.mHour  	= 6;
#endif
	// Setup task to run
#ifdef TIME_DEBUG
	SCH_AddTask(UpdateRealTimeClockTask ,0,TASK_10MSEC_PERIOD);
#else
//	SCH_AddTask(UpdateRealTimeClockTask ,0,TASK_100MSEC_PERIOD);
#endif
}

/*
*------------------------------------------------------------------------------
* void UpdateRealTimeClockTask(void)
*
* Summary	:
*
* Input		: None
*
* Output	: None
*
*------------------------------------------------------------------------------
*/

void UpdateRealTimeClockTask(void)
{
	static UINT8 prevSec = 0;

#ifdef TIME_DEBUG
//	if (++stRtcRegs.mSeconds == 60)
	{
		stRtcRegs.mSeconds = 0;
		if (++stRtcRegs.mMinute == 60)
		{
			stRtcRegs.mMinute = 0;
			if(++stRtcRegs.mHour == 24)
			{
				stRtcRegs.mHour = 0;
			}
		}
	}
	if(stRtcRegs.mMinute != prevSec)
	{
		prevSec = stRtcRegs.mMinute;
		TimeClockUpdateRequired = TRUE;
	}
#else
	ReadRtcTimeAndDate((UINT8*)&stRtcRegs);
	if(stRtcRegs.mSeconds != prevSec)
	{
		prevSec = stRtcRegs.mSeconds;
		TimeClockUpdateRequired = TRUE;
	}
#endif
}





/*
*------------------------------------------------------------------------------
* Private Functions
*------------------------------------------------------------------------------
*/

/*
*  End of rtc_driver.c
*/