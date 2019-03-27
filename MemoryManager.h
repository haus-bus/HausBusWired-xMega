/*
 * MemoryManager.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef SwFramework_MemoryManager_MemoryManager_H
#define SwFramework_MemoryManager_MemoryManager_H

#include <stdint.h>
#include <stddef.h>

static const uint8_t STACK_MASK = 'S';
static const uint8_t HEAP_MASK = 'H';

void* allocOnce( size_t size );

void getUnusedMemory( uint16_t* freeStack, uint16_t* freeHeap );

bool hasStackError();

void operator delete( void* ptr );

void operator delete[]( void* ptr );

void* operator new( size_t size );

void* operator new[]( size_t size );

#endif
