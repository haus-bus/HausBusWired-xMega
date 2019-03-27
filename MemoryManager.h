/*
 * MemoryManager.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef SwFramework_MemoryManager_MemoryManager_H
#define SwFramework_MemoryManager_MemoryManager_H

#ifdef _DEBUG_
  #include "Tracing/Logger.h"
#endif
#include "SwFramework.h"
class CriticalSection;



static const uint8_t STACK_MASK = 'S';
static const uint8_t HEAP_MASK = 'H';

#ifndef _HEAP_SIZE_
  #define _HEAP_SIZE_ 0
#endif

#ifdef __AVR__
// From linker script
extern "C" char __heap_start;
// extern "C" char __heap_end;
#else
class Heap;
extern Heap __heap_start;
#endif

#ifndef HEAP_BLOCK_SIZE
  #define HEAP_BLOCK_SIZE 32
#endif

#define MAX_HEAP_BLOCKS ( _HEAP_SIZE_ / HEAP_BLOCK_SIZE )

// this manages memory up to 255 * HEAP_BLOCK_SIZE
#if MAX_HEAP_BLOCKS > 255
#error "MAX_HEAP_BLOCKS > 255, decrease _HEAP_SIZE_ or increase HEAP_BLOCK_SIZE!"
#endif

#ifndef RAMEND
#define RAMEND ( ( (uintptr_t)& __heap_start ) + _HEAP_SIZE_ )
#endif

#ifndef SP
#define SP (uintptr_t)RAMEND
#endif


void* allocOnce( size_t size );

void getUnusedMemory( uint16_t& freeStack, uint16_t& freeHeap );

bool hasStackError();

void operator delete( void* ptr );

void operator delete[]( void* ptr );

void* operator new( size_t size );

void* operator new[]( size_t size );

#endif
