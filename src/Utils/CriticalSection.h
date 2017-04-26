/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: CriticalSection
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/Basics/CriticalSection.h
*********************************************************************/

#ifndef Basics_CriticalSection_H
#define Basics_CriticalSection_H

extern "C"
{
    #include <asf.h>
}

class CriticalSection 
{
    ////    Constructors and destructors    ////
    
public :

    inline CriticalSection() 
    {
        flags = cpu_irq_save();
    }
    
    inline ~CriticalSection() 
    {
        cpu_irq_restore( flags );
    }
    
    ////    Attributes    ////

private :

    irqflags_t flags;
};

#endif

