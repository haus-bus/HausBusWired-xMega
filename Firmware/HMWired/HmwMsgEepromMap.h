/*
 * HmwMsgEepromMap.h
 *
 * Created: 19.11.2017 00:05:11
 * Author: viktor.pankraz
 */


#ifndef __HMWMSGEEPROMMAP_H__
#define __HMWMSGEEPROMMAP_H__

#include "HmwMessageBase.h"

class HmwMsgEepromMap : public HmwMessageBase
{
// variables
   public:
   protected:
   private:

// functions
   public:
      inline void setupResponse()
      {
         uint8_t blocksize = frameData[3];
         uint8_t blocknum = frameData[4];

         // length of response
         frameDataLength = 4 + blocknum / 8;

         // care for odd block numbers
         if ( blocknum % 8 )
         {
            frameDataLength++;
         }

         // set answer ID
         frameData[0] = HmwMessageBase::EEPROM_MAP;

         // init to zero, mainly because we need it later
         memset( &frameData[4], 0, frameDataLength - 4 );


         // determine whether blocks are used
         for ( int block = 0; block <= blocknum; block++ )
         {
            // check this memory block
            uint8_t* blockAddr = (uint8_t*)( MAPPED_EEPROM_START + block * blocksize );
            for ( int byteIdx = 0; byteIdx < blocksize; byteIdx++ )
            {
               if ( blockAddr[byteIdx] != 0xFF )
               {
                  frameData[4 + block / 8] |= ( 1 << ( block % 8 ) );
                  break;
               }
            }
         }
      }

   protected:

   private:


}; // HmwMsgEepromMap

#endif // __HMWMSGEEPROMMAP_H__
