/*
 * HbwBooterHw.h
 *
 * Created: 23.11.2017 19:23:55
 * Author: viktor.pankraz
 */


#ifndef __HMWBOOTERSTREAMHW_H__
#define __HMWBOOTERSTREAMHW_H__

#include <HmwUnits/HmwStreamHw.h>

class HBWBooterHw : public HmwStreamHw
{
// variables
   public:
   protected:
   private:

// functions
   public:
      HBWBooterHw();

      void enableTranceiver( bool enable );

      static void debug( char c );

      static void handleLeds( bool isDownloadRunning );

      static void notifyNextDownloadPacket();

   protected:
   private:


}; // HmwBooterStreamHw

#endif // __HMWBOOTERSTREAMHW_H__
