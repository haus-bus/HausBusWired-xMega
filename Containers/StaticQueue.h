/*
 * StaticQueue.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Containers_StaticQueue_H
#define SwFramework_Containers_StaticQueue_H

#include "Containers.h"
#include "BasicQueue.h"
#include "Tracing/Logger.h"

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
};

template<typename DataType, typename SizeType, SizeType capacity>
StaticQueue<DataType, SizeType, capacity>::StaticQueue()
{
   BasicQueue<DataType, SizeType, DataType[capacity]>();
}

template<typename DataType, typename SizeType, SizeType capacity>
inline SizeType StaticQueue<DataType, SizeType, capacity>::getCapacity()
{
   return capacity;
}

template<typename DataType, typename SizeType, SizeType capacity>
inline SizeType StaticQueue<DataType, SizeType, capacity>::getSize()
{
   return this->full;
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<DataType, SizeType, capacity>::isEmpty()
{
   return !this->full;
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<DataType, SizeType, capacity>::isFull()
{
   return ( this->full == capacity );
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<DataType, SizeType, capacity>::pop( DataType& value )
{
   if ( isEmpty() )
   {
      return false;
   }
   // Logger::instance() << endl << "pop  r " << this->pRead << " f " << (uint8_t) (this->full - 1);

   value = this->data[this->pRead++];
   this->pRead %= capacity;
   this->full--;

   return true;

}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<DataType, SizeType, capacity>::pop()
{
   DataType value;
   return pop( value );
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<DataType, SizeType, capacity>::popBack( DataType& value )
{
   if ( isEmpty() )
   {
      return false;
   }
   // Logger::instance() << endl << "popB w" << this->pWrite << " f " << (uint8_t) (this->full - 1);

   value = this->data[this->pWrite--];
   this->pWrite %= capacity;
   this->full--;
   return true;
}

template<typename DataType, typename SizeType, SizeType capacity>
bool StaticQueue<DataType, SizeType, capacity>::push( const DataType& value )
{
   if ( isFull() )
   {
      return false;
   }
   this->pWrite++;
   this->pWrite %= capacity;
   this->full++;
   this->data[this->pWrite] = value;

   // Logger::instance() << endl << "push w " << this->pWrite << " f " << this->full;
   return true;
}

#endif
