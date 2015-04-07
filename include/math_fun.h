#ifndef MATH_FUN_H
#define MATH_FUN_H

/*
*------------------------------------------------------------------------------
* math_fun.h
*
* Include file for math_fun functons
*
* (C)2009 Sam's Logic.
*
* The copyright notice above does not evidence any
* actual or intended publication of such source code.
*
*------------------------------------------------------------------------------
*/

/*
*------------------------------------------------------------------------------
* File				: math_fun.h
* Created by		: Sam
* Last changed by	: Sam
* Last changed		: 11/12/2010
*------------------------------------------------------------------------------
*
* Revision 1.2 11/12/2010  Sam
* Demo Release
* Revision 1.1 07/07/2010 Sam
* First Release
* Revision 1.0 20/04/2010 Sam
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
* Hardware Port Allocation
*------------------------------------------------------------------------------
*/

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

#define	LD_WORD(ptr)		(((UINT16)*(UINT8*)(ptr+1)<<8)|*(ptr))
#define	LD_DWORD(ptr)		(((UINT32)*(UINT8*)(ptr+3)<<24)|((UINT32)*(UINT8*)(ptr+2)<<16)|((UINT16)*(UINT8*)(ptr+1)<<8)|*(UINT8*)(ptr))
#define	ST_WORD(ptr,val)	*(UINT8*)(ptr)=val; *(UINT8*)(ptr+1)=val>>8
#define	ST_DWORD(ptr,val)	*(UINT8*)(ptr)=val; *(UINT8*)(ptr+1)=val>>8; *(UINT8*)(ptr+2)=val>>16; *(UINT8*)(ptr+3)=val>>24

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

extern UINT8 ConvertBCD2HEX(UINT16 bcd);
extern UINT16 BCD2HEX(UINT8 val);
extern UINT8 HEX2BCD(UINT8 hexIn);
extern void ConvertAndDisplay16BitValue(UINT16 inValue, UINT8 *pNewVal);
extern UINT8 BCDtoBin(UINT8 val);
extern UINT16 BinToBCD(UINT8 temp);
extern void ConvertAndDisplay8BitValue(UINT8 inValue, UINT8 *pNewVal);
extern UINT16 ConvertUnPackedAsciiToPacked16BitHexValue(UINT8 *inArray, UINT8 numOfDigits);
extern void ConvertAndDisplay32BitValue(UINT32 inValue, UINT8 *pNewVal);

#endif
/*
*  End of math_fun.h
*/