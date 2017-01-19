/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: Checksum16
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/Security/Checksum16.h
*********************************************************************/

#ifndef SwFramework_Security_Checksum16_H
#define SwFramework_Security_Checksum16_H

//## auto_generated
#include "Security/Security.h"
//## operation get(void *,uint16_t,uint32_t)
#include "DefaultTypes.h"
//## package SwFramework::Security

//## class Checksum16
class Checksum16 {
public :

    //## type Constants
    enum Constants {
        StartValue = 0
    };
    
    ////    Operations    ////
    
    //## operation get(void *,uint16_t,uint32_t)
    static uint16_t get(void * pData, uint16_t length, uint32_t checksum = StartValue);
    
    //## operation hasError(void *,uint16_t)
    inline static uint16_t hasError(void * pData, uint16_t length) {
        //#[ operation hasError(void *,uint16_t)
        return get(pData, length)+1;
        //#]
    }
};

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/Security/Checksum16.h
*********************************************************************/
