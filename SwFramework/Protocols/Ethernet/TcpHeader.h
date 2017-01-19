/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: TcpHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/TcpHeader.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_TcpHeader_H
#define SwFramework_Protocols_Ethernet_TcpHeader_H

//## auto_generated
#include "Ethernet.h"
//## class TcpHeader
#include "IpHeader.h"
//## package SwFramework::Protocols::Ethernet

//## class TcpHeader
class TcpHeader : public IpHeader {
public :

    //## type Flags
    enum Flags {
        F_FIN = 0x01,
        F_SYN = 0x02,
        F_RST = 0x04,
        F_PSH = 0x08,
        F_ACK = 0x10,
        F_URG = 0x20,
        F_CTL = 0x3F
    };
    
    //## type Options
    enum Options {
        OPT_END,
        OPT_NOOP,
        OPT_MSS,
        OPT_MSS_LEN = 4
    };
    
    ////    Operations    ////
    
    //## operation changeToAckPacket()
    inline void changeToAckPacket();
    
    //## operation changeToFinAckPacket()
    inline void changeToFinAckPacket();
    
    //## operation changeToResetAckPacket()
    void changeToResetAckPacket();
    
    //## operation changeToResetPacket()
    void changeToResetPacket();
    
    //## operation getAckNumber() const
    inline uint32_t getAckNumber() const {
        //#[ operation getAckNumber() const
        return changeEndianness( ackNumber );
        //#]
    }
    
    //## operation getChecksum() const
    inline uint16_t getChecksum() const {
        //#[ operation getChecksum() const
        return changeEndianness( checksum );
        //#]
    }
    
    // returns dataOffset in bytes after TcpHeader 
    //## operation getData() const
    uint8_t* getData() const;
    
    // returns dataOffset in bytes after TcpHeader 
    //## operation getDataLength() const
    uint16_t getDataLength() const;
    
    // returns dataOffset in bytes after TcpHeader 
    //## operation getDataOffset() const
    uint16_t getDataOffset() const;
    
    //## operation getDestinationPort() const
    inline uint16_t getDestinationPort() const {
        //#[ operation getDestinationPort() const
        return changeEndianness( destinationPort );
        //#]
    }
    
    // returns dataOffset in bytes after TcpHeader 
    //## operation getOptions() const
    uint8_t* getOptions() const;
    
    //## operation getSeqNumber() const
    inline uint32_t getSeqNumber() const {
        //#[ operation getSeqNumber() const
        return changeEndianness( seqNumber );
        //#]
    }
    
    //## operation getSourcePort() const
    inline uint16_t getSourcePort() const {
        //#[ operation getSourcePort() const
        return changeEndianness( sourcePort );
        //#]
    }
    
    //## operation getUrgentPointer() const
    inline uint16_t getUrgentPointer() const {
        //#[ operation getUrgentPointer() const
        return changeEndianness( urgentPointer );
        //#]
    }
    
    //## operation getWindow() const
    inline uint16_t getWindow() const {
        //#[ operation getWindow() const
        return changeEndianness( window );
        //#]
    }
    
    //## operation isACK() const
    inline bool isACK() const;
    
    //## operation isChecksumCorrect()
    inline bool isChecksumCorrect();
    
    //## operation isDestinationPort(uint16_t) const
    inline bool isDestinationPort(uint16_t port) const;
    
    //## operation isFIN() const
    inline bool isFIN() const;
    
    //## operation isOnlySYN() const
    inline bool isOnlySYN() const;
    
    //## operation isRST() const
    inline bool isRST() const;
    
    //## operation isSYN() const
    inline bool isSYN() const;
    
    //## operation isSYN_ACK() const
    inline bool isSYN_ACK() const;
    
    //## operation isURG() const
    inline bool isURG() const;
    
    //## operation prepareSynAck(uint16_t)
    void prepareSynAck(uint16_t maxSegmentSize);
    
    //## operation setAckNumber(uint32_t)
    inline void setAckNumber(const uint32_t& p_ackNumber) {
        //#[ operation setAckNumber(uint32_t)
        ackNumber = changeEndianness( p_ackNumber );
        //#]
    }
    
    //## operation setChecksum(uint16_t)
    inline void setChecksum(uint16_t p_checksum) {
        //#[ operation setChecksum(uint16_t)
        checksum = changeEndianness( p_checksum );
        //#]
    }
    
    //## operation setChecksum()
    void setChecksum();
    
