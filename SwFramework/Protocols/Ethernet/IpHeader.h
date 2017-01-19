/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: IpHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/IpHeader.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_IpHeader_H
#define SwFramework_Protocols_Ethernet_IpHeader_H

//## auto_generated
#include "Ethernet.h"
//## dependency Checksum16
#include "Security/Checksum16.h"
//## class IpHeader
#include "EthernetHeader.h"
//## attribute sourceAddress
#include "IP.h"
//## package SwFramework::Protocols::Ethernet

//## class IpHeader
class IpHeader : public EthernetHeader {
public :

    //#[ type Protocoll
    static const uint8_t ICMP_PROTOCOL = 0x01;    
    static const uint8_t TCP_PROTOCOL  = 0x06;
    static const uint8_t UDP_PROTOCOL  = 0x11;
    //#]
    
    //#[ type FragmentOffset
    static const uint16_t DO_NOT_FRAGMENT = 0x4000;    
    static const uint16_t MORE_FRAGMENTS  = 0x8000;
    //#]
    
    //#[ type MIN_PACKET_SIZE
    static const uint8_t MIN_PACKET_SIZE = 60;
    //#]
    
    ////    Operations    ////
    
    //## operation getChecksum() const
    inline uint16_t getChecksum() const {
        //#[ operation getChecksum() const
        return changeEndianness( checksum );
        //#]
    }
    
    //## operation getFragmentOffset() const
    inline uint16_t getFragmentOffset() const {
        //#[ operation getFragmentOffset() const
        return changeEndianness( fragmentOffset );
        //#]
    }
    
    //## operation getId() const
    inline uint16_t getId() const {
        //#[ operation getId() const
        return changeEndianness( id );
        //#]
    }
    
    //## operation getPacketSize() const
    inline uint16_t getPacketSize() const {
        //#[ operation getPacketSize() const
        return changeEndianness( packetSize );
        //#]
    }
    
    //## operation isChecksumCorrect()
    inline bool isChecksumCorrect();
    
    //## operation isFragmented()
    inline bool isFragmented();
    
    //## operation isPacketSizeValid(uint16_t)
    inline bool isPacketSizeValid(uint16_t dataLength) {
        //#[ operation isPacketSizeValid(uint16_t)
        uint16_t dataSize = getPacketSize() + sizeof(EthernetHeader); 
        return ( dataSize <= dataLength ) ;
        //#]
    }
    
    //## operation isProtocollIcmp()
    inline bool isProtocollIcmp();
    
    //## operation isProtocollTcp()
    inline bool isProtocollTcp();
    
    //## operation isProtocollUdp()
    inline bool isProtocollUdp();
    
    //## operation isValid()
    inline bool isValid();
    
    //## operation setChecksum(uint16_t)
    inline void setChecksum(uint16_t p_checksum) {
        //#[ operation setChecksum(uint16_t)
        checksum = changeEndianness( p_checksum );
        //#]
    }
    
    //## operation setChecksum()
    void setChecksum();
    
    //## operation setFragmentOffset(uint16_t)
    inline void setFragmentOffset(uint16_t p_fragmentOffset) {
        //#[ operation setFragmentOffset(uint16_t)
        fragmentOffset = changeEndianness( p_fragmentOffset );
        //#]
    }
    
    //## operation setId(uint16_t)
    inline void setId(uint16_t p_id) {
        //#[ operation setId(uint16_t)
        id = changeEndianness( p_id );
        //#]
    }
    
    //## operation setPacketSize(uint16_t)
    inline void setPacketSize(uint16_t p_packetSize) {
        //#[ operation setPacketSize(uint16_t)
        packetSize = changeEndianness( p_packetSize );
        //#]
    }
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getLength() const {
        //#[ auto_generated
        return length;
        //#]
    }
    
    //## auto_generated
    inline void setLength(uint8_t p_length) {
        //#[ auto_generated
        length = p_length;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getVersion() const {
        //#[ auto_generated
        return version;
        //#]
    }
    
    //## auto_generated
    inline void setVersion(uint8_t p_version) {
        //#[ auto_generated
        version = p_version;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getTypeOfService() const {
        //#[ auto_generated
        return typeOfService;
        //#]
    }
    
    //## auto_generated
    inline void setTypeOfService(uint8_t p_typeOfService) {
        //#[ auto_generated
        typeOfService = p_typeOfService;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getTimeToLive() const {
        //#[ auto_generated
        return timeToLive;
        //#]
    }
    
    //## auto_generated
    inline void setTimeToLive(uint8_t p_timeToLive) {
        //#[ auto_generated
        timeToLive = p_timeToLive;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getProtocol() const {
        //#[ auto_generated
        return protocol;
        //#]
    }
    
    //## auto_generated
    inline void setProtocol(uint8_t p_protocol) {
        //#[ auto_generated
        protocol = p_protocol;
        //#]
    }
    
    //## auto_generated
    inline IP* getSourceAddress() const {
        //#[ auto_generated
        return (IP*) &sourceAddress;
        //#]
    }
    
    //## auto_generated
    inline IP* getDestinationAddress() const {
        //#[ auto_generated
        return (IP*) &destinationAddress;
        //#]
    }

    ////    Attributes    ////
    
    uint8_t length : 4;		//## attribute length
    
    uint8_t version : 4;		//## attribute version
    
    uint8_t typeOfService;		//## attribute typeOfService
    
    uint16_t packetSize;		//## attribute packetSize
    
    uint16_t id;		//## attribute id
    
    uint16_t fragmentOffset;		//## attribute fragmentOffset
    
    uint8_t timeToLive;		//## attribute timeToLive
    
    uint8_t protocol;		//## attribute protocol
    
    uint16_t checksum;		//## attribute checksum
    
    IP sourceAddress;		//## attribute sourceAddress
    
    IP destinationAddress;		//## attribute destinationAddress
};

inline bool IpHeader::isChecksumCorrect() {
    //#[ operation isChecksumCorrect()
    return Checksum16::get( ((EthernetHeader*)this)+1, getLength()<<2 ) == 0;
    //#]
}

inline bool IpHeader::isFragmented() {
    //#[ operation isFragmented()
    return ( fragmentOffset & 0xFF3F ) != 0;
    //#]
}

inline bool IpHeader::isProtocollIcmp() {
    //#[ operation isProtocollIcmp()
    return (protocol == ICMP_PROTOCOL);
    //#]
}

inline bool IpHeader::isProtocollTcp() {
    //#[ operation isProtocollTcp()
    return (protocol == TCP_PROTOCOL);
    //#]
}

inline bool IpHeader::isProtocollUdp() {
    //#[ operation isProtocollUdp()
    return (protocol == UDP_PROTOCOL);
    //#]
}

inline bool IpHeader::isValid() {
    //#[ operation isValid()
    return ( version == 4 ) && ( length == 5 );
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/IpHeader.h
*********************************************************************/
