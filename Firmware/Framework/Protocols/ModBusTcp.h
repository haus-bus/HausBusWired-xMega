/*
 * ModBusTcp.h
 *
 *  Created on: 28.08.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Protocols_ModBusPkg_ModBusTcp_H
#define SwFramework_Protocols_ModBusPkg_ModBusTcp_H

#include <DefaultTypes.h>

class Event;

class IpConnection;

class IStream;

class ModBusTcp
{
   public:

      class Adu;

      class ReadRegister;

      class ReadStatus;

      class WriteRegister;

      static const uint16_t SERVER_PORT = 502;

      class Adu
      {
         public:

            class Header;

            enum RegisterMode
            {
               REG_READ, // Read register values and pass to protocol stack
               REG_WRITE // Update register values
            };

            enum FunctionCode
            {
               NONE,
               READ_COILS,
               READ_DISCRETE_INPUTS,
               READ_HOLDING_REGISTER,
               READ_INPUT_REGISTER,
               WRITE_SINGLE_COIL,
               WRITE_REGISTER,
               DIAG_READ_EXCEPTION,
               DIAG_DIAGNOSTIC,
               DIAG_GET_COM_EVENT_CNT = 11,
               DIAG_GET_COM_EVENT_LOG,
               WRITE_MULTIPLE_COILS = 15,
               WRITE_MULTIPLE_REGISTERS,
               OTHER_REPORT_SLAVEID,
               READWRITE_MULTIPLE_REGISTERS = 23,
               FUNCTION_ERROR = 0x80
            };

            enum Exception
            {
               EX_NONE,
               EX_ILLEGAL_FUNCTION,
               EX_ILLEGAL_DATA_ADDRESS,
               EX_ILLEGAL_DATA_VALUE,
               EX_SLAVE_DEVICE_FAILURE,
               EX_ACKNOWLEDGE,
               EX_SLAVE_BUSY,
               EX_MEMORY_PARITY_ERROR = 8,
               GATEWAY_PATH_FAILED = 10,
               GATEWAY_TGT_FAILED
            };

#define PDU_MAX_DATA_LENGTH 252

            class Header
            {
               ////    Operations    ////

               public:

                  uint16_t getLength() const;

                  uint16_t getProtocol() const;

                  uint16_t getTransaction() const;

                  void setLength( uint16_t p_length );

                  void setProtocol( uint16_t p_protocol );

                  void setTransaction( uint16_t p_transaction );

                  inline uint8_t getUnitId() const
                  {
                     return unitId;
                  }

                  inline void setUnitId( uint8_t p_unitId )
                  {
                     unitId = p_unitId;
                  }

                  ////    Attributes    ////

               protected:

                  uint16_t transaction;

                  uint16_t protocol;

                  uint16_t length;

                  uint8_t unitId;
            };

            ////    Operations    ////

            void setException( const Exception exception );

            ////    Additional operations    ////

            inline Header* getHeader() const
            {
               return (ModBusTcp::Adu::Header*) &header;
            }

            inline uint8_t getFunctionCode() const
            {
               return functionCode;
            }

            inline void setFunctionCode( uint8_t p_functionCode )
            {
               functionCode = p_functionCode;
            }

            inline uint8_t getData( int i1 ) const
            {
               return data[i1];
            }

            inline void setData( int i1, uint8_t p_data )
            {
               data[i1] = p_data;
            }

            ////    Attributes    ////

            Header header;

            uint8_t functionCode;

            uint8_t data[PDU_MAX_DATA_LENGTH];
      };

      class ReadRegister
      {
         ////    Operations    ////

         public:

            inline uint16_t calculateResponseDataLength();

            inline uint16_t calculateResponseLength();

            uint16_t* changeIntoResponse();

            uint16_t getAddress() const;

            uint16_t getLength() const;

            ////    Attributes    ////

         protected:

            uint16_t address;

            uint16_t length;
      };

      class ReadStatus
      {
         ////    Operations    ////

         public:

            uint16_t calculateResponseDataLength();

            inline uint16_t calculateResponseLength();

            uint8_t* changeIntoResponse();

            uint16_t getAddress() const;

            uint16_t getLength() const;

            ////    Attributes    ////

         protected:

            uint16_t address;

            uint16_t length;
      };

      class WriteRegister
      {
         ////    Operations    ////

         public:

            uint16_t getAddress() const;

            uint16_t getValue() const;

            ////    Attributes    ////

         protected:

            uint16_t address;

            uint16_t value;
      };

      class WriteMultipleRegister
      {
         ////    Operations    ////

         public:

            uint16_t getAddress() const;

            uint16_t getLength() const;

            uint16_t getData( uint8_t idx = 0 ) const;

            void changeIntoResponse()
            {
               memcpy( &byteCount, data, byteCount );
            }

            ////    Attributes    ////

         protected:

            uint16_t address;

            uint16_t length;

            uint8_t byteCount;

         private:

            uint16_t data[2];
      };

      class WriteMultipleCoils
      {
         ////    Operations    ////

         public:

            uint16_t getAddress() const;

            uint16_t getLength() const;

            const uint8_t* getData() const
            {
               return &data;
            }

            void changeIntoResponse()
            {
               memcpy( &byteCount, &data, byteCount );
            }


            ////    Attributes    ////

         protected:

            uint16_t address;

            uint16_t length;

            uint8_t byteCount;

         private:

            uint8_t data;
      };

      ////    Constructors and destructors    ////

      ModBusTcp();

      ////    Operations    ////

      bool notifyEvent( const Event& event );

      virtual Adu::Exception readRegister( ReadRegister* cmd, bool inputsOnly );

      virtual Adu::Exception readStatus( ReadStatus* cmd, bool inputsOnly );

      virtual Adu::Exception writeRegister( WriteRegister* cmd );

      virtual Adu::Exception writeMultipleRegister(
         ModBusTcp::WriteMultipleRegister* cmd );

      virtual Adu::Exception writeMultipleCoils(
         ModBusTcp::WriteMultipleCoils* cmd );

      ////    Additional operations    ////

   protected:

      inline static const uint8_t getDebugLevel()
      {
         return debugLevel;
      }

      ////    Attributes    ////

      static const uint8_t debugLevel;
};

inline uint16_t ModBusTcp::ReadRegister::calculateResponseDataLength()
{
   return getLength() << 1;
}

inline uint16_t ModBusTcp::ReadRegister::calculateResponseLength()
{
   return calculateResponseDataLength() + 3;
}

inline uint16_t ModBusTcp::ReadStatus::calculateResponseLength()
{
   return calculateResponseDataLength() + 3;
}

#endif