    //## operation setDataOffset(uint8_t)
    void setDataOffset(uint8_t p_dataOffset);
    
    //## operation setDestinationPort(uint16_t)
    inline void setDestinationPort(uint16_t p_destinationPort) {
        //#[ operation setDestinationPort(uint16_t)
        destinationPort = changeEndianness( p_destinationPort );
        //#]
    }
    
    //## operation setSeqNumber(uint32_t)
    inline void setSeqNumber(const uint32_t& p_seqNumber) {
        //#[ operation setSeqNumber(uint32_t)
        seqNumber = changeEndianness( p_seqNumber );
        //#]
    }
    
    //## operation setSourcePort(uint16_t)
    inline void setSourcePort(uint16_t p_sourcePort) {
        //#[ operation setSourcePort(uint16_t)
        sourcePort = changeEndianness( p_sourcePort );
        //#]
    }
    
    //## operation setUrgentPointer(uint16_t)
    inline void setUrgentPointer(uint16_t p_urgentPointer) {
        //#[ operation setUrgentPointer(uint16_t)
        urgentPointer = changeEndianness( p_urgentPointer );
        //#]
    }
    
    //## operation setWindow(uint16_t)
    inline void setWindow(uint16_t p_window) {
        //#[ operation setWindow(uint16_t)
        window = changeEndianness( p_window );
        //#]
    }
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getFlags() const {
        //#[ auto_generated
        return flags;
        //#]
    }
    
    //## auto_generated
    inline void setFlags(uint8_t p_flags) {
        //#[ auto_generated
        flags = p_flags;
        //#]
    }
    
    ////    Attributes    ////
    
    uint16_t sourcePort;		//## attribute sourcePort
    
    uint16_t destinationPort;		//## attribute destinationPort
    
    uint32_t seqNumber;		//## attribute seqNumber
    
    uint32_t ackNumber;		//## attribute ackNumber
    
    uint8_t dataOffset;		//## attribute dataOffset
    
    uint8_t flags;		//## attribute flags
    
    uint16_t window;		//## attribute window
    
    uint16_t checksum;		//## attribute checksum
    
    uint16_t urgentPointer;		//## attribute urgentPointer
};

inline void TcpHeader::changeToAckPacket() {
    //#[ operation changeToAckPacket()
    flags = F_ACK;
    setDataOffset( 0 );
    //#]
}

inline void TcpHeader::changeToFinAckPacket() {
    //#[ operation changeToFinAckPacket()
    flags = F_FIN | F_ACK;
    setDataOffset( 0 );
    //#]
}

inline bool TcpHeader::isACK() const {
    //#[ operation isACK() const
    return ( flags & F_ACK ) == F_ACK;
    //#]
}

inline bool TcpHeader::isChecksumCorrect() {
    //#[ operation isChecksumCorrect()
    // calculate data length and add Pseudoheaderlength 2xIP = 8     
    uint16_t len = getPacketSize() - ( sizeof(IpHeader)-sizeof(EthernetHeader) ) + 8; 
    uint16_t startValue = len - 8 + IpHeader::TCP_PROTOCOL;
    return ( Checksum16::get( &sourceAddress, len, startValue ) == 0 );
    //#]
}

inline bool TcpHeader::isDestinationPort(uint16_t port) const {
    //#[ operation isDestinationPort(uint16_t) const
    return (getDestinationPort() == port);
    //#]
}

inline bool TcpHeader::isFIN() const {
    //#[ operation isFIN() const
    return ( flags & F_FIN ) == F_FIN;
    //#]
}

inline bool TcpHeader::isOnlySYN() const {
    //#[ operation isOnlySYN() const
    return ( ( flags & F_CTL ) == F_SYN ) ;
    //#]
}

inline bool TcpHeader::isRST() const {
    //#[ operation isRST() const
    return ( flags & F_RST ) == F_RST;
    //#]
}

inline bool TcpHeader::isSYN() const {
    //#[ operation isSYN() const
    return ( flags & F_SYN ) == F_SYN;
    //#]
}

inline bool TcpHeader::isSYN_ACK() const {
    //#[ operation isSYN_ACK() const
    return ( flags & F_CTL ) == ( F_SYN | F_ACK );
    //#]
}

inline bool TcpHeader::isURG() const {
    //#[ operation isURG() const
    return ( flags & F_URG ) == F_URG;
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/TcpHeader.h
*********************************************************************/
