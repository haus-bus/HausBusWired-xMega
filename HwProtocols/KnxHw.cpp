/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: Xmega192A3
        Configuration   : debug
        Model Element	: KnxHw
   //!	Generated Date	: Tue, 24, Jun 2014
        File Path	: Xmega192A3/debug/HwProtocols/Knx/KnxHw.cpp
 *********************************************************************/

// ## auto_generated
#include "KnxHw.h"
// ## package HwProtocols::Knx

// ## class KnxHw
KnxHw::KnxHw( DigitalInput _digitalIn, DigitalOutput _digitalOut ) : digitalIn( _digitalIn ), digitalOut( _digitalOut )
{
   // #[ operation KnxHw(DigitalInput,DigitalOutput)
   // #]
}

void KnxHw::readByte()
{
   // #[ operation readByte()
   // #]
}

Stream::Status KnxHw::write( uint8_t* pData, uint16_t length )
{
   // #[ operation write(uint8_t,uint16_t)
   uint8_t* port = (uint8_t*)&digitalIn.getIoPort() + 8;
   uint8_t pin = digitalIn.getPin();

   // wait for the begin of the start bit, 5,1ms
   uint16_t remainingTime = delayBit( COUNT_DELAY_BIT_US( 5100 ), port, pin, 0 );
   if ( remainingTime )
   {
      return Stream::LOCKED;
   }

   // bus was at least 5,1ms IDLE try to send message
   while ( length-- )
   {
      Stream::Status status = writeByte( *pData++ );
      if ( status != Stream::SUCCESS )
      {
         return status;
      }
   }
   return Stream::SUCCESS;

   // #]
}

Stream::Status KnxHw::writeByte( uint8_t data )
{
   // #[ operation writeByte(uint8_t)

   return Stream::SUCCESS;
   // #]
}

/*********************************************************************
        File Path	: Xmega192A3/debug/HwProtocols/Knx/KnxHw.cpp
*********************************************************************/
