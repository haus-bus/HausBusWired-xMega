/********************************************************************
        Rhapsody	: 8.0.3
        Login		: viktor.pankraz
        Component	: AR8
        Configuration   : release
        Model Element	: IpHeader
   //!	Generated Date	: Wed, 6, Aug 2014
        File Path	: AR8/release/SwFramework/Protocols/Ethernet/IpHeader.cpp
 *********************************************************************/

// ## auto_generated
#include "IpHeader.h"
// ## package SwFramework::Protocols::Ethernet

// ## class IpHeader
void IpHeader::setChecksum()
{
   // #[ operation setChecksum()
   checksum = 0;
   setChecksum( Checksum16::get( ( (EthernetHeader*)this ) + 1, getLength() << 2 ) );
   // #]
}

/*********************************************************************
        File Path	: AR8/release/SwFramework/Protocols/Ethernet/IpHeader.cpp
*********************************************************************/
