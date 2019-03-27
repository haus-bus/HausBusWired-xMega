/*
 * UdpHeader.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "UdpHeader.h"

void UdpHeader::setChecksum()
{
   checksum = 0;
   // calculate Headerlength and add Pseudoheaderlength 2xIP = 8
   uint16_t len = UdpHeader::getLength() + 8;
   uint16_t startValue = len - 8 + IpHeader::UDP_PROTOCOL;
   setChecksum( Checksum16::get( &sourceAddress, len, startValue ) );
}