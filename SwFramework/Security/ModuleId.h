/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: ModuleId
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/Security/ModuleId.h
*********************************************************************/

#ifndef SwFramework_Security_ModuleId_H
#define SwFramework_Security_ModuleId_H

//## auto_generated
#include "Security/Security.h"
//## attribute size
#include "DefaultTypes.h"
//## package SwFramework::Security

//## class ModuleId
class ModuleId {
    ////    Additional operations    ////
    
public :

    //## auto_generated
    inline char getName(int i1) const {
        //#[ auto_generated
        return name[i1];
        //#]
    }
    
    //## auto_generated
    inline void setName(int i1, char p_name) {
        //#[ auto_generated
        name[i1] = p_name;
        //#]
    }
    
    //## auto_generated
    inline uint32_t getSize() const {
        //#[ auto_generated
        return size;
        //#]
    }
    
    //## auto_generated
    inline void setSize(uint32_t p_size) {
        //#[ auto_generated
        size = p_size;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getMajorRelease() const {
        //#[ auto_generated
        return majorRelease;
        //#]
    }
    
    //## auto_generated
    inline void setMajorRelease(uint8_t p_majorRelease) {
        //#[ auto_generated
        majorRelease = p_majorRelease;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getMinorRelease() const {
        //#[ auto_generated
        return minorRelease;
        //#]
    }
    
    //## auto_generated
    inline void setMinorRelease(uint8_t p_minorRelease) {
        //#[ auto_generated
        minorRelease = p_minorRelease;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getFirmwareId() const {
        //#[ auto_generated
        return firmwareId;
        //#]
    }
    
    //## auto_generated
    inline void setFirmwareId(uint8_t p_firmwareId) {
        //#[ auto_generated
        firmwareId = p_firmwareId;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getChecksum() const {
        //#[ auto_generated
        return checksum;
        //#]
    }
    
    //## auto_generated
    inline void setChecksum(uint8_t p_checksum) {
        //#[ auto_generated
        checksum = p_checksum;
        //#]
    }
    
    ////    Attributes    ////
    
    char name[16];		//## attribute name
    
    uint32_t size;		//## attribute size
    
    uint8_t majorRelease;		//## attribute majorRelease
    
    uint8_t minorRelease;		//## attribute minorRelease
    
    uint8_t firmwareId;		//## attribute firmwareId
    
    uint8_t checksum;		//## attribute checksum
};

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/Security/ModuleId.h
*********************************************************************/
