/*
 * IrDecoder.h
 *
 *  Created on: 24.06.2014
 *      Author: viktor.pankraz
 */

#ifndef HwProtocols_IrDecoder_H
#define HwProtocols_IrDecoder_H

#include "Protocols.h"

class IrDecoder
{
   public:

      class NEC;

      class RC5;

      /*---------------------------------------------------------------------------------------------------------------------------------------------------
       * Change settings from 1 to 0 if you want to disable one or more decoders.
       * This saves program space.
       *
       * 1 enable  decoder
       * 0 disable decoder
       *
       * The standard decoders are enabled per default.
       * Some less common protocols are disabled here, you need to enable them manually.
       *
       * If you want to use FDC or RCCAR simultaneous with RC5 protocol, additional program space is required.
       * If you don't need RC5 when using FDC/RCCAR, you should disable RC5.
       *---------------------------------------------------------------------------------------------------------------------------------------------------
       */
      // Protocol                        Enable     Remarks
    #define SUPPORT_SIRCS_PROTOCOL             0       // Sony SIRCS
    #define SUPPORT_NEC_PROTOCOL               0       // NEC + APPLE
    #define SUPPORT_SAMSUNG_PROTOCOL           0       // Samsung + Samsung32
    #define SUPPORT_MATSUSHITA_PROTOCOL        0       // Matsushita
    #define SUPPORT_KASEIKYO_PROTOCOL          0       // Kaseikyo
    #define SUPPORT_DENON_PROTOCOL             0       // DENON, Sharp
    #define SUPPORT_JVC_PROTOCOL               0       // JVC
    #define SUPPORT_RC5_PROTOCOL               1       // RC5
    #define SUPPORT_RC6_PROTOCOL               0       // RC6 & RC6A
    #define SUPPORT_GRUNDIG_PROTOCOL           0       // Grundig
    #define SUPPORT_NOKIA_PROTOCOL             0       // Nokia
    #define SUPPORT_NUBERT_PROTOCOL            0       // NUBERT
    #define SUPPORT_BANG_OLUFSEN_PROTOCOL      0       // Bang & Olufsen
    #define SUPPORT_NIKON_PROTOCOL             0       // NIKON
    #define SUPPORT_FDC_PROTOCOL               0       // FDC3402 keyboard
    #define SUPPORT_RCCAR_PROTOCOL             0       // RC Car
    #define SUPPORT_SIEMENS_PROTOCOL           0       // Siemens Gigaset
    #define SUPPORT_RECS80_PROTOCOL            0       // RECS80
    #define SUPPORT_RECS80EXT_PROTOCOL         0       // RECS80EXT

    #if SUPPORT_RC5_PROTOCOL == 1 || SUPPORT_RC6_PROTOCOL == 1 || SUPPORT_GRUNDIG_OR_NOKIA_PROTOCOL == 1 || SUPPORT_SIEMENS_PROTOCOL == 1
    #define SUPPORT_MANCHESTER                 1
    #else
    #define SUPPORT_MANCHESTER                 0
    #endif

    #ifndef IR_TIMER_BASE
    #define IR_TIMER_BASE                     32       // µs
    #endif



      static const uint16_t MIN_IDLE_TIME = 40000UL / IR_TIMER_BASE;
      static const uint16_t MAX_REPETITION_DELAY = 150000UL / IR_TIMER_BASE;
      static const uint16_t TIMEOUT = 250000UL / IR_TIMER_BASE;

      static const uint8_t SIRCS_PROTOCOL = 1;       // Sony
      static const uint8_t NEC_PROTOCOL = 2;         // NEC, Pioneer, JVC, Toshiba, NoName etc.
      static const uint8_t SAMSUNG_PROTOCOL = 3;     // Samsung
      static const uint8_t MATSUSHITA_PROTOCOL = 4;  // Matsushita
      static const uint8_t KASEIKYO_PROTOCOL = 5;    // Kaseikyo (Panasonic etc)
      static const uint8_t RECS80_PROTOCOL = 6;      // Philips, Thomson, Nordmende, Telefunken, Saba
      static const uint8_t RC5_PROTOCOL = 7;         // Philips etc
      static const uint8_t DENON_PROTOCOL = 8;       // Denon
      static const uint8_t RC6_PROTOCOL = 9;         // Philips etc
      static const uint8_t SAMSUNG32_PROTOCOL = 10;  // Samsung32: no sync pulse at bit 16, length 32 instead of 37
      static const uint8_t APPLE_PROTOCOL = 11;      // Apple, very similar to NEC
      static const uint8_t RECS80EXT_PROTOCOL = 12;  // Philips, Technisat, Thomson, Nordmende, Telefunken, Saba
      static const uint8_t NUBERT_PROTOCOL = 13;     // Nubert
      static const uint8_t BANG_OLUFSEN_PROTOCOL = 14; // Bang & Olufsen
      static const uint8_t GRUNDIG_PROTOCOL = 15;    // Grundig
      static const uint8_t NOKIA_PROTOCOL = 16;      // Nokia
      static const uint8_t SIEMENS_PROTOCOL = 17;    // Siemens, e.g. Gigaset
      static const uint8_t FDC_PROTOCOL = 18;        // FDC keyboard
      static const uint8_t RCCAR_PROTOCOL = 19;      // RC Car
      static const uint8_t JVC_PROTOCOL = 20;        // JVC
      static const uint8_t RC6A_PROTOCOL = 21;       // RC6A, e.g. Kathrein, XBOX
      static const uint8_t NIKON_PROTOCOL = 22;      // Nikon

