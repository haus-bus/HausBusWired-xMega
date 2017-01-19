/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: OneWire
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/HwProtocols/OneWire.h
*********************************************************************/

#ifndef HwProtocols_OneWire_H
#define HwProtocols_OneWire_H

//## auto_generated
#include "HwProtocols.h"
//## attribute ioPort
#include "Peripherals/IoPort.h"
//## dependency DS1820
class DS1820;

//## operation OneWire(PortPin)
class PortPin;

//## package HwProtocols

//## class OneWire
class OneWire {
public :

    //#[ type Constants
    static const uint8_t RECOVERY_DELAY = 2;     
    static const uint8_t CYCLES_PER_IO_ACCESS = 13;
    static const uint8_t IO_ACCESS_DELAY =  CYCLES_PER_IO_ACCESS*1000000L / F_CPU;
    static const uint8_t READ_TIME_SLOT = 15;   
    static const uint8_t BIT_TIME_SLOT = 60;  
     
    static const uint8_t ROMCODE_SIZE = 8;
    static const uint8_t LAST_DEVICE  = 0x00;  
    static const uint8_t SEARCH_FIRST = 0xFF;  
    
    //#]
    
    //#[ type Errors
    static const uint8_t DATA_ERROR     = 0xFE;   
    static const uint8_t PRESENCE_ERROR = 0xFF;
    //#]
    
    //#[ type Commands
    static const uint8_t MATCH_ROM	= 0x55;   
    static const uint8_t SKIP_ROM	  = 0xCC;
    static const uint8_t SEARCH_ROM	= 0xF0; 
    
    //#]
    
    //## type RomCode
    struct RomCode {
        uint8_t family;		//## attribute family
        uint8_t serial[6];		//## attribute serial
        uint8_t crc;		//## attribute crc
    };
    
    ////    Constructors and destructors    ////
    
    //## operation OneWire(PortPin)
    OneWire(PortPin portPin);
    
    ////    Operations    ////
    
    //## operation disableParasite()
    inline void disableParasite();
    
    //## operation enableParasite()
    inline void enableParasite();
    
    //## operation isIdle()
    inline uint8_t isIdle();
    
    //## operation read()
    inline uint8_t read();
    
    //## operation reset()
    uint8_t reset();
    
    //## operation scanAndCreateDevices()
    void scanAndCreateDevices();
    
    //## operation searchROM(uint8_t,uint8_t)
    uint8_t searchROM(uint8_t diff, uint8_t* id);
    
    //## operation sendCommand(uint8_t,uint8_t)
    void sendCommand(uint8_t command, uint8_t* id);
    
    //    Timing issue when using runtime-bus-selection (!OW_ONE_BUS):
    //    The master should sample at the end of the 15-slot after initiating
    //    the read-time-slot. The variable bus-settings need more
    //    cycles than the constant ones so the delays had to be shortened 
    //    to achive a 15uS overall delay 
    //    Setting/clearing a bit in I/O Register needs 1 cyle in OW_ONE_BUS
    //    but around 14 cyles in configureable bus (us-Delay is 4 cyles per uS) 
    //## operation sendReceiveBit(uint8_t)
    uint8_t sendReceiveBit(uint8_t bit);
    
    //## operation write(uint8_t)
    uint8_t write(uint8_t data);
    
    ////    Additional operations    ////
    
    //## auto_generated
    inline IoPort& getIoPort() const {
        //#[ auto_generated
        return ioPort;
        //#]
    }
    
    //## auto_generated
    inline void setIoPort(IoPort& p_ioPort) {
        //#[ auto_generated
        ioPort = p_ioPort;
        //#]
    }
    
    //## auto_generated
    inline uint8_t getPin() const {
        //#[ auto_generated
        return pin;
        //#]
    }
    
    //## auto_generated
    inline void setPin(uint8_t p_pin) {
        //#[ auto_generated
        pin = p_pin;
        //#]
    }

private :

    //## auto_generated
    inline static const uint8_t getDebugLevel() {
        //#[ auto_generated
        return debugLevel;
        //#]
    }
    
    ////    Attributes    ////

public :
    uint8_t pin;		//## attribute pin

    IoPort& ioPort;		//## attribute ioPort
    

private :

    static const uint8_t debugLevel;		//## attribute debugLevel
};

inline void OneWire::disableParasite() {
    //#[ operation disableParasite()
    ioPort.clearPins( pin );
    ioPort.setPinsAsInput( pin );
    //#]
}

inline void OneWire::enableParasite() {
    //#[ operation enableParasite()
    ioPort.setPins( pin );
    ioPort.setPinsAsOutput( pin );
    //#]
}

inline uint8_t OneWire::isIdle() {
    //#[ operation isIdle()
    return ioPort.isPinSet( pin );
    //#]
}

inline uint8_t OneWire::read() {
    //#[ operation read()
    return write( 0xFF );
    //#]
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/HwProtocols/OneWire.h
*********************************************************************/
