#ifndef CONFIG_H
#define CONFIG_H

/*
*------------------------------------------------------------------------------
* config.h
*
*/


#define DEVICE_ADDRESS			0x01


#define __FACTORY_CONFIGURATION__


//TIMER Configuration
#define TIMESTAMP_DURATION 		(200)



//COM module configuration
#define __BCC_XOR__
//#define __RESPONSE_ENABLED__
//#define __LOOP_BACK__
#define BROADCAST_ADDRESS		0xFF
#define CMD_SOP	0x3A
#define CMD_EOP 0x0D
#define RESP_SOP	0xCC
#define RESP_EOP	0xDD
//#define __NO_CHECKSUM__

enum
{
	CMD_PACKET_SIZE = 30,
	RESP_PACKET_SIZE = 30
};

#define 	RX_PACKET_SIZE		(60)	
#define 	TX_PACKET_SIZE		(60)


#define __DISPLAY_TEST__
#define COMMON_CATHODE

//#define __SIMULATION__  // keypad stimulation

#define MAX_MSG 7


// Enable for external eeprom access
// Comment out this line if internal eeprom access required
#define EEP_EXTERRNAL
// For uart enable
//#define EN_UART
// Enable for hardware i2c module usage
//#define USE_MSSP_I2C


//RTC CONFIGURATION
//#define TIME_DEBUG

//#define RTC_DS1307

/*------------------------------------
 *	APP Configuration
 *-----------------------------------*/
#define EEPROM_STATE_ADDRESS 	(0x00)
#define EEPROM_HOOTER_ADDRESS 	(0x01)
#define EEPROM_PRESET_ADDRESS 	(0x10)
#define EEPROM_RTC_ADDRESS 		(0x20)
//#define RTC_DATA_ON_UART
//#define MODE_12HRS

/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* Public Variables (extern)
*------------------------------------------------------------------------------
*/

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

#endif
/*
*  End of config.h
*/



