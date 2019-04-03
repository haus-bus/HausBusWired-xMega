/*
 * HmwDeviceHw.h
 *
 * Created: 09.01.2018 17:47:15
 * Author: viktor.pankraz
 */


#ifndef __HMWDEVICEHW_H__
#define __HMWDEVICEHW_H__

#include "HmwStreamHw.h"
#include "HmwLinkSender.h"
#include "HmwLinkReceiver.h"

#include <xEeprom.h>

class HmwDeviceHw : public HmwStreamHw
{
// functions
   public:
      struct BasicConfig
      {
         uint8_tx hwVersion;                          // 0x0000
         uint8_tx loggingTime;                        // 0x0001
         uint32_tx centralAddress;                    // 0x0002   - 0x0005
         uint32_tx ownAddress;                        // 0x0006   - 0x0009
      };

      enum ConfigButtonState
      {
         IDLE,
         FIRST_PRESS,
         FIRST_LONG_PRESS,
         WAIT_SECOND_PRESS,
         SECOND_PRESS,
         SECOND_LONG_PRESS
      };

      virtual BasicConfig* getBasicConfig() = 0;

      virtual inline bool isConfigButtonPressed()
      {
         return false;
      }

      virtual inline void notifyConfigButtonState( ConfigButtonState state )
      {
      }

}; // HmwDeviceHw

#endif // __HMWDEVICEHW_H__
