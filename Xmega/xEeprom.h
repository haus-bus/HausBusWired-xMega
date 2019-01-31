/*
 * xEeprom.h
 *
 *  Created on: 18.07.2017
 *      Author: Viktor Pankraz
 */

#ifndef XEEPROM_H
#define XEEPROM_H

#include "Peripherals/Eeprom.h"


template<typename T>
class XEeprom
{
   private:
      // never call constructor the object itself points to eeprom cell
      inline XEeprom()
      {
      }
      T member;

   public:
      // Access/read members.
      T operator*() const
      {
         // Todo check if needed here
         // NvmController::waitWhileBusy();
         return member;
      }
      operator const T() const { return **this; }

      // Assignment/write members.
      XEeprom<T>& operator=( const XEeprom<T>& ref )
      {
         return *this = *ref;
      }
      XEeprom<T>& operator=( T in )
      {
         return Eeprom::write( (uintptr_t) &member, &in, sizeof( member ) ), *this;
      }

      XEeprom<T>& operator +=( T in )
      {
         return *this = **this + in;
      }
      XEeprom<T>& operator -=( T in )
      {
         return *this = **this - in;
      }
      XEeprom<T>& operator *=( T in )
      {
         return *this = **this * in;
      }
      XEeprom<T>& operator /=( T in )
      {
         return *this = **this / in;
      }
      XEeprom<T>& operator ^=( T in )
      {
         return *this = **this ^ in;
      }
      XEeprom<T>& operator %=( T in )
      {
         return *this = **this % in;
      }
      XEeprom<T>& operator &=( T in )
      {
         return *this = **this & in;
      }
      XEeprom<T>& operator |=( T in )
      {
         return *this = **this | in;
      }
      XEeprom<T>& operator <<=( uint8_t in )
      {
         return *this = **this << in;
      }
      XEeprom<T>& operator >>=( uint8_t in )
      {
         return *this = **this >> in;
      }

      XEeprom<T>& update( T in )
      {
         return in != *this ? *this = in : *this;
      }


      // Prefix increment/decrement
      XEeprom& operator++()
      {
         return *this += 1;
      }
      XEeprom& operator--()
      {
         return *this -= 1;
      }


      // Postfix increment/decrement
      T operator++( int )
      {
         T ret = **this;
         return ++( *this ), ret;
      }

      T operator--( int )
      {
         T ret = **this;
         return --( *this ), ret;
      }
};

typedef XEeprom<uint8_t>    uint8_tx;
typedef XEeprom<uint16_t>   uint16_tx;
typedef XEeprom<uint32_t>   uint32_tx;
typedef XEeprom<int8_t>     int8_tx;
typedef XEeprom<int16_t>    int16_tx;
typedef XEeprom<int32_t>    int32_tx;

#endif



