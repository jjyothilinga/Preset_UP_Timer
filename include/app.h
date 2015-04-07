#include "board.h"
#include "typedefs.h"
#include "digitdisplay.h"
#include "rtc_driver.h"


typedef enum 
{
	START = 0,
	STOP,
	SETTING
}APP_STATE;


typedef enum
{
	MODE_CHANGE_INPUT_MASK = 0X80,
	NEXT_INPUT_MASK = 0X40,
	SET_INPUT_MASK = 0X20
}MASK_INTR_DATA;


typedef enum
{
	INCREMENT = 0X03,
	DIGIT_INDEX = 0X02
	
}CLOCK_SETTING_PBS;


typedef enum
{
	MINUTES_LSB = 0,
	MINUTES_MSB,
	HOURS_LSB,
	HOURS_MSB,
	MINUTES_LSB_MAX = '9',
	MINUTES_MSB_MAX = '5',
	HOURS_LSB_MAX = '9',
	HOURS_MSB_MAX = '1'

}TIME_DIGITS;

extern UINT8 APP_comCallBack( far UINT8 *rxPacket, far UINT8* txCode,far UINT8** txPacket);
extern void APP_init(void);
extern void APP_task(void);
