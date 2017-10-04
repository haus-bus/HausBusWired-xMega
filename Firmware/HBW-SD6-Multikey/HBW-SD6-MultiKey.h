/*
 * HBW_SD6_MultiKey.h
 *
 * Created: 15.05.2017 00:29:36
 *  Author: viktor.pankraz
 */


#ifndef HBW_SD6_MULTIKEY_H_
#define HBW_SD6_MULTIKEY_H_

#include "HBWired/HBWired.h"
#include "HBWired/HBWKey.h"
#include "HBWired/HBWDimmer.h"
#include "HBWired/HBWDS1820.h"
#include "HBWired/HBWAnalogIn.h"

#include <Security/ModuleId.h>

extern __attribute__( ( section( ".vectors" ) ) ) const ModuleId moduleId;

// The HBWired-Device
class HBWSD6Multikey : public HBWDevice
{
   public:

      struct Config
      {
         HBWDevice::BasicConfig basicConfig; // 0x0000   - 0x0009
         uint8_t reserved[6];                // 0x000A   - 0x000F
      };

      inline HBWSD6Multikey( uint8_t _devicetype, uint8_t _hardware_version, uint16_t _firmware_version,
                             IoStream* _rs485, PortPin _txen,
                             Config* _config, HBWLinkSender* _linkSender = NULL, HBWLinkReceiver* _linkReceiver = NULL )
         : HBWDevice( _devicetype, _hardware_version, _firmware_version, _rs485, _txen, &_config->basicConfig, _linkSender, _linkReceiver )
      {
         checkConfig();
      }

      virtual void checkConfig();

   private:
      inline void checkAndCorrectConfig( HBWDS1820::Config* config )
      {
         if ( config->minDelta > 250 )
         {
            config->minDelta = 5;
         }
         if ( config->minInterval && ( ( config->minInterval < 5 ) || ( config->minInterval > 3600 ) ) )
         {
            config->minInterval = 10;
         }
         if ( config->maxInterval && ( ( config->maxInterval < 5 ) || ( config->maxInterval > 3600 ) ) )
         {
            config->maxInterval = 150;
         }
         if ( config->maxInterval && ( config->maxInterval < config->minInterval ) )
         {
            config->maxInterval = 0;
         }
      }

      inline void checkAndCorrectConfig( HBWKey::Config* config )
      {
         if ( ( config->getLongPressTime() < 4 ) || ( config->getLongPressTime() > 50 ) )
         {
            config->setLongPressTime( 10 );
         }
      }

      inline void checkAndCorrectConfig( HBWDimmer::Config* config )
      {
         // nothing to do
      }
};

#endif /* HBW_SD6_MULTIKEY_H_ */