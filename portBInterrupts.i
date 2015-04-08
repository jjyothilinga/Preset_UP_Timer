#line 1 "source\portBInterrupts.c"
#line 1 "source\portBInterrupts.c"


#pragma udata interruptData
BOOL portB_intFlag;
UINT8 portB_currentData;
#pragma udata

void portBInterruptInit (void)
{
	OpenPORTB( PORTB_CHANGE_INT_ON & PORTB_PULLUPS_ON 
				& PORTB_INT_PRIO_HIGH ); 

	INTCONbits.RBIF = 0; 
	TRISB = 0XFF;		
	portB_intFlag = 0;
	portB_currentData = PORTB;
}

#pragma code 
#pragma interrupt portBInterruptHandler
void portBInterruptHandler (void)
{
	portB_intFlag = 1;
	portB_currentData = PORTB;		
	
	Delay10us(1); 		
	INTCONbits.RBIF = 0; 
}