/*
 * SwFramework.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_SwFramework_H
#define SwFramework_SwFramework_H

#include "DefaultTypes.h"

class EventDrivenUnit;

class IoStream;

class Object;

class Reactive;

class Scheduler;

class StorageInterface;

class IStream;

// Converts an ASCII character to its Unicode equivalent

// converts ASCII into UNICODE
#ifndef UNICODE
#define UNICODE( a )                  ( a ), 0x00
#endif

// Character code support macros
#define IsUpper( c )  ( ( ( c ) >= 'A' ) && ( ( c ) <= 'Z' ) )
#define IsLower( c )  ( ( ( c ) >= 'a' ) && ( ( c ) <= 'z' ) )
#define IsDigit( c )  ( ( ( c ) >= '0' ) && ( ( c ) <= '9' ) )


// Returns the higher/lower byte of a word
#define HBYTE( word )                 ( (uint8_t) ( ( word ) >> 8 ) )
#define LBYTE( word )                 ( (uint8_t) ( ( word ) & 0x00FF ) )

#define HWORD( dword )                ( (uint16_t) ( ( dword ) >> 16 ) )
#define LWORD( dword )                ( (uint16_t) ( ( dword ) & 0xFFFF ) )

// ! \brief  Converts a byte array to a word value using the big endian format
#define WORDB( bytes )            ( (uint16_t) ( ( bytes[0] << 8 ) | bytes[1] ) )

// ! \brief  Converts a byte array to a dword value using the big endian format
#define DWORDB( bytes )   ( (uint32_t) ( ( bytes[0] << 24 ) | ( bytes[1] << 16 ) \
                                         | ( bytes[2] << 8 ) | bytes[3] ) )

// ! \brief  Stores a word value in a byte array, in little endian format
#define STORE_WORDL( word, bytes ) \
   bytes[0] = (uint8_t) ( word & 0xFF ); \
   bytes[1] = (uint8_t) ( ( word >> 8 ) & 0xFF );


// ! \brief  Stores a dword value in a byte array, in big endian format
#define STORE_DWORDB( dword, bytes ) \
   bytes[0] = (uint8_t) ( ( dword >> 24 ) & 0xFF ); \
   bytes[1] = (uint8_t) ( ( dword >> 16 ) & 0xFF ); \
   bytes[2] = (uint8_t) ( ( dword >> 8 ) & 0xFF ); \
   bytes[3] = (uint8_t) ( dword & 0xFF );

// ! \brief  Stores a word value in a byte array, in big endian format
#define STORE_WORDB( word, bytes ) \
   bytes[0] = (uint8_t) ( ( word >> 8 ) & 0xFF ); \
   bytes[1] = (uint8_t) ( word & 0xFF );

#ifdef NULL
#undef NULL
#endif

#define NULL 0

#define DEBUG_LEVEL_MASK     0x03
#define DEBUG_LEVEL_OFF      0x00
#define DEBUG_LEVEL_HIGH     0x01
#define DEBUG_LEVEL_MIDDLE   0x02
#define DEBUG_LEVEL_LOW      0x03

#ifdef _DEBUG_
#define DEBUG_H1( a1 )                  if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_OFF ) { Logger::instance() << newTraceLine << getId() << a1; }
#define DEBUG_H2( a1, a2 )              if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_OFF ) { Logger::instance() << newTraceLine << getId() << a1 << a2; }
#define DEBUG_H3( a1, a2, a3 )          if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_OFF ) { Logger::instance() << newTraceLine << getId() << a1 << a2 << a3; }
#define DEBUG_H4( a1, a2, a3, a4 )      if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_OFF ) { Logger::instance() << newTraceLine << getId() << a1 << a2 << a3 << a4; }
#define DEBUG_H5( a1, a2, a3, a4, a5 )  if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_OFF ) { Logger::instance() << newTraceLine << getId() << a1 << a2 << a3 << a4 << a5; }
#define DEBUG_M1( a1 )                  if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_HIGH ) { Logger::instance() << newTraceLine << '-' << a1; }
#define DEBUG_M2( a1, a2 )              if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_HIGH ) { Logger::instance() << newTraceLine << '-' << a1 << a2; }
#define DEBUG_M3( a1, a2, a3 )          if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_HIGH ) { Logger::instance() << newTraceLine << '-' << a1 << a2 << a3; }
#define DEBUG_M4( a1, a2, a3, a4 )      if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_HIGH ) { Logger::instance() << newTraceLine << '-' << a1 << a2 << a3 << a4; }
#define DEBUG_M5( a1, a2, a3, a4, a5 )  if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_HIGH ) { Logger::instance() << newTraceLine << '-' << a1 << a2 << a3 << a4 << a5; }
#define DEBUG_L1( a1 )                  if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_MIDDLE ) { Logger::instance() << a1; }
#define DEBUG_L2( a1, a2 )              if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_MIDDLE ) { Logger::instance() << a1 << a2; }
#define DEBUG_L3( a1, a2, a3 )          if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_MIDDLE ) { Logger::instance() << a1 << a2 << a3; }
#define DEBUG_L4( a1, a2, a3, a4 )      if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_MIDDLE ) { Logger::instance() << a1 << a2 << a3 << a4; }
#define DEBUG_L5( a1, a2, a3, a4, a5 )  if ( ( debugLevel & DEBUG_LEVEL_MASK ) > DEBUG_LEVEL_MIDDLE ) { Logger::instance() << a1 << a2 << a3 << a4 << a5; }
#else
#define DEBUG_H1( a1 )
#define DEBUG_H2( a1, a2 )
#define DEBUG_H3( a1, a2, a3 )
#define DEBUG_H4( a1, a2, a3, a4 )
#define DEBUG_H5( a1, a2, a3, a4, a5 )
#define DEBUG_M1( a1 )
#define DEBUG_M2( a1, a2 )
#define DEBUG_M3( a1, a2, a3 )
#define DEBUG_M4( a1, a2, a3, a4 )
#define DEBUG_M5( a1, a2, a3, a4, a5 )
#define DEBUG_L1( a1 )
#define DEBUG_L2( a1, a2 )
#define DEBUG_L3( a1, a2, a3 )
#define DEBUG_L4( a1, a2, a3, a4 )
#define DEBUG_L5( a1, a2, a3, a4, a5 )
#endif


#ifdef _DEBUG_
#define ASSERT( expr )                                      \
   {                                                           \
      if ( !( expr ) )                                            \
      {                                                         \
         /*Logger::instance()<< endl << "ASSERT(" << #expr << ')';*/ \
         while ( 1 ) {; }                                              \
      }                                                         \
   }
