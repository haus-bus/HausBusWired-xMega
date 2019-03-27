/*
 * Traces.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include "Traces.h"
#include "Logger.h"
#include <Time/SystemTime.h>


Logger& endl( Logger& me )
{
   return me << '\n';
}

Logger& newTraceLine( Logger& me )
{
   return me << '\n' << '>' << SystemTime::now() << ' ';
}

void streamDummy( char c )
{
}