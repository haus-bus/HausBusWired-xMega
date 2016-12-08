/*
 * CriticalSection.h
 *
 *  Created on: 08.12.2016 22:17:53
 *      Author: haus-bus
 */

#ifndef CriticalSection_H
#define CriticalSection_H

#include "GlobalInterrupt.h"

class CriticalSection : public GlobalInterrupt
 {
    ////    Constructors and destructors    ////
    
public :

    inline CriticalSection() 
	{
        sreg = SREG;
        disable();
    }

    inline ~CriticalSection() 
	{
        SREG = sreg;
    }
    
    ////    Attributes    ////

private :

    unsigned char sreg;	
};

#endif
