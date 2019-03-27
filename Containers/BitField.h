/*
 * BitField.h
 *
 *  Created on: 26.03.2019
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Containers_BitField_H
#define SwFramework_Containers_BitField_H

#include <DefaultTypes.h>

template<typename T>
class BitField
{
   public:

      BitField()
      {
         bitField = 0;
      }
      ////    Operations    ////

      inline bool isSet( uint8_t idx )
      {
         if ( idx < sizeof( bitField ) )
         {
            return ( bitField >> idx ) & 1;
         }
         return false;
      }

      inline bool set( uint8_t idx, uint8_t value )
      {
         if ( value )
         {
            return set( idx );
         }
         return clear( idx );
      }

      inline bool set( uint8_t idx )
      {
         if ( idx < sizeof( bitField ) )
         {
            bitField |= ( 1 << idx );
            return true;
         }
         return false;
      }

      inline bool clear( uint8_t idx )
      {
         if ( idx < sizeof( bitField ) )
         {
            bitField &= ~( 1 << idx );
            return true;
         }
         return false;
      }

      inline uint8_t size()
      {
         return sizeof( bitField );
      }

      ////    Additional operations    ////

   private:

      ////    Attributes    ////

   private:

      T bitField;
};

#endif

