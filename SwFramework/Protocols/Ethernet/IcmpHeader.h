/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: IcmpHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/IcmpHeader.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_IcmpHeader_H
#define SwFramework_Protocols_Ethernet_IcmpHeader_H

//## auto_generated
#include "Ethernet.h"
//## class IcmpHeader
#include "IpHeader.h"
//## package SwFramework::Protocols::Ethernet

//## class IcmpHeader
class IcmpHeader : public IpHeader {
public :

    //#[ type Types
    static const uint8_t ECHO_REPLY = 0;
    static const uint8_t ECHO       = 8;
    //#]
    
    ////    Operations    ////
    
    //## operation getChecksum() const
    inline uint16_t getChecksum() const {
        //#[ operation getChecksum() const
        return changeEndianness( checksum );
        //#]
    }
    
    //## operation isEchoRequest() const
    inline bool isEchoRequest() const;
    
    //## operation setChecksum(uint16_t)
    inline void setChecksum(uint16_t p_checksum) {
        //#[ operation setChecksum(uint16_t)
        checksum = changeEndianness( p_checksum );
        //#]
    }
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getType() const {
        //#[ auto_generated
        return type;
        //#]
    }
    
    //## auto_generated
    inline void setType(uint8_t p_type) {
        //#[ auto_generated
        type = p_type;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getICode() const {
        //#[ auto_generated
        return iCode;
        //#]
    }
    
    //## auto_generated
    inline void setICode(uint8_t p_iCode) {
        //#[ auto_generated
        iCode = p_iCode;
        //#]
    }
    
    //## auto_generated
    inline uint16_t getId() const {
        //#[ auto_generated
        return id;
        //#]
    }
    
    //## auto_generated
    inline void setId(uint16_t p_id) {
        //#[ auto_generated
        id = p_id;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getSeqNumber(int i1) const {
        //#[ auto_generated
        return seqNumber[i1];
        //#]
    }
    
    //## auto_generated
    inline void setSeqNumber(int i1, uint8_t p_seqNumber) {
        //#[ auto_generated
        seqNumber[i1] = p_seqNumber;
        //#]
    }
    
    ////    Attributes    ////
    
    uint8_t type;		//## attribute type
    
    uint8_t iCode;		//## attribute iCode
    
    uint16_t checksum;		//## attribute checksum
    
    uint16_t id;		//## attribute id
    
    uint8_t seqNumber[4];		//## attribute seqNumber
};

inline bool IcmpHeader::isEchoRequest() const {
    //#[ operation isEchoRequest() const
    return ( type == ECHO );
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/IcmpHeader.h
*********************************************************************/
