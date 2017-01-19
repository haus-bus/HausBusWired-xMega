/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: UdpHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/UdpHeader.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_UdpHeader_H
#define SwFramework_Protocols_Ethernet_UdpHeader_H

//## auto_generated
#include "Ethernet.h"
//## class UdpHeader
#include "IpHeader.h"
//## package SwFramework::Protocols::Ethernet

//## class UdpHeader
class UdpHeader : public IpHeader {
    ////    Operations    ////
    
public :

    //## operation getChecksum() const
    inline uint16_t getChecksum() const {
        //#[ operation getChecksum() const
        return changeEndianness( checksum );
        //#]
    }
    
    //## operation getDestinationPort() const
    inline uint16_t getDestinationPort() const {
        //#[ operation getDestinationPort() const
        return changeEndianness( destinationPort );
        //#]
    }
    
    //## operation getLength() const
    inline uint16_t getLength() const {
        //#[ operation getLength() const
        return changeEndianness( length );
        //#]
    }
    
    //## operation getSourcePort() const
    inline uint16_t getSourcePort() const {
        //#[ operation getSourcePort() const
        return changeEndianness( sourcePort );
        //#]
    }
    
    //## operation isChecksumCorrect()
    inline bool isChecksumCorrect();
    
    //## operation isDestinationPort(uint16_t)
    inline bool isDestinationPort(uint16_t port);
    
    //## operation setChecksum(uint16_t)
    inline void setChecksum(uint16_t p_checksum) {
        //#[ operation setChecksum(uint16_t)
        checksum = changeEndianness( p_checksum );
        //#]
    }
    
    //## operation setChecksum()
    void setChecksum();
    
    //## operation setDestinationPort(uint16_t)
    inline void setDestinationPort(uint16_t p_destinationPort) {
        //#[ operation setDestinationPort(uint16_t)
        destinationPort = changeEndianness( p_destinationPort );
        //#]
    }
    
    //## operation setLength(uint16_t)
    inline void setLength(uint16_t p_length) {
        //#[ operation setLength(uint16_t)
        length = changeEndianness( p_length );
        //#]
    }
    
    //## operation setSourcePort(uint16_t)
    inline void setSourcePort(uint16_t p_sourcePort) {
        //#[ operation setSourcePort(uint16_t)
        sourcePort = changeEndianness( p_sourcePort );
        //#]
    }
    
    ////    Attributes    ////
    
    uint16_t sourcePort;		//## attribute sourcePort
    
    uint16_t destinationPort;		//## attribute destinationPort
    
    uint16_t length;		//## attribute length
    
    uint16_t checksum;		//## attribute checksum
};

inline bool UdpHeader::isChecksumCorrect() {
    //#[ operation isChecksumCorrect()
    // calculate data length and add Pseudoheaderlength 2xIP = 8     
    uint16_t len = getPacketSize() - ( sizeof(IpHeader)-sizeof(EthernetHeader) ) + 8; 
    uint16_t startValue = len - 8 + IpHeader::UDP_PROTOCOL;
    return ( Checksum16::get( &sourceAddress, len, startValue ) == 0 );
    //#]
}

inline bool UdpHeader::isDestinationPort(uint16_t port) {
    //#[ operation isDestinationPort(uint16_t)
    return (getDestinationPort() == port);
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/UdpHeader.h
*********************************************************************/
