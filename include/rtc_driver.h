
#ifndef RTC_DRIVER_H
#define RTC_DRIVER_H


/*
*------------------------------------------------------------------------------
* rtc_driver.h
*
* Include file for rtc_driver module.
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
* File				: rtc_driver.h
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

#include "typedefs.h"

/*
*------------------------------------------------------------------------------
* Public Defines
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/


#define BCD2ASCII(bcdIn,pascii)			*(pascii + 0) = (((bcdIn & 0xF0)>>4)| 0x30);\
										*(pascii + 1) = ((bcdIn & 0x0F)| 0x30);

// Only it can convert upto 63H - 99D
#define HEX2ASCII(hexIn,pascii)			*(pascii + 0) = ((hexIn / 10) | 0x30);\
										*(pascii + 1) = ((hexIn % 10) | 0x30);


#define CLOCK_MODE_24HR					(BOOL)0
#define CLOCK_MODE_12HR					(BOOL)1

#define CLOCK_HOUR_AM					(BOOL)0
#define CLOCK_HOUR_PM					(BOOL)1

#define CLOCK_12HR_MASK					(UINT8)0x1F
#define CLOCK_12_24_HR_MASK_BIT			(UINT8)0x40
#define CLOCK_AM_PM_MASK_BIT			(UINT8)0x20


#define CLOCK_SET_PM(hourBuf)			(hourBuf | 0x20);	// Make D5 bit 1 for PM
#define CLOCK_SET_AM(hourBuf)			(hourBuf & 0xDF);	// Make D5 bit 0 for AM

#define CLOCK_SET_12HR_MODE(hourBuf) 	(hourBuf | 0x40)	// Make D6 bit 1 for 12hr
#define CLOCK_SET_24HR_MODE(hourBuf)	(hourBuf & 0xBF)	// Make D6 bit 0 for 24hr


/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

// Time date structure
typedef struct _DATE_TIME
{
	UINT8 mSeconds;
	UINT8 mMinute;
	UINT8 mHour;
	UINT8 mDay;
	UINT8 mDate;
	UINT8 mMonth;
	UINT8 mYear;
	UINT8 mAmPm;
}DATE_TIME;


/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/
extern UINT8 RegistersRTC[7];
extern BOOL TimeClockUpdateRequired;
extern DATE_TIME stRtcRegs;
extern BOOL TimeClockMode;
/*
*------------------------------------------------------------------------------
* Public Constants (extern)
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Function Prototypes (extern)
*------------------------------------------------------------------------------
*/
extern void InitializeRtc(void);
extern void ReadRtcTimeAndDate(UINT8 * buff);
extern void WriteRtcTimeAndDate(UINT8 *buff);
extern void UpdateRealTimeClockTask(void);
extern void StoreSystemTime(UINT8 *databuffer);
extern void StoreSystemDate(UINT8 *databuffer);
extern void StoreSystemDay(UINT8 day);
extern const rom INT8* Int2Day(UINT8 day);
extern const rom INT8* Int2Month(UINT8 month);
extern UINT8 HEX2BCD(UINT8 hexIn);
extern UINT8 SetHourMode(UINT8 hour, BOOL mode,BOOL amPm);
extern void ConvertTime24to12Format(DATE_TIME *pstRtcData24,DATE_TIME *pstRtcData12);
extern void ConvertTime12to24Format(DATE_TIME *pstRtcData12,DATE_TIME *pstRtcData24);
extern UINT8 ConvertHour12to24Format(UINT8 hour12,UINT8 amPm);
extern void ReadRtcEEPROM(UINT8 * buff);
extern void WriteRtcEEPROM(UINT8 *buff);
#endif
/*
*  End of rtc_driver.h
*/