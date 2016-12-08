/*
 * GlobalInterrupt.h
 *
 *  Created on: 08.12.2016 22:17:53
 *      Author: haus-bus
 */

#ifndef GlobalInterrupt_H
#define GlobalInterrupt_H

#include <avr/io.h>

class GlobalInterrupt 
{
    ////    Operations    ////
    
public :

    inline static void disable() 
	{
        __asm__ __volatile__ ("cli" :: );
    }

    inline static void enable() 
	{
        __asm__ __volatile__ ("sei" :: );
    }

    inline static void waitForInterrupt() 
	{
        __asm__ __volatile__ ("sleep");
    }
};

#endif
