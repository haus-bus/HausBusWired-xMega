/*
 * MemoryManager.cpp
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#include "MemoryManager.h"
#include <Tracing/Logger.h>
#include <CriticalSection.h>

// From linker script
extern "C" char __heap_start;

#ifndef _HEAP_SIZE_
#define _HEAP_SIZE_ 0
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

static char* pHeapEnd( (char*)&__heap_start + _HEAP_SIZE_ );

static const uint8_t debugLevel( DEBUG_LEVEL_OFF );

static void initMemory();

static bool isInHeap( uintptr_t address );

void* allocOnce( size_t size )
{

   if ( (uintptr_t)( pHeapEnd - size ) < (uintptr_t)&__heap_start )
   {
      ERROR_2( "Heap is to small, actual size: 0x", (uintptr_t)pHeapEnd - (uintptr_t)&__heap_start );
      while ( 1 )
      {
      }
   }
   pHeapEnd -= size;
   DEBUG_M4( FSTR( "mo " ), (uintptr_t)pHeapEnd, ' ', size );
   return pHeapEnd;
}

void getUnusedMemory( uint16_t* freeStack, uint16_t* freeHeap )
{
   freeHeap[0] = freeStack[0] = 0;
   char* p = (char*)pHeapEnd - 1;
   while ( p >= (char*)&__heap_start )
   {
      if ( *p-- != HEAP_MASK )
      {
         break;
      }

      freeHeap[0]++;
   }
   p = (char*)&__heap_start + _HEAP_SIZE_;
   while ( p <= (char*) RAMEND )
   {
      if ( *p++ != STACK_MASK )
      {
         break;
      }

      freeStack[0]++;
   }
}

bool hasStackError()
{
   return ( SP < ( ( (uintptr_t)&__heap_start ) + _HEAP_SIZE_ ) );
}

void operator delete( void* ptr )
{
   // ISO C says free(NULL) must be a no-op
   if ( !ptr )
   {
      return;
   }

   DEBUG_M2( "f ", (uintptr_t)ptr );
   while ( !isInHeap( (uintptr_t)ptr ) )
   {
   }
   free( ptr );
}

void operator delete[]( void* ptr )
{
   // ISO C says free(NULL) must be a no-op
   if ( !ptr )
   {
      return;
   }

   DEBUG_M2( "f ", (uintptr_t)ptr );
   while ( !isInHeap( (uintptr_t)ptr ) )
   {
   }
   free( ptr );
}

void* operator new( size_t size )
{
   void* ptr = malloc( size );
   DEBUG_M4( FSTR( "m " ), (uintptr_t)ptr, ' ', size );
   while ( !isInHeap( (uintptr_t)ptr ) )
   {
   }
   while ( !isInHeap( (uintptr_t)ptr + size - 1 ) )
   {
   }
   return ptr;
}

void* operator new[]( size_t size )
{
   void* ptr = malloc( size );
   DEBUG_M4( FSTR( "m " ), (uintptr_t)ptr, ' ', size );
   while ( !isInHeap( (uintptr_t)ptr ) )
   {
   }
   while ( !isInHeap( (uintptr_t)ptr + size - 1 ) )
   {
   }
   return ptr;
}

// !!! Never call this function, it is part of .init-Code
void __attribute__ ( ( naked, used, section( ".init5" ) ) ) initMemory( void );
static void initMemory()
{
   // Use inline assembler so it works even with optimization turned off
    #if ( _HEAP_SIZE_ > 0 )
   // __malloc_heap_end = &__heap_start+_HEAP_SIZE_;
   __asm volatile (
      "ldi r30, lo8 (__heap_start)"  "\n\t"
      "ldi r31, hi8 (__heap_start)"  "\n\t"
      "ldi r24, %0"                  "\n\t"
      "ldi r25, hi8 (%2)"            "\n"
      "0:"                           "\n\t"
      "st  Z+,  r24"                 "\n\t"
      "cpi r30, lo8 (%2)"            "\n\t"
      "cpc r31, r25"                 "\n\t"
      "brlo 0b"                      "\n\t"
      "ldi r24, %1"                  "\n\t"
      "ldi r25, hi8 (%3)"            "\n"
      "1:"                           "\n\t"
      "st  Z+,  r24"                 "\n\t"
      "cpi r30, lo8 (%3)"            "\n\t"
      "cpc r31, r25"                 "\n\t"
      "brlo 1b"
      :
      : "i" ( HEAP_MASK ), "i" ( STACK_MASK ), "i" ( &__heap_start + _HEAP_SIZE_ ), "i" ( RAMEND + 1 )

      );

    #else

   __asm volatile (
      "ldi r30, lo8 (__heap_start)"  "\n\t"
      "ldi r31, hi8 (__heap_start)"  "\n\t"
      "ldi r24, %0"                  "\n\t"
      "ldi r25, hi8 (%1)"            "\n"
      "0:"                           "\n\t"
      "st  Z+,  r24"                 "\n\t"
      "cpi r30, lo8 (%1)"            "\n\t"
      "cpc r31, r25"                 "\n\t"
      "brlo 0b"                      "\n\t"
      :
      : "i" ( STACK_MASK ), "i" ( RAMEND + 1 )

      );
    #endif
}

static bool isInHeap( uintptr_t address )
{
   if ( ( address >= (uintptr_t)&__heap_start ) && ( address < (uintptr_t)pHeapEnd ) )
   {
      return true;
   }
   ERROR_2( address, " isOutOfRange" );
   return false;
}
