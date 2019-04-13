/*
 * KnxHw.cpp
 *
 * Created: 18.06.2014 14:12:55
 * Author: viktor.pankraz
 */

#include <Protocols/KnxHw.h>

KnxHw::KnxHw( DigitalInput _digitalIn, DigitalOutput _digitalOut ) : digitalIn( _digitalIn ), digitalOut( _digitalOut )
{
}

void KnxHw::readByte()
{
}

IStream::Status KnxHw::write( uint8_t* pData, uint16_t length )
{
   uint8_t* port = (uint8_t*)&digitalIn.getIoPort() + 8;
   uint8_t pin = digitalIn.getPin();

   // wait for the begin of the start bit, 5,1ms
   uint16_t remainingTime = delayBit( COUNT_DELAY_BIT_US( 5100 ), port, pin, 0 );
   if ( remainingTime )
   {
      return IStream::LOCKED;
   }

   // bus was at least 5,1ms IDLE try to send message
   while ( length-- )
   {
      IStream::Status status = writeByte( *pData++ );
      if ( status != IStream::SUCCESS )
      {
         return status;
      }
   }
   return IStream::SUCCESS;

}

IStream::Status KnxHw::writeByte( uint8_t data )
{

   return IStream::SUCCESS;
}
