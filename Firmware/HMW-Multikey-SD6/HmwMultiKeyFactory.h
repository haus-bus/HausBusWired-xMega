/*
 * HmwMultiKeyFactory.h
 *
 * Created: 09.01.2018 21:04:36
 * Author: viktor.pankraz
 */


#ifndef __HMWMULTIKEYFACTORY_H__
#define __HMWMULTIKEYFACTORY_H__


#include "HmwMultiKeySD6v0Hw.h"
#include "HmwMultiKeySD6v1Hw.h"
#include "HmwMultiKeySD6v2Hw.h"

class HmwMultiKeyFactory
{
// variables
   public:

      static inline HmwDeviceHw* createHardware()
      {
         switch ( HmwMultiKeySD6BaseHw::getVersion() )
         {
            case Release::REV_0:
            {
               return new HmwMultiKeySD6v0Hw();
            }
            case Release::REV_1:
            {
               return new HmwMultiKeySD6v1Hw();
            }
            case Release::REV_2:
            {
               return new HmwMultiKeySD6v2Hw();
            }

            default:
               while ( 1 )
               {
               }
         }
      }

   protected:
   private:

// functions
   public:

   protected:
   private:

}; // HmwMultiKeyFactory

#endif // __HMWMULTIKEYFACTORY_H__
