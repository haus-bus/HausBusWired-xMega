/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: StorageInterface
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/StorageInterface.h
*********************************************************************/

#ifndef SwFramework_StorageInterface_H
#define SwFramework_StorageInterface_H

//## auto_generated
#include "SwFramework.h"
//## operation read(uint16_t,void *,uint16_t)
#include "DefaultTypes.h"
//## operation read(uint16_t,void *,uint16_t)
#include "Stream.h"
//## package SwFramework

//## class StorageInterface
class StorageInterface {
    ////    Operations    ////
    
public :

    //## operation read(uint16_t,void *,uint16_t)
    virtual Stream::Status read(uintptr_t offset, void * pData, uint16_t length) = 0;
    
    //## operation write(uint16_t,void *,uint16_t)
    virtual Stream::Status write(uintptr_t offset, void * pData, uint16_t length) = 0;
};

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/StorageInterface.h
*********************************************************************/
