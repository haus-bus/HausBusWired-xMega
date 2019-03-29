/*
 * IpStack.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_IpStack_IpStack_H
#define SwFramework_Protocols_IpStack_IpStack_H

#include "Protocols/Protocols.h"

class ArpManager;

class Dhcp;

class IpConnection;

class IpStackManager;

class TcpConnection;

class UdpConnection;

#define IP_STACK_BUFFER_SIZE 1024

extern void* allocOnce( size_t size );

#endif