      struct Parameter {
         uint16_t address;
         uint16_t command;
         uint8_t protocol;
         uint8_t bitPosition;
      };

      struct Data {
         uint16_t address;
         uint16_t command;
         uint8_t repetitionFrameNumber;
      };

      class RC5
      {
         public:

            static const uint8_t BIT_TIME = 889 / IR_TIMER_BASE;
            static const uint8_t BIT_LEN_MIN = BIT_TIME * 0.8;
            static const uint8_t BIT_LEN_MAX = BIT_TIME * 1.2;

            static const uint8_t COMPLETE_LENGTH = 14;
            static const uint8_t FRAME_REPEAT_PAUSE_TIME = 90; // frame repeat after 45ms


            struct Protocol {
               uint16_t command : 6;
               uint16_t address : 5;
               uint16_t toggle : 1;
               uint16_t startOrCmdBit6 : 1;
               uint16_t start : 1;
            };

            ////    Operations    ////

            inline static bool isStartBit( uint16_t pulseTime, uint16_t& pauseTime, Parameter& param );

            inline static bool notifyEdge( uint8_t rising, uint16_t t1, uint16_t& t2, Parameter& param );
      };

      class NEC
      {
         public:

            static const uint16_t START_BIT_PULSE_TIME = 9000 / IR_TIMER_BASE;
            static const uint8_t START_BIT_PAUSE_TIME = 4500 / IR_TIMER_BASE;
            static const uint8_t REPEATED_START_BIT_PAUSE_TIME = 2250 / IR_TIMER_BASE;

            static const uint16_t START_BIT_PULSE_LEN_MIN = START_BIT_PULSE_TIME * 0.6;
            static const uint16_t START_BIT_PULSE_LEN_MAX = START_BIT_PULSE_TIME * 1.4;
            static const uint16_t START_BIT_PAUSE_LEN_MIN = START_BIT_PAUSE_TIME * 0.6;
            static const uint16_t START_BIT_PAUSE_LEN_MAX = START_BIT_PAUSE_TIME * 1.4;
            static const uint8_t REPEATED_START_BIT_PAUSE_LEN_MIN = REPEATED_START_BIT_PAUSE_TIME * 0.6;
            static const uint8_t REPEATED_START_BIT_PAUSE_LEN_MAX = REPEATED_START_BIT_PAUSE_TIME * 1.4;

            static const uint8_t PULSE_TIME = 560 / IR_TIMER_BASE;
            static const uint8_t HPAUSE_TIME = 1690 / IR_TIMER_BASE;
            static const uint8_t LPAUSE_TIME = 560 / IR_TIMER_BASE;

            static const uint8_t HPULSE_MIN = PULSE_TIME * 0.6;
            static const uint8_t HPULSE_MAX = PULSE_TIME * 1.4;
            static const uint8_t HPAUSE_MIN = HPAUSE_TIME * 0.6;
            static const uint8_t HPAUSE_MAX = HPAUSE_TIME * 1.4;
            static const uint8_t LPULSE_MIN = HPULSE_MIN;
            static const uint8_t LPULSE_MAX = HPULSE_MAX;
            static const uint8_t LPAUSE_MIN = LPAUSE_TIME * 0.6;
            static const uint8_t LPAUSE_MAX = LPAUSE_TIME * 1.4;

            static const uint8_t ADDRESS_OFFSET = 0;         // skip 2 bits (2nd start + 1 toggle)
            static const uint8_t ADDRESS_LEN = 16;           // read 5 address bits
            static const uint8_t COMMAND_OFFSET = 16;        // skip 5 bits (2nd start + 1 toggle + 5 address)
            static const uint8_t COMMAND_LEN = 16;           // read 6 command bits
            static const uint8_t COMPLETE_DATA_LEN = 32;     // complete length
            static const uint8_t STOP_BIT = 1;               // has no stop bit
            static const uint8_t LSB = 1;                    // MSB...LSB
            static const uint8_t IS_MANCHESTER = 0;          // ManchesterCode
            static const uint8_t FIRST_BIT_IS_1 = 0;

