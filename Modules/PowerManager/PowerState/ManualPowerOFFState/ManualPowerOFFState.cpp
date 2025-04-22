//=====[Libraries]=============================================================

#include "ManualPowerOFFState.h" 
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
ManualPowerOFFState::ManualPowerOFFState () {
    this->manager = NULL;
    this->status = MANUAL_POWER_OFF;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningUP = false;
}


/** 
* @brief
* 
* @param 
*/
ManualPowerOFFState::ManualPowerOFFState (PowerManager * newManager) {
    this->manager = newManager;
    this->status = MANUAL_POWER_OFF;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningUP = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ManualPowerOFFState::~ManualPowerOFFState () {
    this->manager = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t ManualPowerOFFState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {

    // If powerStatus is in OFF status the power is ON (negate logic)
    if (this->manager->readPowerStatus()  == OFF) {
        //////////////////////////////////////////
        char StringToSend [30] = "Turning ON";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changePowerState (new PowerONState ( this->manager) );
        this->status = POWER_ON;
        return this->status;
    }

 
    // reverse logic
    // Reading button
    if (this->manager->readInputControlButton() == OFF) {
        this->ManualTurningPower = true;
        this->TurningUP = true;
    }

    // Start Stop Signal
    if (ManualTurningPower == true && SignalTurningPowerUp == false && this->TurningUP  == true ) {

        //////////////////////////////////////////
        char StringToSend [30] = "UP";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (true);
        this->ManualTurningPower = true;
        this->SignalTurningPowerUp = true;
        powerChangeDurationTimer->restart();
    }
    if (this->SignalTurningPowerUp  == true && powerChangeDurationTimer->read() && this->TurningUP  == true) {

        //////////////////////////////////////////
        char StringToSend [30] = "DOWN";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (false);
        this->SignalTurningPowerUp = false;
        powerChangeDurationTimer->restart();
        this->TurningUP = false;
    }
    return this->status;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool ManualPowerOFFState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool ManualPowerOFFState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void ManualPowerOFFState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool ManualPowerOFFState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
    return false;
}
//=====[Implementations of private functions]==================================
