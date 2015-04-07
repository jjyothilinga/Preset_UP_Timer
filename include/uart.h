
#ifndef UART_DRIVER_H
#define UART_DRIVER_H




/*
*------------------------------------------------------------------------------
* Include Files
*------------------------------------------------------------------------------
*/

#include "typedefs.h"
#include "config.h"


/*
*------------------------------------------------------------------------------
* Public Defines
*------------------------------------------------------------------------------
*/

enum RECEIVER_RESPONSE
{
	IDLE,
	STARTED,
	COMPLETED,
	ERROR
};


/*
*------------------------------------------------------------------------------
* Public Macros
*------------------------------------------------------------------------------
*/
// Enable this macro to enable BCC check in pc communication
//#define 	COMM_CHECK_BCC






/*
*------------------------------------------------------------------------------
* Public Data Types
*------------------------------------------------------------------------------
*/


// the uart structure used to provide access to uart data
typedef struct _UART
{
	UINT8 txBuffIndex;
	UINT8 txDataIndex;  
	INT8  txDataCount;
	UINT8 txBuff[TX_PACKET_SIZE];

	UINT8 rxBuffIndex;
	UINT8 rxDataIndex;
	INT8  rxDataCount;
	UINT8 rxBuff[RX_PACKET_SIZE];
}UART;


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
void UART_init( unsigned long baudRate );
extern void UartReceiveHandler(void);
extern void UartTransmitHandler(void);
extern void ResetReceiver(void);
extern void PutrsUART(const rom char *data);
extern void PutrsUARTRam(const UINT8 *data);
extern void PutcUARTRam(const UINT8 data);

extern BOOL UART_hasData(void);
extern UINT8 UART_read(void);
extern BOOL UART_write(UINT8 data);
extern BOOL UART_transmit(void);

#endif
/*
*  End of uart_driver.h
*/



