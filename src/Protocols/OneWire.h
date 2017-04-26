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

extern "C"
{
    #include <asf.h>
}


//## dependency DS1820
class HBWDS1820;

//## operation OneWire(PortPin)
class PortPin;

//## package HwProtocols

//## class OneWire
class OneWire {
public :

    //#[ type Constants
    static const uint8_t RECOVERY_DELAY = 2;     
    static const uint8_t CYCLES_PER_IO_ACCESS = 13;
    static const uint8_t IO_ACCESS_DELAY =  1;
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
    OneWire( uint8_t _pin );
    
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
    
private :
    
    ////    Attributes    ////

public :
    uint8_t pin;		//## attribute pin
    

private :

    static const uint8_t debugLevel;		//## attribute debugLevel
};

inline void OneWire::disableParasite() 
{
	ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);
}

inline void OneWire::enableParasite() 
{
	ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT);
}

inline uint8_t OneWire::isIdle() 
{
    return ioport_get_value( pin );
}

inline uint8_t OneWire::read() 
{
    return write( 0xFF );
}

#endif
/*********************************************************************
	File Path	: Xmega192A3/debug/HwProtocols/OneWire.h
*********************************************************************/
