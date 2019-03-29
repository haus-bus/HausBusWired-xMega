/*
 * BasicQueue.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#include <stdint.h>
#ifndef SwFramework_Containers_BasicQueue_H
#define SwFramework_Containers_BasicQueue_H


template<typename DataType, typename SizeType, typename ListType>
class BasicQueue
{
   ////    Constructors and destructors    ////

   public:

      BasicQueue();

      ////    Operations    ////

      DataType& back();

      void clear();

      DataType& front();

      ////    Attributes    ////

   protected:

      ListType data;

      volatile SizeType full;

      volatile SizeType pRead;

      volatile SizeType pWrite;
};


template<typename DataType, typename SizeType, typename ListType>
BasicQueue<DataType, SizeType, ListType>::BasicQueue()
{
   clear();
}

template<typename DataType, typename SizeType, typename ListType>
DataType& BasicQueue<DataType, SizeType, ListType>::back()
{
   return data[this->pWrite];
}

template<typename DataType, typename SizeType, typename ListType>
void BasicQueue<DataType, SizeType, ListType>::clear()
{
   pWrite = -1;
   pRead = 0;
   full = 0;
}

template<typename DataType, typename SizeType, typename ListType>
DataType& BasicQueue<DataType, SizeType, ListType>::front()
{
   return data[pRead];
}

#endif
