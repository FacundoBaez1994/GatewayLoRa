//=====[Libraries]=============================================================

#include "PowerOFFState.h"
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
PowerOFFState::PowerOFFState () {
    this->manager = NULL;
    this->status = POWER_OFF;
}


/** 
* @brief
* 
* @param 
*/
PowerOFFState::PowerOFFState (PowerManager * newManager) {
    this->manager = newManager;
    this->status = POWER_OFF;
}

/** 
* @brief 
* 
* 
* @returns 
*/
PowerOFFState::~PowerOFFState () {
    this->manager = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t PowerOFFState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool turningPower = false;

    // If powerStatus is in OFF status the power is ON (negate logic)
    if (this->manager->readPowerStatus()  == OFF) {
        this->manager->changePowerState (new PowerONState ( this->manager) );
        this->status = POWER_ON;
        return this->status;
    }

    if (turningPower == false && powerChangeDurationTimer->read() ) {
        ////////////  //////////// ////////////
        char StringToSend [30] = "AUTOMATIC POWER ON";;
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////// //////////// ////////////
        this->manager->changeKeyDigitalSignal (true);
        turningPower = true;
        powerChangeDurationTimer->restart();
    }
    if (turningPower == true && powerChangeDurationTimer->read() ) {
        this->manager->changeKeyDigitalSignal (false);
        turningPower = false;
        powerChangeDurationTimer->restart();
    }

    return this->status;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool PowerOFFState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool PowerOFFState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerOFFState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool PowerOFFState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
    return false;
}
//=====[Implementations of private functions]==================================
