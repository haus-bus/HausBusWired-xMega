/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: EthernetHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/EthernetHeader.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_EthernetHeader_H
#define SwFramework_Protocols_Ethernet_EthernetHeader_H

//## auto_generated
#include "Ethernet.h"
//## attribute packetDestination
#include "MAC.h"
//## package SwFramework::Protocols::Ethernet

//## class EthernetHeader
class EthernetHeader {
public :

    //#[ type EthernetPacketType
    static const uint16_t ARP            = 0x0806;
    static const uint16_t IP_DATAGRAMM   = 0x0800; 
    static const uint16_t IP6_DATAGRAMM  = 0x86DD; 
    //#]
    
    ////    Operations    ////
    
    //## operation getPacketType() const
    inline uint16_t getPacketType() const {
        //#[ operation getPacketType() const
        return changeEndianness( packetType );
        //#]
    }
    
    //## operation isArp() const
    inline bool isArp() const;
    
    //## operation isIpDatagramm() const
    inline bool isIpDatagramm() const;
    
    //## operation setPacketType(uint16_t)
    inline void setPacketType(uint16_t p_packetType) {
        //#[ operation setPacketType(uint16_t)
        packetType = changeEndianness( p_packetType );
        //#]
    }
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline MAC* getPacketDestination() const {
        //#[ auto_generated
        return (MAC*) &packetDestination;
        //#]
    }
    
    //## auto_generated
    inline MAC* getPacketSource() const {
        //#[ auto_generated
        return (MAC*) &packetSource;
        //#]
    }
    
    ////    Attributes    ////
    
    MAC packetDestination;		//## attribute packetDestination
    
    MAC packetSource;		//## attribute packetSource
    
    uint16_t packetType;		//## attribute packetType
};

inline bool EthernetHeader::isArp() const {
    //#[ operation isArp() const
    return (getPacketType() == ARP);
    //#]
}

inline bool EthernetHeader::isIpDatagramm() const {
    //#[ operation isIpDatagramm() const
    return (getPacketType() == IP_DATAGRAMM);
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/EthernetHeader.h
*********************************************************************/
