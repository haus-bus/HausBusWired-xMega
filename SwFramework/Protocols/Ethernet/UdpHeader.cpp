/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: UdpHeader
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/UdpHeader.cpp
*********************************************************************/

//## auto_generated
#include "UdpHeader.h"
//## package SwFramework::Protocols::Ethernet

//## class UdpHeader
void UdpHeader::setChecksum() {
    //#[ operation setChecksum()
    checksum = 0;
    // calculate Headerlength and add Pseudoheaderlength 2xIP = 8     
    uint16_t len = UdpHeader::getLength() + 8; 
    uint16_t startValue = len - 8 + IpHeader::UDP_PROTOCOL;
    setChecksum( Checksum16::get( &sourceAddress, len, startValue ) );
    //#]
}

/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/UdpHeader.cpp
*********************************************************************/
