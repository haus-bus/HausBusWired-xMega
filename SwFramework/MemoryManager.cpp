/********************************************************************
	Rhapsody	: 8.0.3
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr
	Configuration 	: debug
	Model Element	: MemoryManager
//!	Generated Date	: Wed, 18, Jun 2014
	File Path	: SwFrameworkAvr/debug/SwFramework/MemoryManager/MemoryManager.cpp
*********************************************************************/

//## auto_generated
#include "MemoryManager.h"
//## dependency CriticalSection
#include <CriticalSection.h>
//## package SwFramework::MemoryManager

static char* pHeapEnd((char*)&__heap_start + _HEAP_SIZE_);

//## attribute debugLevel
static const uint8_t debugLevel(DEBUG_LEVEL_OFF);

//## operation initMemory()
static void initMemory();

//## operation isInHeap(uintptr_t)
static bool isInHeap(uintptr_t address);

//## operation allocOnce(size_t)
void * allocOnce(size_t size) {
    //#[ operation allocOnce(size_t)

    if ( (uintptr_t)( pHeapEnd - size ) < (uintptr_t)&__heap_start )
    {
      ERROR_2( "Heap is to small, actual size: 0x", (uintptr_t)pHeapEnd - (uintptr_t)&__heap_start );
      while( 1 );
    }
    pHeapEnd -= size;
    DEBUG_M4( FSTR("mo "), (uintptr_t)pHeapEnd, ' ', size );
    return pHeapEnd;
    //#]
}

//## operation getUnusedMemory(uint16_t,uint16_t)
void getUnusedMemory(uint16_t& freeStack, uint16_t& freeHeap) {
    //#[ operation getUnusedMemory(uint16_t,uint16_t)
    freeHeap = freeStack = 0;
    char* p = (char*)pHeapEnd-1;
    while( p >= (char*)&__heap_start )
    {
      if( *p-- != HEAP_MASK )
         break;

      freeHeap++;
    }
    p = (char*)&__heap_start + _HEAP_SIZE_;
    while( p <= (char*) RAMEND )
    {
      if( *p++ != STACK_MASK )
         break;

      freeStack++;
    }
    //#]
}

//## operation hasStackError()
bool hasStackError() {
    //#[ operation hasStackError()
    return ( SP < (((uintptr_t)&__heap_start) + _HEAP_SIZE_ ) );
    //#]
}

//## operation operator delete(void *)
void operator delete(void * ptr) {
    //#[ operation operator delete(void *)
    // ISO C says free(NULL) must be a no-op
    if( !ptr )
    {
      return;
    }

    DEBUG_M2( "f ", (uintptr_t)ptr );
    while( !isInHeap( (uintptr_t)ptr ) );
    free( ptr );
    //#]
}

//## operation operator delete[](void *)
void operator delete[](void * ptr) {
    //#[ operation operator delete[](void *)
    // ISO C says free(NULL) must be a no-op
    if( !ptr )
    {
      return;
    }

    DEBUG_M2( "f ", (uintptr_t)ptr );
    while( !isInHeap( (uintptr_t)ptr ) );
    free( ptr );
    //#]
}

//## operation operator new(size_t)
void * operator new(size_t size) {
    //#[ operation operator new(size_t)
    void* ptr = malloc( size );
    DEBUG_M4( FSTR("m "), (uintptr_t)ptr, ' ', size );
    while( !isInHeap( (uintptr_t)ptr ) );
    while( !isInHeap( (uintptr_t)ptr + size - 1 ) );
    return ptr;
    //#]
}

//## operation operator new[](size_t)
void * operator new[](size_t size) {
    //#[ operation operator new[](size_t)
    void* ptr = malloc( size );
    DEBUG_M4( FSTR("m "), (uintptr_t)ptr, ' ', size );
    while( !isInHeap( (uintptr_t)ptr ) );
    while( !isInHeap( (uintptr_t)ptr + size - 1 ) );
    return ptr;
    //#]
}

//## operation initMemory()
//#[ ignore
// !!! Never call this function, it is part of .init-Code
#ifdef __AVR__
void __attribute__ ((naked, used, section(".init5"))) initMemory (void);
//#]
static void initMemory() {
    //#[ operation initMemory()
    //  Use inline assembler so it works even with optimization turned off
    #if ( _HEAP_SIZE_ > 0 )
        //__malloc_heap_end = &__heap_start+_HEAP_SIZE_;
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
           : "i" (HEAP_MASK), "i" (STACK_MASK), "i" (&__heap_start+_HEAP_SIZE_), "i" (RAMEND+1)

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
         : "i" (STACK_MASK), "i" (RAMEND+1)

      );
    #endif
    //#]
}
#endif

//## operation isInHeap(uintptr_t)
static bool isInHeap(uintptr_t address) {
    //#[ operation isInHeap(uintptr_t)
    #ifdef WIN32
      return true;
    #else
      if( (address >= (uintptr_t)&__heap_start) && (address < (uintptr_t)pHeapEnd) )
      {
        return true;
      }
      ERROR_2( address, " isOutOfRange" );
      return false;
    #endif
    //#]
}

/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/MemoryManager/MemoryManager.cpp
*********************************************************************/
