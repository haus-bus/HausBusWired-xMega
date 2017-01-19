/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: ArpHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/ArpHeader.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_ArpHeader_H
#define SwFramework_Protocols_Ethernet_ArpHeader_H

//## auto_generated
#include "Ethernet.h"
//## class ArpHeader
#include "EthernetHeader.h"
//## attribute sourceIpAddress
#include "IP.h"
//## attribute sourceHwAddress
#include "MAC.h"
//## package SwFramework::Protocols::Ethernet

//## class ArpHeader
class ArpHeader : public EthernetHeader {
public :

    //## type Opcodes
    enum Opcodes {
        REQUEST = 1,
        REPLY = 2
    };
    
    //## type HwTypes
    enum HwTypes {
        HWTYPE_ETH = 1
    };
    
    ////    Operations    ////
    
    //## operation getHwType() const
    inline uint16_t getHwType() const {
        //#[ operation getHwType() const
        return changeEndianness( hwType );
        //#]
    }
    
    //## operation getOpcode() const
    inline uint16_t getOpcode() const {
        //#[ operation getOpcode() const
        return changeEndianness( opcode );
        //#]
    }
    
    //## operation getProtocol() const
    inline uint16_t getProtocol() const {
        //#[ operation getProtocol() const
        return changeEndianness( protocol );
        //#]
    }
    
    //## operation setHwType(uint16_t)
    inline void setHwType(uint16_t p_hwType) {
        //#[ operation setHwType(uint16_t)
        hwType = changeEndianness( p_hwType );
        //#]
    }
    
    //## operation setOpcode(uint16_t)
    inline void setOpcode(uint16_t p_opcode) {
        //#[ operation setOpcode(uint16_t)
        opcode = changeEndianness( p_opcode );
        //#]
    }
    
    //## operation setProtocol(uint16_t)
    inline void setProtocol(uint16_t p_protocol) {
        //#[ operation setProtocol(uint16_t)
        protocol = changeEndianness( p_protocol );
        //#]
    }
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getHwLength() const {
        //#[ auto_generated
        return hwLength;
        //#]
    }
    
    //## auto_generated
    inline void setHwLength(uint8_t p_hwLength) {
        //#[ auto_generated
        hwLength = p_hwLength;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getProtocolLength() const {
        //#[ auto_generated
        return protocolLength;
        //#]
    }
    
    //## auto_generated
    inline void setProtocolLength(uint8_t p_protocolLength) {
        //#[ auto_generated
        protocolLength = p_protocolLength;
        //#]
    }
    
    //## auto_generated
    inline MAC* getSourceHwAddress() const {
        //#[ auto_generated
        return (MAC*) &sourceHwAddress;
        //#]
    }
    
    //## auto_generated
    inline IP* getSourceIpAddress() const {
        //#[ auto_generated
        return (IP*) &sourceIpAddress;
        //#]
    }
    
    //## auto_generated
    inline MAC* getDestinationHwAddress() const {
        //#[ auto_generated
        return (MAC*) &destinationHwAddress;
        //#]
    }
    
    //## auto_generated
    inline IP* getDestinationIpAddress() const {
        //#[ auto_generated
        return (IP*) &destinationIpAddress;
        //#]
    }
    
    ////    Attributes    ////
    
    uint16_t hwType;		//## attribute hwType
    
    uint16_t protocol;		//## attribute protocol
    
    uint8_t hwLength;		//## attribute hwLength
    
    uint8_t protocolLength;		//## attribute protocolLength
    
    uint16_t opcode;		//## attribute opcode
    
    MAC sourceHwAddress;		//## attribute sourceHwAddress
    
    IP sourceIpAddress;		//## attribute sourceIpAddress
    
    MAC destinationHwAddress;		//## attribute destinationHwAddress
    
    IP destinationIpAddress;		//## attribute destinationIpAddress
};

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/ArpHeader.h
*********************************************************************/
