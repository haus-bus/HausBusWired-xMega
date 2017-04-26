/********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: Xmega192A3 
	Configuration 	: debug
	Model Element	: OneWire
//!	Generated Date	: Tue, 24, Jun 2014  
	File Path	: Xmega192A3/debug/HwProtocols/OneWire.cpp
*********************************************************************/


#include "OneWire.h"
#include <Utils/CriticalSection.h>
#include <util/delay.h>


OneWire::OneWire( uint8_t _pin ) : pin(_pin) 
{
    ioport_configure_pin( pin, IOPORT_PULL_UP );
}

uint8_t OneWire::reset() 
{
	ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_LOW);    // disable internal pull-up (maybe on from parasite)
	ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT);         // pull OW-Pin low for 480us
    
    _delay_us( 600 );
    
    uint8_t err;
    CriticalSection doNotInterrupt;
    {  
      // set Pin as input - wait for clients to pull low
      ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);
      
      _delay_us(66);
      
      err = ioport_get_pin_level( pin );		// no presence detect
      // nobody pulled to low, still high
    }
    
    // after a delay the clients should release the line
    // and input-pin gets back to high due to pull-up-resistor
    _delay_us(480-66);
    if( !ioport_get_pin_level( pin ) )		// short circuit  
    {
    	err = 1;
    }
    
    return err;
}

uint8_t OneWire::searchROM(uint8_t diff, uint8_t* id) {
    //#[ operation searchROM(uint8_t,uint8_t)
    uint8_t i, j, next_diff;
    uint8_t b;
    
    if( reset() ) 
    {
      return PRESENCE_ERROR;	  // error, no device found
    }
    
    write( SEARCH_ROM );				// ROM search command
    next_diff = LAST_DEVICE;		// unchanged on last device
    
    i = ROMCODE_SIZE * 8;		    // 8 bytes
    
    do {
    	j = 8;									  // 8 bits
    	do {
    		b = sendReceiveBit( 1 );		// read bit
    		if( sendReceiveBit( 1 ) )   // read complement bit
    		{				
    			if( b )							      // 11
    			{
    			  return DATA_ERROR;		  // data error     
    			}
    		}
    		else 
    		{
    			if( !b ) {						    // 00 = 2 devices
    				if( (diff > i) || ((*id & 1) && (diff != i)) ) 
    				{
    				  b = 1;				        // now 1
    				  next_diff = i;			  // next pass 0
    				}
    			}
    		}
    		sendReceiveBit( b );     	  // write bit
    		*id >>= 1;
    		if( b ) *id |= 0x80;			  // store bit
    		
    		i--;
    		
    	} while( --j );
    	
    	id++;   // next byte
    
    } while( i );
    
    return next_diff;				// to continue search
    //#]
}

void OneWire::sendCommand(uint8_t command, uint8_t* id) {
    //#[ operation sendCommand(uint8_t,uint8_t)
    uint8_t i;
    
    reset();
    
    if( id ) 
    {
    	write( MATCH_ROM );			// to a single device
    	i = ROMCODE_SIZE;
    	do {
    		write( *id );
    		id++;
    	} while( --i );
    } 
    else {
    	write( SKIP_ROM );			// to all devices
    }
    
    write( command );
    //#]
}

uint8_t OneWire::sendReceiveBit(uint8_t bit) 
{
    CriticalSection doNotInterrupt;

    ioport_set_pin_level(pin, IOPORT_PIN_LEVEL_LOW); 
    ioport_set_pin_dir(pin, IOPORT_DIR_OUTPUT);     // drive bus low
      
    _delay_us( RECOVERY_DELAY );    // Recovery-Time
    if ( bit )     // if bit is 1 set bus high (by ext. pull-up)
    {
        ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);
    } 
    _delay_us( READ_TIME_SLOT-RECOVERY_DELAY-IO_ACCESS_DELAY );
    
    // sample at end of read-timeslot	
    if( !ioport_get_pin_level( pin ) )
    {
        bit = 0;
    }  
      
    _delay_us( BIT_TIME_SLOT-READ_TIME_SLOT );
    ioport_set_pin_dir(pin, IOPORT_DIR_INPUT);

    // bei langen Kabeln kann dieser Wert wohl auch um einiges größer sein! (recovery time)
    _delay_us(10);	
    
    return bit;
}

uint8_t OneWire::write(uint8_t data) 
{
    uint8_t i = 8, j;
    
    do {
    	j = sendReceiveBit( data & 1 );
    	data >>= 1;
    	if( j ) data |= 0x80;
    } while( --i );
    
    return data;
}

/*********************************************************************
	File Path	: Xmega192A3/debug/HwProtocols/OneWire.cpp
*********************************************************************/
