/*
 * HBW-Booter.h
 *
 * Created: 30.08.2017 00:29:36
 *  Author: viktor.pankraz
 */


#ifndef HBW_BOOTER_H_
#define HBW_BOOTER_H_

#include <Security/ModuleId.h>
#include "HmwUnits/HmwDevice.h"

extern __attribute__( ( section( ".vectors" ) ) ) const ModuleId moduleId;



#endif