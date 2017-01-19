/*********************************************************************
 Rhapsody	: 8.0.3
 Login		: viktor.pankraz
 Component	: AR8
 Configuration 	: release
 Model Element	: IP
 //!	Generated Date	: Wed, 6, Aug 2014
 File Path	: AR8/release/SwFramework/Protocols/Ethernet/IP.h
 *********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_IP_H
#define SwFramework_Protocols_Ethernet_IP_H

//## auto_generated
#include "Ethernet.h"
//## package SwFramework::Protocols::Ethernet

//## class IP
class IP
{
  ////    Constructors and destructors    ////

public:

  inline IP( uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, uint8_t d = 0 )
  {
    setAddress(
        ((uint32_t) (d) << 24) + ((uint32_t) (c) << 16) + ((uint32_t) (b) << 8)
            + a );
  }

  inline IP( const uint32_t& _address )
  {
    setAddress( _address );
  }

  ////    Operations    ////

  inline void clear()
  {
    address = 0;
  }

  inline IP getMaskedIp() const;

  inline bool isBroadcast();

  inline bool isLocalBroadcast();

  inline bool isForMe() const;

  inline bool isInLocalNetwork() const;

  inline bool isValid() const
  {
    return address != 0;
  }

  inline bool operator==( const IP& _ip )
  {
    return this->address == _ip.address;
  }

  inline bool operator==( const uint32_t& _ip )
  {
    return this->address == _ip;
  }

  inline void setBroadcast()
  {
    address = 0xFFFFFFFF;
  }

  inline void setLocalBroadcast()
  {
    address = local.address | 0xFF000000;
  }

  ////    Additional operations    ////

  //## auto_generated
  inline uint32_t getAddress() const
  {
    //#[ auto_generated
    return address;
    //#]
  }

  //## auto_generated
  inline void setAddress( uint32_t p_address )
  {
    //#[ auto_generated
    address = p_address;
    //#]
  }

  //## auto_generated
  inline static IP* getLocal()
  {
    //#[ auto_generated
    return (IP*) &local;
    //#]
  }

  //## auto_generated
  inline static IP* getNetmask()
  {
    //#[ auto_generated
    return (IP*) &netmask;
    //#]
  }

  //## auto_generated
  inline static IP* getRouter()
  {
    //#[ auto_generated
    return (IP*) &router;
    //#]
  }

  ////    Attributes    ////

  uint32_t address;		//## attribute address

  static IP local;		//## attribute local

  static IP netmask;		//## attribute netmask

  static IP router;		//## attribute router
};


inline IP IP::getMaskedIp() const
{
  //#[ operation getMaskedIp() const
  return (address & netmask.address);
  //#]
}

inline bool IP::isBroadcast()
{
  //#[ operation isBroadcast()
  return address == 0xFFFFFFFF;
  //#]
}

inline bool IP::isLocalBroadcast()
{
  //#[ operation isBroadcast()
  return (address & 0xFF000000) == 0xFF000000;
  //#]
}

inline bool IP::isForMe() const
{
  //#[ operation isForMe() const
  return (local == address);
  //#]
}

inline bool IP::isInLocalNetwork() const
{
  //#[ operation isInLocalNetwork() const
  return getMaskedIp() == local.getMaskedIp();
  //#]
}

#endif
/*********************************************************************
 File Path	: AR8/release/SwFramework/Protocols/Ethernet/IP.h
 *********************************************************************/
