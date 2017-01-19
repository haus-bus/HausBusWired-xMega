/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: Ethernet
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/Ethernet.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_Ethernet_H
#define SwFramework_Protocols_Ethernet_Ethernet_H

//## auto_generated
#include "Protocols/Protocols.h"
//## auto_generated
class ArpHeader;

//## auto_generated
class EthernetHeader;

//## auto_generated
class IP;

//## auto_generated
class IcmpHeader;

//## auto_generated
class IpHeader;

//## auto_generated
class MAC;

//## auto_generated
class TcpHeader;

//## auto_generated
class UdpHeader;

//## package SwFramework::Protocols::Ethernet


//#[ type MACRO
//#define HTONS(n) changeEndianness( n )
//#]

//#[ type Configuration
#define DEFAULT_IP          IP( 192, 168,   0, 254 ) 
#define DEFAULT_ROUTER_IP   IP( 192, 168,   0,   1 )
#define DEFAULT_NETMASK     IP( 255, 255, 255,   0 )
//#]

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/Ethernet.h
*********************************************************************/