#define ERROR_1( a1 )                   Logger::instance() << newTraceLine << "ERROR> " << a1;
#define ERROR_2( a1, a2 )               Logger::instance() << newTraceLine << "ERROR> " << a1 << a2;
#define ERROR_3( a1, a2, a3 )           Logger::instance() << newTraceLine << "ERROR> " << a1 << a2 << a3;
#define ERROR_4( a1, a2, a3, a4 )       Logger::instance() << newTraceLine << "ERROR> " << a1 << a2 << a3 << a4;
#define ERROR_DATA_1( a1 )              Logger::instance() << a1;
#define ERROR_DATA_2( a1, a2 )          Logger::instance() << a1 << a2;
#define ERROR_DATA_3( a1, a2, a3 )      Logger::instance() << a1 << a2 << a3;
#define ERROR_DATA_4( a1, a2, a3, a4 )  Logger::instance() << a1 << a2 << a3 << a4;
#else
#define ASSERT( expr )
#define ERROR_1( a1 )
#define ERROR_2( a1, a2 )
#define ERROR_3( a1, a2, a3 )
#define ERROR_4( a1, a2, a3, a4 )
#define ERROR_DATA_1( a1 )
#define ERROR_DATA_2( a1, a2 )
#define ERROR_DATA_3( a1, a2, a3 )
#define ERROR_DATA_4( a1, a2, a3, a4 )
#endif

#define DEBUG_STATE_MASK     0x0C
#define DEBUG_STATE_L1       0x04
#define DEBUG_STATE_L2       0x08
#define DEBUG_STATE_L3       0x0C

#ifdef _DEBUG_
#define STATE_L1( a1 )    if ( ( debugLevel & DEBUG_STATE_MASK ) > DEBUG_LEVEL_OFF ) { Logger::instance() << newTraceLine << getId() << " enters -> " #a1 "(" << (uint8_t)a1 << ')'; }
#define STATE_L2( a1 )    if ( ( debugLevel & DEBUG_STATE_MASK ) > DEBUG_STATE_L1 ) { Logger::instance() << newTraceLine << getId() << " enters -> " #a1 "(" << (uint8_t)a1 << ')'; }
#define STATE_L3( a1 )    if ( ( debugLevel & DEBUG_STATE_MASK ) > DEBUG_STATE_L2 ) { Logger::instance() << newTraceLine << getId() << " enters -> " #a1 "(" << (uint8_t)a1 << ')'; }
#else
#define STATE_L1( a1 )
#define STATE_L2( a1 )
#define STATE_L3( a1 )
#endif

