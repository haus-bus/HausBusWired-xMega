/*********************************************************************
	Rhapsody	: 8.0.3 
	Login		: viktor.pankraz
	Component	: SwFrameworkAvr 
	Configuration 	: debug
	Model Element	: BasicQueue
//!	Generated Date	: Wed, 18, Jun 2014  
	File Path	: SwFrameworkAvr/debug/SwFramework/Containers/BasicQueue.h
*********************************************************************/

#ifndef SwFramework_Containers_BasicQueue_H
#define SwFramework_Containers_BasicQueue_H

//## auto_generated
#include "Containers.h"
//## package SwFramework::Containers

//## class BasicQueue
template <typename DataType, typename SizeType, typename ListType> class BasicQueue {
    ////    Constructors and destructors    ////
    
public :

    //## operation BasicQueue()
    BasicQueue();
    
    ////    Operations    ////
    
    //## operation back()
    DataType& back();
    
    //## operation clear()
    void clear();
    
    //## operation front()
    DataType& front();
    
    ////    Attributes    ////

protected :

    ListType data;		//## attribute data
    
    volatile SizeType full;		//## attribute full
    
    volatile SizeType pRead;		//## attribute pRead
    
    volatile SizeType pWrite;		//## attribute pWrite
};

//## package SwFramework::Containers

//## class BasicQueue
template <typename DataType, typename SizeType, typename ListType> BasicQueue<DataType, SizeType, ListType>::BasicQueue() {
    //#[ operation BasicQueue()
    clear();
    //#]
}

template <typename DataType, typename SizeType, typename ListType> DataType& BasicQueue<DataType, SizeType, ListType>::back() {
    //#[ operation back()
    return data[this->pWrite];
    //#]
}

template <typename DataType, typename SizeType, typename ListType> void BasicQueue<DataType, SizeType, ListType>::clear() {
    //#[ operation clear()
    pWrite = -1; 
    pRead = 0;  
    full = 0;
    
    //#]
}

template <typename DataType, typename SizeType, typename ListType> DataType& BasicQueue<DataType, SizeType, ListType>::front() {
    //#[ operation front()
    return data[pRead];
    //#]
}

#endif
/*********************************************************************
	File Path	: SwFrameworkAvr/debug/SwFramework/Containers/BasicQueue.h
*********************************************************************/
