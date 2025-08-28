//=====[Libraries]=============================================================
#include "PowerOFFState.h"
#include "PowerManager.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE_CURRENT_STATE "\r\nPowerOFFState\r\n"
#define LOG_MESSAGE_CURRENT_STATE_LEN (sizeof(LOG_MESSAGE_CURRENT_STATE) - 1)

#define LOG_MESSAGE_CHANGE_STATE "AUTOMATIC POWER ON"
#define LOG_MESSAGE_CHANGE_STATE_LEN (sizeof(LOG_MESSAGE_CHANGE_STATE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
PowerOFFState::PowerOFFState () {
    uartUSB.write ( LOG_MESSAGE_CURRENT_STATE , strlen ( LOG_MESSAGE_CURRENT_STATE ));

    this->manager = nullptr;
    this->status = POWER_OFF;
}

PowerOFFState::PowerOFFState (PowerManager * newManager) {
    uartUSB.write ( LOG_MESSAGE_CURRENT_STATE , strlen ( LOG_MESSAGE_CURRENT_STATE ));
    this->manager = newManager;
    this->status = POWER_OFF;
}

PowerOFFState::~PowerOFFState () {
    this->manager = nullptr;
}

powerStatus_t PowerOFFState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool turningPower = false;

    // If powerStatus is in OFF status the power is ON (negate logic)
    if (this->manager->readPowerStatus()  == OFF) {
        this->manager->changePowerState (new PowerONState ( this->manager) );
        this->status = POWER_ON;
        return this->status;
    }

    if (turningPower == false && powerChangeDurationTimer->read() ) {
        uartUSB.write (LOG_MESSAGE_CHANGE_STATE, strlen (LOG_MESSAGE_CHANGE_STATE));  // debug only
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

bool PowerOFFState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

bool PowerOFFState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

void PowerOFFState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

bool PowerOFFState::turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    powerStatus_t currentPowerStatus;
    currentPowerStatus = this->startStopUpdate (AThandler, powerChangeDurationTimer);
    if (currentPowerStatus == POWER_ON) {
        return true;
    }
    return false;
}

bool PowerOFFState::turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    this->manager->changePowerState (new ManualPowerOFFState ( this->manager) );
    return true;
}

bool PowerOFFState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
    return false;
}
//=====[Implementations of private functions]==================================
