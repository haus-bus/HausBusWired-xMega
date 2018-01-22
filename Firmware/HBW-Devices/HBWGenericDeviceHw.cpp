/*
 * HBWGenericDeviceHw.cpp
 *
 * Created: 20.01.2018 11:52:48
 *  Author: viktor.pankraz
 */

 #include "HBWGenericDeviceHw.h"

 #include <DigitalOutput.h>

void debug( char c )
{
   Usart::instance<PortD, 1>().write( c );
}

HBWGenericDeviceHw::HBWGenericDeviceHw()
{
   // setup debug console
        #ifdef DEBUG
   DigitalInputTmpl< PortD, 6 > rxDebug;
   DigitalOutputTmpl<PortD, 7> txDebug;
   Usart::instance<PortD, 1>().init<115200>();
   Logger::instance().setStream( debug );
        #endif

   // setup the serial for HmwStream, the txEnable/rxEnable lines have to be set in the special HW version
   DigitalInputTmpl< PortE, 2 > rxHmwStream;
   DigitalOutputTmpl<PortE, 3> txHmwStream;
   serial = &Usart::instance<PortE, 0>();
}

SIGNAL(USARTE0_RXC_vect)
{
   HmwStream::nextByteReceivedFromISR(Usart::instance<PortE,0>().readDataRegisterFromISR());
}