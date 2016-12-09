/*
 * Framework.h
 *
 * Created: 03.12.2016 03:17:53
 *  Author: haus-bus
 */ 


#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

extern "C"
{
	#include <asf.h>
}

#include "Logger.h"
#include "Timestamp.h"
#include "CriticalSection.h"

#define DEBUG_LEVEL_MASK     0x03
#define DEBUG_LEVEL_OFF      0x00
#define DEBUG_LEVEL_HIGH     0x01
#define DEBUG_LEVEL_MIDDLE   0x02
#define DEBUG_LEVEL_LOW      0x03

#ifdef DEBUG
#define DEBUG_H1( a1 )                  if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_OFF )    Logger::instance() << newTraceLine << getId() << a1;
#define DEBUG_H2( a1, a2 )              if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_OFF )    Logger::instance() << newTraceLine << getId() << a1 << a2;
#define DEBUG_H3( a1, a2, a3 )          if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_OFF )    Logger::instance() << newTraceLine << getId() << a1 << a2 << a3;
#define DEBUG_H4( a1, a2, a3, a4 )      if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_OFF )    Logger::instance() << newTraceLine << getId() << a1 << a2 << a3 << a4;
#define DEBUG_H5( a1, a2, a3, a4, a5 )  if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_OFF )    Logger::instance() << newTraceLine << getId() << a1 << a2 << a3 << a4 << a5;
#define DEBUG_M1( a1 )                  if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_HIGH )   Logger::instance() << newTraceLine << '-' << a1;
#define DEBUG_M2( a1, a2 )              if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_HIGH )   Logger::instance() << newTraceLine << '-' << a1 << a2;
#define DEBUG_M3( a1, a2, a3 )          if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_HIGH )   Logger::instance() << newTraceLine << '-' << a1 << a2 << a3;
#define DEBUG_M4( a1, a2, a3, a4 )      if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_HIGH )   Logger::instance() << newTraceLine << '-' << a1 << a2 << a3 << a4;
#define DEBUG_M5( a1, a2, a3, a4, a5 )  if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_HIGH )   Logger::instance() << newTraceLine << '-' << a1 << a2 << a3 << a4 << a5;
#define DEBUG_L1( a1 )                  if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_MIDDLE ) Logger::instance() << a1;
#define DEBUG_L2( a1, a2 )              if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_MIDDLE ) Logger::instance() << a1 << a2;
#define DEBUG_L3( a1, a2, a3 )          if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_MIDDLE ) Logger::instance() << a1 << a2 << a3;
#define DEBUG_L4( a1, a2, a3, a4 )      if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_MIDDLE ) Logger::instance() << a1 << a2 << a3 << a4;
#define DEBUG_L5( a1, a2, a3, a4, a5 )  if( (debugLevel & DEBUG_LEVEL_MASK) > DEBUG_LEVEL_MIDDLE ) Logger::instance() << a1 << a2 << a3 << a4 << a5;
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


#ifdef DEBUG
#define ASSERT( expr )                                      \
{                                                           \
	if ( !(expr) )                                            \
	{                                                         \
		/*Logger::instance()<< endl << "ASSERT(" << #expr << ')';*/ \
		while (1);                                              \
	}                                                         \
}
#define ERROR_1( a1 )              Logger::instance()<< newTraceLine << "ERROR> " << a1 ;
#define ERROR_2( a1, a2 )          Logger::instance()<< newTraceLine << "ERROR> " << a1 << a2;
#define ERROR_3( a1, a2, a3 )      Logger::instance()<< newTraceLine << "ERROR> " << a1 << a2 << a3;
#define ERROR_4( a1, a2, a3, a4 )  Logger::instance()<< newTraceLine << "ERROR> " << a1 << a2 << a3 << a4;
#else
#define ASSERT( expr )
#define ERROR_1( a1 )
#define ERROR_2( a1, a2 )
#define ERROR_3( a1, a2, a3 )
#define ERROR_4( a1, a2, a3, a4 )
#endif

#define DEBUG_STATE_MASK     0x0C
#define DEBUG_STATE_L1       0x04
#define DEBUG_STATE_L2       0x08
#define DEBUG_STATE_L3       0x0C

#ifdef DEBUG
#define STATE_L1( a1 )    if( (debugLevel & DEBUG_STATE_MASK) > DEBUG_LEVEL_OFF )  Logger::instance()<< newTraceLine << getId() << " enters -> " #a1;
#define STATE_L2( a1 )    if( (debugLevel & DEBUG_STATE_MASK) > DEBUG_STATE_L1 )   Logger::instance()<< newTraceLine << getId() << " enters -> " #a1;
#define STATE_L3( a1 )    if( (debugLevel & DEBUG_STATE_MASK) > DEBUG_STATE_L2 )   Logger::instance()<< newTraceLine << getId() << " enters -> " #a1;
#else
#define STATE_L1( a1 )
#define STATE_L2( a1 )
#define STATE_L3( a1 )
#endif

#ifdef DEBUG
#define WARN_1( a1 )              Logger::instance()<< newTraceLine << "WARNING> " << a1;
#define WARN_2( a1, a2 )          Logger::instance()<< newTraceLine << "WARNING> " << a1 << a2;
#define WARN_3( a1, a2, a3 )      Logger::instance()<< newTraceLine << "WARNING> " << a1 << a2 << a3;
#define WARN_4( a1, a2, a3, a4 )  Logger::instance()<< newTraceLine << "WARNING> " << a1 << a2 << a3 << a4;
#else
#define WARN_1( a1 )
#define WARN_2( a1, a2 )
#define WARN_3( a1, a2, a3 )
#define WARN_4( a1, a2, a3, a4 )
#endif


#endif /* FRAMEWORK_H_ */