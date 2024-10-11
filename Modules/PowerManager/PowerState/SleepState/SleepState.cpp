//=====[Libraries]=============================================================

#include "SleepState.h"
#include "PowerManager.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
SleepState::SleepState () {
    this->manager = NULL;
    this->status = SLEEP;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief
* 
* @param 
*/
SleepState::SleepState (PowerManager * newManager) {
    this->manager = newManager;
    this->status = SLEEP;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
SleepState::~SleepState () {
    this->manager = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t SleepState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return SLEEP;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void SleepState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
  return true;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void SleepState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    char StringToSendUSB [40] = "Awakening";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
     uartUSB.write ( "\r\n",  3 );  // debug only
    this->manager->changeDTRSignal(OFF);
    this->manager->changePowerState (new PowerONState ( this->manager) );
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool SleepState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
   return false;
}