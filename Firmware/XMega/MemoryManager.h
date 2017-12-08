/*
 * MemoryManager.h
 *
 *  Created on: 07.12.2017
 *      Author: Viktor Pankraz
 */

#ifndef Framework_MemoryManager_MemoryManager_H
#define Framework_MemoryManager_MemoryManager_H

#include <stdio.h>

void* allocOnce( size_t size );

void getUnusedMemory( uint16_t& freeStack, uint16_t& freeHeap );

bool hasStackError();

#endif

