/*
 * EventSystem.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef Peripherals_EventSystem_H
#define Peripherals_EventSystem_H

#include "Peripherals.h"

class EventSystem
{
   public:

      static const uint8_t MAX_CHANNELS = 8;

      ////    Constructors and destructors    ////

   protected:

      inline EventSystem()
      {
      }

      ////    Operations    ////

   public:

      ///*! \brief This function sets the filter parameters for an event channel.
      // *
      // *  \param channel  The event channel number, range 0-7.
      // *  \param filter   Filter coefficient for the digital input filter.
      // */
      inline static void setEventChannelFilter( uint8_t channel, EVSYS_DIGFILT_t filter )
      {
         // Check if channel is valid and set the pointer offset for the selected
         // channel and assign the configuration value.
         if ( channel < MAX_CHANNELS )
         {
            volatile uint8_t* chCtrl = &EVSYS.CH0CTRL;
            chCtrl[channel] = filter;
         }
      }

      ///*! \brief This function sets the parameters for an event channel.
      // *
      // *  \note The quadrature decoder is only available on channel 0, 2 and 4.
      // *
      // *  \param channel      The event channel number, either 0, 2 or 4.
      // *  \param qdMode       Quadrature decoder index recognition mode.
      // *  \param qdndexEnable Enable quadrature decoder index.
      // *  \param qdnable      Enable Quadrature decoder.
      // *  \param filter       Filter coefficient for the digital input filter.
      // */
      inline static void setEventChannelParameters( uint8_t channel, EVSYS_QDIRM_t qdMode, bool qdIndexEnable, bool qdEnable, EVSYS_DIGFILT_t filter )
      {
         // Check if channel is valid and set the pointer offset for the selected
         // channel and assign the configuration value.
         if ( ( channel == 0 )
            || ( channel == 2 )
            || ( channel == 4 ) )
         {
            volatile uint8_t* chCtrl = &EVSYS.CH0CTRL;
            chCtrl[channel] = ( uint8_t ) qdMode
                              | filter
                              | ( qdIndexEnable ? EVSYS_QDIEN_bm : 0 )
                              | ( qdEnable ? EVSYS_QDEN_bm : 0 );
         }
      }

      ///*! \brief This function sets the event source for an event channel.
      // *
      // *  \param channel     The event channel number, range 0-7.
      // *  \param source      The event source to use as input to the MUX.
      // */
      inline static void setEventSource( uint8_t channel, EVSYS_CHMUX_t source )
      {
         // Check if channel is valid and set the pointer offset for the selected
         // channel and assign the eventSource value.
         if ( channel < MAX_CHANNELS )
         {
            volatile uint8_t* chMux = &EVSYS.CH0MUX;
            chMux[channel] = source;
         }
      }

      ///*! \brief This function sets the event data and strobe for a manual event trigger.
      // *
      // *  This function manually triggers events on the selected channels. The
      // *  "Manually Generating Events" section in the Xmega manual have a detailed
      // *  description of the events generated with the different combinations of the
      // *  bit settings.
      // *
      // *  \param dataMask   Bit mask for data events on the channel n, where the bit
      // *                    position n correspond to the channel n.
      // *  \param strobeMask Bit mask for strobe on the channel n, where the bit
      // *                    position n correspond to the channel n.
      // */
      inline static void triggerManual( uint8_t dataMask, uint8_t strobeMask )
      {
         // The datamask register must be set before the strobe register.
         EVSYS.DATA = dataMask;
         EVSYS.STROBE = strobeMask;
      }
};

#endif
