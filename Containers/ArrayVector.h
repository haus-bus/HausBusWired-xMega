/*
 * ArrayVector.h
 *
 *  Created on: 18.06.2014
 *      Author: Viktor Pankraz
 */

#ifndef SwFramework_Containers_ArrayVector_H
#define SwFramework_Containers_ArrayVector_H

#include <DefaultTypes.h>
#include <Tracing/Logger.h>

template<typename T, uint8_t MAX_SIZE>
class ArrayVector
{
   ////    Constructors and destructors    ////

   public:

      inline ArrayVector() :
         m_size( 0 )
      {

      }

      ////    Operations    ////

      inline T* begin()
      {
         return elems;
      }

      inline uint8_t capacity()
      {
         return MAX_SIZE;
      }

      inline void clear()
      {
         m_size = 0;
      }

      inline T* end()
      {
         return &elems[m_size];
      }

      inline T& getElement( uint8_t index )
      {
         return elems[index];
      }

      inline uint8_t size()
      {
         return m_size;
      }

      // adds an element if it does not already exist in the vector
      void add( const T& val );

      // adds an element to the end of the vector
      void pushBack( const T& val );

      // removes the element from the vector and closes the gap
      void remove( const T& val );

      bool hasElement( const T& val );

      ////    Attributes    ////

   protected:

      T elems[MAX_SIZE];

      uint8_t m_size;
};

template<typename T, uint8_t MAX_SIZE>
void ArrayVector<T, MAX_SIZE>::add(
   const T& val )
{
   for ( uint8_t i = 0; i < size(); i++ )
   {
      if ( val == elems[i] )
      {
         return;
      }
   }
   pushBack( val );
}

template<typename T, uint8_t MAX_SIZE>
void ArrayVector<T, MAX_SIZE>::pushBack(
   const T& val )
{
   if ( m_size < MAX_SIZE )
   {
      elems[m_size++] = val;
   }
   else
   {
      WARN_1( "ArrayVector<>: index out of range" );
   }
}

template<typename T, uint8_t MAX_SIZE>
void ArrayVector<T, MAX_SIZE>::remove(
   const T& val )
{
   for ( uint8_t i = 0; i < size(); i++ )
   {
      if ( val == elems[i] )
      {
         // copy all elements behind the found element
         // one position in frontside direction
         for ( uint8_t j = i; j < size() - 1; j++ )
         {
            elems[j] = elems[j + 1];
         }
         i--; // recheck the actual element
         m_size--;
      }
   }
}

template<typename T, uint8_t MAX_SIZE>
bool ArrayVector<T, MAX_SIZE>::hasElement(
   const T& val )
{
   for ( uint8_t i = 0; i < size(); i++ )
   {
      if ( val == elems[i] )
      {
         return true;
      }
   }
   return false;
}

#endif

