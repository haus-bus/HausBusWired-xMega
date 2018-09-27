/*
 * IpHeader.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "IpHeader.h"

void IpHeader::setChecksum()
{
   checksum = 0;
   setChecksum( Checksum16::get( ( (EthernetHeader*)this ) + 1, getLength() << 2 ) );
}