/*
 * Traces.h
 *
 * Created: 18.06.2014 14:12:56
 * Author: viktor.pankraz
 */

#ifndef SwFramework_Traces_Traces_H
#define SwFramework_Traces_Traces_H

#include <DefaultTypes.h>
#include <SwFramework.h>
class Logger;

class SystemTime;



Logger& endl( Logger& me );

Logger& newTraceLine( Logger& me );

void streamDummy( char c );

#endif
