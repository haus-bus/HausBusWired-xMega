/*
 * StaticQueue.h
 *
 *  Created on: 06.12.2017
 *      Author: Viktor Pankraz
 */

#ifndef Framework_Containers_StaticQueue_H
#define Framework_Containers_StaticQueue_H

#include "BasicQueue.h"

template<typename DataType, typename SizeType, SizeType capacity>
class StaticQueue : public BasicQueue<
                       DataType, SizeType, DataType[capacity]>
{
   ////    Constructors and destructors    ////

   public:

      StaticQueue();

      ////    Operations    ////

      inline SizeType getCapacity();

      inline SizeType getSize();

      bool isEmpty();

      bool isFull();

      bool pop( DataType& value );

      bool pop();

      bool popBack( DataType& value );

      bool push( const DataType& value );

      static const uint8_t debugLevel = DEBUG_LEVEL_OFF;
};

template<typename DataType, typename SizeType, SizeType capacity>
StaticQueue<
   DataType, SizeType, capacity>::StaticQueue()
{
   BasicQueue<DataType, SizeType, DataType[capacity]>();
}

template<typename DataType, typename SizeType, SizeType capacity>
inline SizeType StaticQueue<
   DataType, SizeType, capacity>::getCapacity()
{
   return capacity;
}

template<typename DataType, typename SizeType, SizeType capacity>
inline SizeType StaticQueue<
   DataType, SizeType, capacity>::getSize()
{
   return this->full;
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<
   DataType, SizeType, capacity>::isEmpty()
{
   return !this->full;
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<
   DataType, SizeType, capacity>::isFull()
{
   return ( this->full == capacity );
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<
   DataType, SizeType, capacity>::pop( DataType& value )
{
   if ( isEmpty() )
   {
      return false;
   }
   DEBUG_M3( (uintptr_t)this, ": pop ", this->pRead );
   DEBUG_L2( ':', this->full );

   value = this->data[this->pRead++];
   this->pRead %= capacity;
   this->full--;

   return true;

}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<
   DataType, SizeType, capacity>::pop()
{
   DataType value;
   return pop( value );
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<
   DataType, SizeType, capacity>::popBack( DataType& value )
{
   if ( isEmpty() )
   {
      return false;
   }
   DEBUG_M3( (uintptr_t)this, ": pop ", this->pWrite );
   DEBUG_L2( ':', this->full );
   value = this->data[this->pWrite--];
   this->pWrite %= capacity;
   this->full--;
   return true;
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<
   DataType, SizeType, capacity>::push( const DataType& value )
{
   if ( isFull() )
   {
      return false;
   }
   this->pWrite++;
   this->pWrite %= capacity;
   this->full++;
   this->data[this->pWrite] = value;
   DEBUG_M3( (uintptr_t)this, ": push ", this->pWrite );
   DEBUG_L2( ':', this->full );
   return true;
}

#endif