#ifdef _DEBUG_
#define WARN_1( a1 )              Logger::instance() << newTraceLine << "WARNING> " << a1;
#define WARN_2( a1, a2 )          Logger::instance() << newTraceLine << "WARNING> " << a1 << a2;
#define WARN_3( a1, a2, a3 )      Logger::instance() << newTraceLine << "WARNING> " << a1 << a2 << a3;
#define WARN_4( a1, a2, a3, a4 )  Logger::instance() << newTraceLine << "WARNING> " << a1 << a2 << a3 << a4;
#else
#define WARN_1( a1 )
#define WARN_2( a1, a2 )
#define WARN_3( a1, a2, a3 )
#define WARN_4( a1, a2, a3, a4 )
#endif

union convert_u
{
   uint32_t dword;
   uint16_t word[2];
   uint8_t byte[4];
};

#define CONST_IP( a, b, c, d ) ( (uint32_t)( d ) << 24 ) + ( (uint32_t)( c ) << 16 ) + ( (uint32_t)( b ) << 8 ) + a

#define SET_STATE_L1( stateL1 ) setMainState( (States)stateL1 ); STATE_L1( stateL1 );

#define SET_STATE_L2( stateL2 ) setSubState( stateL2 ); STATE_L2( stateL2 );

// change endianness little <-> big for DWORDS
uint32_t changeEndianness( const uint32_t& value );

// change endianness little <-> big for WORDS
uint16_t changeEndianness( const uint16_t& value );

// ! \brief  Converts a byte array to a dword value using the little endian format
static inline uint32_t getDwordLittle( uint8_t* byte )
{
   convert_u tmp;
   tmp.byte[0] = byte[0];
   tmp.byte[1] = byte[1];
   tmp.byte[2] = byte[2];
   tmp.byte[3] = byte[3];
   return tmp.dword;
}

// ! \brief  Converts a byte array to a word value using the little endian format
static inline uint16_t getWordLittle( uint8_t* byte )
{
   convert_u tmp;
   tmp.byte[0] = byte[0];
   tmp.byte[1] = byte[1];
   return tmp.word[0];
}

// ! \brief  Converts a byte array to a dword value using the little endian format
static inline void storeDwordLittle( uint32_t data, uint8_t* byte )
{
   convert_u* tmp = (convert_u*)&data;
   byte[0] = tmp->byte[0];
   byte[1] = tmp->byte[1];
   byte[2] = tmp->byte[2];
   byte[3] = tmp->byte[3];
}

// ! \brief  Converts a byte array to a dword value using the little endian format
static inline void storeWordLittle( uint16_t data, uint8_t* byte )
{
   convert_u* tmp = (convert_u*)&data;
   byte[0] = tmp->byte[0];
   byte[1] = tmp->byte[1];
}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t dec2bcd( uint8_t num );

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec( uint8_t num );

extern void delayMs( uint8_t ms );

extern void delayUs( uint16_t us );

static inline const uint8_t getBitPosition( uint32_t value )
{
   if ( !value )
   {
      return -1;
   }

   uint8_t pos = 0;
   while ( value != 1 )
   {
      value >>= 1;
      pos++;
   }
   return pos;
}

extern bool isRunning();

template<typename T>
inline T invertBits(T value, T bits)
{
   T masked = value & bits;
   value |= bits;
   value &= ~masked;
   return value;
}

template<typename T>
inline T maximum( const T val1, const T val2 )
{
   return ( val1 > val2 ) ? val1 : val2;
}

template<typename T>
inline T minimum( const T val1, const T val2 )
{
   return ( val1 < val2 ) ? val1 : val2;
}

template<int base, unsigned int exp>
class Pow
{
   enum { value = base * Pow<base, exp - 1>::value };
};

// stopping condition
template<int base>
class Pow<base, 0>
{
   enum { value = 1 };
};

extern void notifyBusy();

extern void notifyIdle();

#endif

