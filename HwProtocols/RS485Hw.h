/*
 * RS485Hw.h
 *
 *  Created on: 19.05.2016
 *      Author: viktor.pankraz
 */

#ifndef HWPROTOCOLS_RS485HW_H_
#define HWPROTOCOLS_RS485HW_H_

#include "HwProtocols.h"
#include "Peripherals/Usart.h"
#include "DigitalOutput.h"
#include <IoStream.h>

class RS485Hw: public IoStream
{
public:

  ////    Constructors and destructors    ////

  inline RS485Hw( Usart& _usart, DigitalOutput _enableRx,
                  DigitalOutput _enableTx ) :
      owner( NULL ), transmitBuffer( NULL ), receiveBuffer( NULL ),
      receiveBufferSize( 0 ), maxReceiveBufferSize( 0 ), usart( _usart ),
      disableRx( _enableRx ), enableTx( _enableTx )
  {

  }
  ////    Operations    ////

public:

  virtual Stream::Status genericCommand( Command command, void* buffer );

  virtual Stream::Status read( void * pData, uint16_t length,
                               EventDrivenUnit* user = 0 );

  virtual Stream::Status write( void * pData, uint16_t length,
                                EventDrivenUnit* user = 0 );

  void handleDataRegisterEmpty();

  void handleTransmitFinished();

  void handleDataReceived();

protected:

  bool init();

  inline void disableReceiver()
  {
    disableRx.set();
  }

  inline void enableReceiver()
  {
    disableRx.clear();
  }

  inline bool isReceiverEnabled()
  {
    return !disableRx.isSet();
  }

  inline void disableTransmitter()
  {
    enableTx.clear();
  }

  inline void enableTransmitter()
  {
    enableTx.set();
  }

  inline bool isTransmitterEnabled()
  {
    return enableTx.isSet();
  }

private:

  ////    Attributes    ////

protected:

  EventDrivenUnit* owner;

  uint8_t* transmitBuffer;

  uint8_t* receiveBuffer;

  uint16_t receiveBufferSize;

  uint16_t maxReceiveBufferSize;

  Usart& usart;

  DigitalOutput disableRx;

  DigitalOutput enableTx;

  static const uint8_t debugLevel;

};

#endif /* HWPROTOCOLS_RS485HW_H_ */
