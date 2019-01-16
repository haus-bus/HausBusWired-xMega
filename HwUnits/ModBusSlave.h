/*
 * ModBusSlave.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwUnits_ModBusSlave_H
#define SwUnits_ModBusSlave_H

#include "SwUnits.h"
#include "Gateway.h"
#include <Protocols/ModBusTcp.h>
class Button;

class Dimmer;

class Event;

class HACF;

class IResponse;

class IpConnection;

class Led;

class RollerShutter;

class Scheduler;

class IStream;

class ModBusSlave : public Gateway,
                    public ModBusTcp
{
   public:

      class Device;

      class DeviceInfo;

      enum Constants
      {
         MAX_DEVICES = 64,
         MAX_PORTS = 4,
         MAX_PINS = 8
      };

      struct RegisterAddress
      {
         uint16_t pin : 3;
         uint16_t port : 2;
         uint16_t device : 6;
      };

      enum ClassIds
      {
         UNKNOWN,
         DIGITAL_OUT,
         ANALOG_OUT,
         DIGITAL_IN = 8,
         ANALOG_IN
      };
      class Command
      {
         public:

            enum Commands
            {
               GET_CONFIGURATION = HACF::COMMANDS_START,
               SET_CONFIGURATION,
               READ_COIL_STATUS, // digital output states
               READ_INPUT_STATUS, // digital input state
               READ_HOLDING_REGISTERS, // read registers (counter, ad, configuration..)

            };

            struct ReadCoilStatus
            {
               uint16_t regId;
               uint8_t len;
            };

            struct ReadInputStatus
            {
               uint16_t regId;
               uint8_t len;
            };

            struct ReadHoldingRegisters
            {
               uint16_t regId;
               uint8_t len;
            };

            union Parameter
            {
               ReadCoilStatus readCoilStatus;
               ReadInputStatus readInputStatus;
               ReadHoldingRegisters readHoldingRegisters;
            };

            ////    Operations    ////

            inline Parameter& getParameter()
            {
               return parameter;
            }

            ////    Additional operations    ////

            inline uint8_t getCommand() const
            {
               return command;
            }

            inline void setCommand( uint8_t p_command )
            {
               command = p_command;
            }

            inline void setParameter( Parameter p_parameter )
            {
               parameter = p_parameter;
            }

            ////    Attributes    ////

            uint8_t command;

            Parameter parameter;
      };

      class Response : public IResponse
      {
         public:

            enum Responses
            {
               CONFIGURATION = HACF::RESULTS_START,
               COIL_STATUS, // digital output states
               INPUT_STATUS, // digital input state
               HOLDING_REGISTERS, // registers (counter, ad, configuration..)
            };

            union Parameter
            {
               uint16_t registers[2]; // if more data is needed, this will be allocated by the protocol
            };

            ////    Constructors and destructors    ////

            inline Response( uint16_t id ) :
               IResponse( id )
            {
            }

            inline Response( uint16_t id, const HACF& message ) :
               IResponse( id, message )
            {
            }

            ////    Operations    ////

            inline Parameter& getParameter()
            {
               return *reinterpret_cast<Parameter*>( IResponse::getParameter() );
            }

            Parameter& setConfiguration();

            ////    Attributes    ////

         private:

            Parameter params;
      };

      class Device
      {
         public:

            class Port
            {
               public:

                  struct Pin
                  {
                     uint16_t value : 12;
                     uint16_t id : 4;
                  };

                  ////    Operations    ////

                  Pin getPin( int i1 ) const;

                  void setPin( int i1, const Pin& p_pin );

                  ////    Additional operations    ////

                  inline void setPin( int i1, Pin p_pin )
                  {
                     pin[i1] = p_pin;
                  }

                  ////    Attributes    ////

                  Pin pin[MAX_PINS];
            };

            ////    Operations    ////

            int getPort() const;

            ////    Attributes    ////

            Port port[MAX_PORTS];

      };

      class DeviceInfo
      {
         ////    Operations    ////

         public:

            bool isAr8() const;

            void setAr8( bool p_ar8 );

            ////    Attributes    ////

            bool ar8 : 1;
      };

      ////    Constructors and destructors    ////

      ModBusSlave();

      ////    Operations    ////

      uint8_t convertToModBusClassId( uint8_t hacfClassId );

      int getDevice() const;

      int getDeviceInfo() const;

      bool isInput( uint8_t _id );

      virtual bool notifyEvent( const Event& event );

      inline void* operator new( size_t size );

      virtual ModBusTcp::Adu::Exception readRegister( ModBusTcp::ReadRegister* cmd,
                                                      bool inputsOnly );

      virtual ModBusTcp::Adu::Exception readStatus( ModBusTcp::ReadStatus* cmd,
                                                    bool inputsOnly );

      uint8_t readStatus( uint16_t registerId, uint8_t length, uint8_t* cmd );

      void run();

      void sendCommand( const RegisterAddress& regAddr );

      uint16_t updateValueFromMsg( const HACF& event, uint16_t value );

      virtual ModBusTcp::Adu::Exception writeRegister(
         ModBusTcp::WriteRegister* cmd );

      virtual ModBusTcp::Adu::Exception writeMultipleRegister(
         ModBusTcp::WriteMultipleRegister* cmd );

      virtual ModBusTcp::Adu::Exception writeMultipleCoils(
         ModBusTcp::WriteMultipleCoils* cmd );

   protected:

      uint8_t getDebugLevel();

      bool isClassIdSupported( uint8_t classId )
      {
         return ( classId == Object::ClassId::BUTTON )
                || ( classId == Object::ClassId::LED )
                || ( classId == Object::ClassId::DIGITAL_OUTPUT )
                || ( classId == Object::ClassId::TEMPERATURE );
      }

////    Attributes    ////

      static const uint8_t debugLevel;

////    Relations and components    ////

      Device device[MAX_DEVICES];

      DeviceInfo deviceInfo[MAX_DEVICES];

};

inline void* ModBusSlave::operator new( size_t size )
{
   return allocOnce( size );
}

#endif
