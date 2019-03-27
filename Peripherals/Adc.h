/*
 * Adc.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_Adc_H
#define Peripherals_Adc_H

#include "Peripherals.h"
#include "NvmController.h"

#include <assert.h>

class Adc
{
   public:

      class Channel
      {
         public:

            ////    Constructors and destructors    ////

         protected:

            inline Channel()
            {
            }

            ////    Operations    ////

         public:

            ///**
            // * \brief Disable interrupts on ADC channel
            // */
            inline void disableInterrupt()
            {
               reg.INTCTRL &= ~ADC_CH_INTLVL_gm;
               reg.INTCTRL |= ADC_CH_INTLVL_OFF_gc;
            }

            ///**
            // * \brief Enable interrupts on ADC channel
            // *
            // * \param level
            // * \arg \c ADC_CH_INTLVL_OFF_gc  : Interrupt disabled
            // * \arg \c ADC_CH_INTLVL_LO_gc   : Low level
            // * \arg \c ADC_CH_INTLVL_MED_gc  : Medium level
            // * \arg \c ADC_CH_INTLVL_HI_gc   : High level
            // */
            inline void enableInterrupt( ADC_CH_INTLVL_t level = ADC_CH_INTLVL_LO_gc )
            {
               reg.INTCTRL &= ~ADC_CH_INTLVL_gm;
               reg.INTCTRL |= level;
            }

            ///**
            // * \brief Get result from ADC channel
            // *
            // * Gets the latest conversion result from the ADC channel,
            // * with interrupt protection of the 16-bit read.
            // *
            // * \return Latest conversion result of ADC channel. Signedness does not matter.
            // */
            inline uint16_t getResult()
            {
               uint16_t value;
               {
                  CriticalSection doNotInterrupt;
                  value = reg.RES;
               }
               return value;
            }

            ///**
            // * \brief Set ADC channel interrupt mode
            // *
            // * \param mode Interrupt mode to set.
            // * \arg \c ADC_CH_INTMODE_COMPLETE_gc : Interrupt on conversion complete
            // * \arg \c ADC_CH_INTMODE_BELOW_gc    : Interrupt on result below compare value
            // * \arg \c ADC_CH_INTMODE_ABOVE_gc    : Interrupt on result above compare value
            // */
            inline void setInterruptMode( ADC_CH_INTMODE_t mode
                                             = ADC_CH_INTMODE_COMPLETE_gc )
            {
               reg.INTCTRL &= ~ADC_CH_INTMODE_gm;
               reg.INTCTRL |= mode;
            }

            ///**
            // * \brief Set ADC channel input mode, multiplexing and gain
            // *
            // * Sets up an ADC channel's input mode and multiplexing according to specified
            // * input signals, as well as the gain.
            // *
            // * \param pos Positive input signal.
            // * \arg \c ADC_CH_MUXPOS_PINn_gc , where \c n specifies a pin, for differential measurements.
            // * \param neg Negative input signal:
            // * \arg \c ADC_CH_MUXNEG_PINn_gc , where \c n specifies a pin, for differential measurements.
            // * \param gain Gain factor for measurements:
            // * \arg ADC_CH_GAIN_1X_gc for single-ended or differential with pin 0, 1, 2 or 3 as negative input.
            // * \arg ADC_CH_GAIN_1X_gc
            // * \arg ADC_CH_GAIN_2X_gc
            // * \arg ADC_CH_GAIN_4X_gc
            // * \arg ADC_CH_GAIN_8X_gc
            // * \arg ADC_CH_GAIN_16X_gc
            // * \arg ADC_CH_GAIN_32X_gc
            // * \arg ADC_CH_GAIN_64X_gc
            // * for differential with pin 4, 5, 6 or 7 as negative input.
            // */
            inline void setupInput( ADC_CH_INPUTMODE_t mode, uint8_t posPinNumber, uint8_t negPinNumber,
                                    ADC_CH_GAIN_t gain )
            {
               // convert into right type
               posPinNumber = ( posPinNumber & 0xF ) << 3;
               negPinNumber = ( negPinNumber & 0x7 );

               // Configure for internal input or single-ended measurement
               if ( ( mode == ADC_CH_INPUTMODE_INTERNAL_gc )
                  || ( mode == ADC_CH_INPUTMODE_SINGLEENDED_gc ) )
               {
                  assert( gain == ADC_CH_GAIN_1X_gc );
                  reg.CTRL = mode | ADC_CH_GAIN_1X_gc;
                  reg.MUXCTRL = posPinNumber;
               }
               // Configure for differential measurement
               else
               {
                  // Pins 0-3 can only be used for negative input if the gain
                  // stage is not used, i.e., unity gain.
                  if ( negPinNumber <= ADC_CH_MUXNEG_PIN3_gc )
                  {
                     assert( gain == ADC_CH_GAIN_1X_gc );
                     reg.CTRL = mode | ADC_CH_GAIN_1X_gc;
                     reg.MUXCTRL = posPinNumber | negPinNumber;
                  }
                  else
                  {
                     // Pins 4-7 can be used for all gain settings, including unity
                     // gain, which is available even if the gain stage is active.
                     reg.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | gain;
                     reg.MUXCTRL = posPinNumber | negPinNumber;
                  }
               }
            }

            inline void startConversion()
            {
               reg.CTRL |= ADC_CH_START_bm;
            }

            inline void waitForConversionFinished()
            {
               while ( !reg.INTFLAGS )
               {
               }
               reg.INTFLAGS = ADC_CH_CHIF_bm;
            }
            ////    Attributes    ////

         protected:

            ADC_CH_t reg;
      };

      ////    Constructors and destructors    ////

   protected:

      inline Adc()
      {
      }

      ////    Operations    ////

   public:

      ///**
      // * \brief Check if the ADC is enabled
      // *
      // * \retval true if ADC is enabled.
      // * \retval false if ADC is disabled.
      // */
      bool isEnabled();

      ///**
      // * \brief Clear interrupt flag of ADC channel(s)
      // *
      // * \param channels Mask of ADC channel(s):
      // * \arg \c ADC_CHnIF_bm , where \c n specifies the channel. (These can be OR'ed together.)
      // *
      // * \note The ADC must be enabled for this function to have any effect.
      // */
      inline void clearInterrupts( uint8_t channels )
      {
         reg.INTFLAGS = channels;
      }

      ///**
      // * \brief Disable ADC
      // *
      // * Disables the ADC and unlocks IDLE mode for the sleep manager.
      // */
      inline void disable()
      {
         {
            CriticalSection doNotInterrupt;
            reg.CTRLA &= ~ADC_ENABLE_bm;
            disableClock();
         }
         // TODO sleepmgr_unlock_mode(SLEEPMGR_IDLE);
      }

      ///**
      // * \brief Disable internal ADC input
      // *
      // * \param intputs Internal inputs to disable:
      // * \arg \c ADC_TEMPREF_bm for temperature sensor.
      // * \arg \c ADC_BANDGAP_bm for bandgap reference.
      // */
      inline void disableInternalInput( uint8_t inputs )
      {
         inputs &= ~( ADC_TEMPREF_bm | ADC_BANDGAP_bm );
         reg.REFCTRL &= ~inputs;
      }

      ///**
      // * \brief Enable ADC
      // *
      // * Enables the ADC and locks IDLE mode for the sleep manager.
      // *
      // * \note After enabling the ADC, one dummy conversion should be done to allow
      // * for its common mode voltage to settle.
      // */
      inline void enable()
      {
         {
            CriticalSection doNotInterrupt;
            enableClock();
            reg.CTRLA |= ADC_ENABLE_bm;
         }
         // TODO sleepmgr_lock_mode(SLEEPMGR_IDLE);
      }

      ///**
      // * \brief Enable internal ADC input
      // *
      // * \param intputs Internal inputs to enable:
      // * \arg \c ADC_TEMPREF_bm for temperature sensor.
      // * \arg \c ADC_BANDGAP_bm for bandgap reference.
      // */
      inline void enableInternalInput( uint8_t inputs )
      {
         inputs &= ( ADC_TEMPREF_bm | ADC_BANDGAP_bm );
         reg.REFCTRL |= inputs;
      }

      ///**
      // * \brief Flush the ADC
      // *
      // * Forces the ADC to abort any ongoing conversions and restart its clock on the
      // * next peripheral clock cycle. Pending conversions are started after the
      // * clock reset.
      // *
      // * \note The ADC must be enabled for this function to have any effect.
      // */
      inline void flush()
      {
         CriticalSection doNotInterrupt;
         reg.CTRLA |= ADC_FLUSH_bm;
      }

      inline uint16_t getFacktoryCalibration()
      {
         uint16_t data = 0;

#if defined( ADCA ) || defined( ADCB )
         uint8_t offset
            = reinterpret_cast<uint16_t>( &( (NVM_PROD_SIGNATURES_t*) 0 )->ADCACAL0 );
#if defined( ADCB )
         if ( this == reinterpret_cast<Adc*>( &ADCB ) )
         {
            offset
               = reinterpret_cast<uint16_t>( &( (NVM_PROD_SIGNATURES_t*) 0 )->ADCBCAL0 );
         }
#endif
         data = NvmController::readCalibrationByte( offset + 1 );
         data <<= 8;
         data |= NvmController::readCalibrationByte( offset );
#endif
         return data;
      }

      inline void resetToFactoryCalibration()
      {
         CriticalSection doNotInterrupt;
         reg.CAL = getFacktoryCalibration();
      }

      ///**
      // * \brief Get compare value directly from ADC
      // *
      // * Gets the compare value directly from the ADC, with interrupt
      // * protection of the 16-bit read, for quick access while the ADC is enabled.
      // *
      // * \return Current compare value of the ADC. Signedness does not matter.
      // */
      inline uint16_t getCompareValue()
      {
         uint16_t value;
         {
            CriticalSection doNotInterrupt;
            value = reg.CMP;
         }
         return value;
      }

      ///**
      // * \brief Get interrupt flag of ADC channel(s)
      // *
      // * Returns the interrupt flag of the masked channels. The meaning of the
      // * interrupt flag depends on what mode the individual channels are in.
      // *
      // * \param channels Mask of ADC channel(s):
      // * \arg \c ADC_CHnIF_bm , where \c n specifies the channel. (These can be OR'ed together.)
      // *
      // * \return Mask with interrupt flags.
      // */
      inline uint8_t getInterrupts( uint8_t channels = 0xFF )
      {
         return reg.INTFLAGS & channels;
      }

      inline Channel& getChannel( uint8_t channel = 0 )
      {
         switch ( channel )
         {
            case 0:
               return *reinterpret_cast<Adc::Channel*>( &reg.CH0 );

            case 1:
               return *reinterpret_cast<Adc::Channel*>( &reg.CH1 );

            case 2:
               return *reinterpret_cast<Adc::Channel*>( &reg.CH2 );

            case 3:
               return *reinterpret_cast<Adc::Channel*>( &reg.CH3 );
         }
         fatalErrorLoop();
         return *(Adc::Channel*) 0;
      }

      inline static uint16_t getTemperatureCalibration()
      {
         uint16_t data = 0;

#if defined( ADCA ) || defined( ADCB )
         data
            = NvmController::readCalibrationByte(
            reinterpret_cast<uint16_t>( &( (NVM_PROD_SIGNATURES_t*) 0 )->TEMPSENSE1 ) );
         data <<= 8;
         data
            |= NvmController::readCalibrationByte(
            reinterpret_cast<uint16_t>( &( (NVM_PROD_SIGNATURES_t*) 0 )->TEMPSENSE0 ) );
#endif
         return data;
      }

      static Adc& instance( uint8_t portNumber );

      template<uint32_t rate>
      inline void setClockRate()
      {
         assert( rate );
         assert( rate <= F_CPU );

         uint16_t ratio = F_CPU / rate;

         // Round ratio up to the nearest prescaling factor.
         if ( ratio <= 4 )
         {
            reg.PRESCALER = ADC_PRESCALER_DIV4_gc;
         }
         if ( ratio <= 8 )
         {
            reg.PRESCALER = ADC_PRESCALER_DIV8_gc;
         }
         if ( ratio <= 16 )
         {
            reg.PRESCALER = ADC_PRESCALER_DIV16_gc;
         }
         if ( ratio <= 32 )
         {
            reg.PRESCALER = ADC_PRESCALER_DIV32_gc;
         }
         if ( ratio <= 64 )
         {
            reg.PRESCALER = ADC_PRESCALER_DIV64_gc;
         }
         if ( ratio <= 256 )
         {
            reg.PRESCALER = ADC_PRESCALER_DIV256_gc;
         }
         else
         {
            reg.PRESCALER = ADC_PRESCALER_DIV512_gc;
         }
      }

      ///**
      // * \brief Set compare value directly to ADC
      // *
      // * Sets the compare value directly to the ADC, for quick access while the ADC is
      // * enabled.
      // *
      // * \param value Compare value to set, either signed or unsigned.
      // *
      // * \note The ADC must be enabled for this function to have any effect.
      // */
      inline void setCompareValue( uint16_t value )
      {
         CriticalSection doNotInterrupt;
         reg.CMP = value;
      }

      ///**
      // * \brief Set ADC conversion parameters
      // *
      // * Sets the signedness, resolution and voltage reference for conversions in the
      // * ADC module configuration.
      // *
      // * \param sign Conversion signedness.
      // * \param resolution Resolution of conversions.
      // * \param reference Voltage reference to use.
      // */
      template<bool sign, ADC_RESOLUTION_t resolution, ADC_REFSEL_t reference>
      inline void setConversionParameter()
      {
         // Preserve all but conversion and resolution config.
         reg.CTRLB &= ~( ADC_CONMODE_bm | ADC_RESOLUTION_gm );
         reg.CTRLB |= resolution | ( sign ? ADC_CONMODE_bm : 0 );

         reg.REFCTRL &= ~ADC_REFSEL_gm;
         reg.REFCTRL |= reference;
      }

      ///**
      // * \brief Set ADC conversion trigger
      // *
      // * Configures the conversion triggering of the ADC.
      // *
      // * For automatic triggering modes, the number of channels to start conversions
      // * on must be specified with \a nr_of_ch. The channel selection for these
      // * modes is incrementally inclusive, always starting with channel 0.
      // *
      // * For event triggered modes, the base event channel must also be specified.
      // * The event channels are assigned to the ADC channels in an
      // * incremental fashion \a without \a wrap-around (in single-trigger event mode).
      // * This means that the maximum base event channel that can be used is determined
      // * by the number of ADC channels to start conversions on.
      // *
      // * \param sweep
      // * \param evSel Base event channel, if used.
      // * \param evAct Conversion trigger to set.
      // */
      template<ADC_SWEEP_t sweep, ADC_EVSEL_t evSel, ADC_EVACT_t evAct>
      inline void setConversionTrigger()
      {
         reg.EVCTRL = sweep | evSel | evAct;
      }

      ///**
      // * \brief Set DMA request group
      // *
      // * Configures the DMA group request for the specified number of ADC channels.
      // * The channel group selection is incrementally inclusive, always starting with
      // * channel 0.
      // *
      // * \param dma DMA request selection:
      // * \arg ADC_DMASEL_OFF_gc to disable.
      // * \arg ADC_DMASEL_CH01_gc,
      // * \arg ADC_DMASEL_CH012_gc or
      // * \arg ADC_DMASEL_CH0123_gc to enable.
      // */
      inline void setDmaRequestGroup( ADC_DMASEL_t dma )
      {
         reg.CTRLA |= dma;
      }

      ///**
      // * \brief Start one-shot conversion on ADC channel(s)
      // *
      // * \param channels Mask of ADC channel(s):
      // * \arg \c ADC_CHnIF_bm , where \c n specifies the channel. (These can be OR'ed together.)
      // *
      // * \note The ADC must be enabled for this function to have any effect.
      // */
      inline void startConversion( uint8_t channelMask )
      {
         CriticalSection doNotInterrupt;
         reg.CTRLA |= channelMask << ADC_CH0START_bp;
      }

      ///**
      // * \brief Wait for interrupt flag of ADC channel(s)
      // *
      // * Waits for the interrupt flag of the specified channel(s) to be set, then
      // * clears it before returning. If several channels are masked, the function will
      // * wait for \a all interrupt flags to be set.
      // *
      // * \param channels Mask of ADC channel(s):
      // * \arg \c ADC_CHnIF_bm , where \c n specifies the channel. (These can be OR'ed together.)
      // */
      inline void waitForInterrupts( uint8_t channels )
      {
         while ( getInterrupts( channels ) != channels )
         {
         }
         clearInterrupts( channels );
      }

   private:

      ///**
      // * \internal
      // * \brief Disable peripheral clock for ADC
      // *
      // * Decrements the enable count for the ADC, then disables its peripheral clock
      // * if the count hit zero. If the count did not hit zero, it indicates the ADC is
      // * enabled.
      // */
      void disableClock();

      ///**
      // * \internal
      // * \brief Enable peripheral clock for ADC
      // *
      // * Checks if the enable count for the ADC is zero, then increments it. If the
      // * count was zero, the peripheral clock is enabled. Otherwise, it is already
      // * enabled.
      // */
      void enableClock();

      ////    Attributes    ////

   protected:

      ADC_t reg;

};

#endif
