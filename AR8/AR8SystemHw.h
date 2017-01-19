#ifndef Electronics_SystemBoards_AR8_AR8_Boards_AR8SystemHw_H
#define Electronics_SystemBoards_AR8_AR8_Boards_AR8SystemHw_H

#include "AR8_Board.h"
#include <HomeAutomationHw.h>
#include <InternalEeprom.h>

class Checksum;

class DS1820;

class DaliDimmerHw;

class DaliLine;

class Dimmer;

class Eeprom;

class EventSystem;

class HacfIpStackManager;

class IrReceiver;

class Logger;

class LogicalButton;

class ModuleId;

class PersistentRules;

class PortPinUnit;

class RollerShutter;

class TimerCounter0;

class AR8SystemHw: public HomeAutomationHw
{
public:

  static const uint16_t TCC1_PRESCALER = 1024;

  ////    Constructors and destructors    ////

  AR8SystemHw();

  ////    Operations    ////

private:

  void configure();

  void configureDaliLine();

  void configureEthernet();

  void configureTwi();

  void configureLogicalButtons();

  void configureSlots();

  void configureZeroCrossDetection();

  ////    Additional operations    ////

public:

  InternalEeprom* getInternalEeprom() const;

  ////    Attributes    ////

  ////    Relations and components    ////

protected:

  DaliLine* daliLine;

  InternalEeprom internalEeprom;

};

#endif
/*********************************************************************
 File Path	: AR8/release/Electronics/SystemBoards/AR8/AR8_Boards/AR8SystemHw.h
 *********************************************************************/