            static const uint8_t FRAME_REPEAT_PAUSE_TIME = 40; // frame repeat after 40ms


            ////    Operations    ////

            inline static bool isStartBit( uint16_t pulseTime, uint16_t pauseTime );
      };

      ////    Constructors and destructors    ////

      IrDecoder();

      ////    Operations    ////

      bool getData( Data& data );

      void notifyEdge( uint8_t bit, uint16_t time );

      inline bool notifyTimeout();

      ////    Attributes    ////

   private:

      uint8_t irDetected : 1;

      uint8_t startBitDetected : 1;

      uint8_t waitForStartSpace : 1;

      uint8_t repetitionFrame : 1;

      Data lastData;

      uint16_t lastTime;

      static const uint8_t debugLevel;

      Parameter parameter;
};

inline bool IrDecoder::notifyTimeout()
{
   if ( startBitDetected )
   {
      startBitDetected = 0;
      ERROR_1( "IR timeout" );
   }
   else if ( lastData.repetitionFrameNumber )
   {
      lastData.repetitionFrameNumber = 0;
      return true;
   }
   return false;
}

inline bool IrDecoder::RC5::isStartBit( uint16_t pulseTime, uint16_t& pauseTime, IrDecoder::Parameter& param )
{
    #if SUPPORT_RC5_PROTOCOL == 1
   if ( ( pulseTime >= BIT_LEN_MIN ) && ( pulseTime <= BIT_LEN_MAX )
      && ( pauseTime >= BIT_LEN_MIN ) && ( pauseTime <= BIT_LEN_MAX ) )
   {
      param.protocol = RC5_PROTOCOL;
      param.command = 3;
      param.bitPosition = 2;
      pauseTime = 0;
      return true;
   }
   else if ( ( pulseTime >= 2 * BIT_LEN_MIN ) && ( pulseTime <= 2 * BIT_LEN_MAX ) )
   {
      if ( ( pauseTime >= BIT_LEN_MIN ) && ( pauseTime <= BIT_LEN_MAX ) )
      {
         param.protocol = RC5_PROTOCOL;
         param.command = 2;
         param.bitPosition = 2;
         return true;
      }
      else if ( ( pauseTime >= 2 * BIT_LEN_MIN ) && ( pauseTime <= 2 * BIT_LEN_MAX ) )
      {
         param.protocol = RC5_PROTOCOL;
         param.command = 5;
         param.bitPosition = 3;
         pauseTime = 0;
         return true;
      }
   }
    #endif
   return false;
}

inline bool IrDecoder::RC5::notifyEdge( uint8_t rising, uint16_t t1, uint16_t& t2, IrDecoder::Parameter& param )
{
    #if SUPPORT_RC5_PROTOCOL == 1
   if ( ( t2 < BIT_LEN_MIN )
      || ( ( t2 > BIT_LEN_MAX ) && ( t2 < 2 * BIT_LEN_MIN ) )
      || ( t2 > 2 * BIT_LEN_MAX ) )
   {
      param.bitPosition = 0;
      return false;
   }

   if ( param.bitPosition == COMPLETE_LENGTH )
   {
      return true;
   }

   t1 += t2;
   if ( ( t1 >= 2 * BIT_LEN_MIN ) && ( t1 <= 3 * BIT_LEN_MAX ) )
   {
      t2 = 0;
      param.command <<= 1;
      if ( !rising )
      {
         param.command |= 1;
      }
      if ( ++param.bitPosition == COMPLETE_LENGTH )
      {
         Protocol* data = reinterpret_cast<Protocol*>( &param.command );
         param.address = data->address;
         param.command = ( data->startOrCmdBit6 ) ? data->command : ( data->command | ( 1 << 6 ) );
         if ( rising )
         {
            return true;
         }
      }
   }

    #endif
   return false;
}

inline bool IrDecoder::NEC::isStartBit( uint16_t pulseTime, uint16_t pauseTime )
{
    #if SUPPORT_NEC_PROTOCOL == 1
   if ( pulseTime >= START_BIT_PULSE_LEN_MIN && pulseTime <= START_BIT_PULSE_LEN_MAX
      && pauseTime >= START_BIT_PAUSE_LEN_MIN && pauseTime <= START_BIT_PAUSE_LEN_MAX )
   {
      return true;
   }
    #endif
   return false;
}

#endif
