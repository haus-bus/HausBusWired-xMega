/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: AR8 
	Configuration 	: release
	Model Element	: MAC
//!	Generated Date	: Wed, 6, Aug 2014  
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/MAC.h
*********************************************************************/

#ifndef SwFramework_Protocols_Ethernet_MAC_H
#define SwFramework_Protocols_Ethernet_MAC_H

//## auto_generated
#include "Ethernet.h"
//## package SwFramework::Protocols::Ethernet

//## class MAC
class MAC {
    ////    Constructors and destructors    ////
    
public :

    //## operation MAC(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t)
    inline MAC(uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, uint8_t d = 0, uint8_t e = 0, uint8_t f = 0) {
        //#[ operation MAC(uint8_t,uint8_t,uint8_t,uint8_t,uint8_t,uint8_t)
        address[0] = a;
        address[1] = b;
        address[2] = c;
        address[3] = d;
        address[4] = e;
        address[5] = f;
        //#]
    }
    
    ////    Operations    ////
    
    //## operation isBroadcast()
    inline bool isBroadcast();
    
    //## operation isLocal()
    inline bool isLocal();
    
    //## operation isValid()
    inline bool isValid();
    
    //## operation setBroadcast()
    inline void setBroadcast();
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline uint8_t getAddress(int i1) const {
        //#[ auto_generated
        return address[i1];
        //#]
    }
    
    //## auto_generated
    inline void setAddress(int i1, uint8_t p_address) {
        //#[ auto_generated
        address[i1] = p_address;
        //#]
    }
    
    //## auto_generated
    inline static MAC* getLocal() {
        //#[ auto_generated
        return (MAC*) &local;
        //#]
    }
    
    ////    Attributes    ////
    
    uint8_t address[6];		//## attribute address
    
    static MAC local;		//## attribute local
};

inline bool MAC::isBroadcast() {
    //#[ operation isBroadcast()
    uint8_t i = sizeof(address);
    while( i-- )
    {
      if( address[i] != 0xFF )
      {
        return false;
      }
    }
    return true;
    //#]
}

inline bool MAC::isLocal() {
    //#[ operation isLocal()
    uint8_t i = sizeof(address);
    while( i-- )
    {
      if( address[i] != local.getAddress(i) )
      {
        return false;
      }
    }
    return true;
    //#]
}

inline bool MAC::isValid() {
    //#[ operation isValid()
    uint8_t i = sizeof(address);
    while( i-- )
    {
      if( address[i] != 0 )
      {
        return true;
      }
    }
    return false;
    //#]
}

inline void MAC::setBroadcast() {
    //#[ operation setBroadcast()
    uint8_t i = sizeof(address);
    while( i-- )
    {
      address[i] = 0xFF;
    }
    //#]
}

#endif
/*********************************************************************
	File Path	: AR8/release/SwFramework/Protocols/Ethernet/MAC.h
*********************************************************************/
