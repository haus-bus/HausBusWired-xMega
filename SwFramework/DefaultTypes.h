/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: DefaultTypes
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/DefaultTypes.h
*********************************************************************/

#ifndef DefaultTypes_DefaultTypes_H
#define DefaultTypes_DefaultTypes_H

//## dependency math
#include <math.h>
//## dependency stddef
#include <stddef.h>
//## dependency stdint
#include <stdint.h>
//## dependency stdlib
#include <stdlib.h>
//## dependency string
#include <string.h>
//## package DefaultTypes
#include <CriticalSection.h>


//#[ type pFunction
typedef void (*pFunction)();
//#]


#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#define GCC_DIAG_STR(s) #s
#define GCC_DIAG_JOINSTR(x,y) GCC_DIAG_STR(x ## y)
# define GCC_DIAG_DO_PRAGMA(x) _Pragma (#x)
# define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
# define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(push) \
         GCC_DIAG_PRAGMA(ignored GCC_DIAG_JOINSTR(-W,x))
# define GCC_DIAG_ON(x) GCC_DIAG_PRAGMA(pop)
# else
# define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(ignored GCC_DIAG_JOINSTR(-W,x))
# define GCC_DIAG_ON(x) GCC_DIAG_PRAGMA(warning GCC_DIAG_JOINSTR(-W,x))
# endif
#else
# define GCC_DIAG_OFF(x)
# define GCC_DIAG_ON(x)
#endif

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/DefaultTypes.h
*********************************************************************/
