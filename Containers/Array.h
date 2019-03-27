/*
 * ArrayVector.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Containers_Array_H
#define SwFramework_Containers_Array_H

#include <DefaultTypes.h>

template<typename T, uint8_t MAX_SIZE>
class Array
{
   ////    Constructors and destructors    ////

   public:

      inline Array()
      {
         clear();
      }

      ////    Operations    ////

      inline uint8_t size()
      {
         return MAX_SIZE;
      }

      inline void clear()
      {
         memset( this, 0, sizeof( *this ) );
      }


      inline T get( uint8_t idx )
      {
         if ( idx < MAX_SIZE )
         {
            return elems[idx];
         }
         return 0;
      }

      inline bool set( uint8_t idx, T value )
      {
         if ( idx < MAX_SIZE )
         {
            elems[idx] = value;
            return true;
         }
         return false;
      }

      ////    Attributes    ////

   protected:

      T elems[MAX_SIZE];
};

#endif

