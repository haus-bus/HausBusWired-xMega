/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: Electronics
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/Electronics/Electronics.cpp
*********************************************************************/

//## auto_generated
#include "Electronics.h"
//## dependency Flash
#include <Peripherals/Flash.h>
//## package Electronics


//## operation getFckE()
uint8_t getFckE() {
    //#[ operation getFckE()
    uint8_t fcke = Flash::readUserSignature( 1 );
    if( fcke == 0xFF )
    {         
      return FCKE_V2_0;
    }
    return fcke;
    //#]
}

/*********************************************************************
	File Path	: AR8/release/Electronics/Electronics.cpp
*********************************************************************/
