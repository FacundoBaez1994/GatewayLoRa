//=====[Libraries]=============================================================
#include "PowerManager.h"
#include "Debugger.h"

//=====[Declaration of private defines]========================================
#define POWERCHANGEDURATION  700
//#define REFRESHTIME  1000
//#define CELLULAR_MODULE_TX_UART PA_9
//#define CELLULAR_MODULE_RX_UART PA_10
//#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT PA_12
#define CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT PA_0
#define CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT PB_0
#define CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT PB_7
#define CELLULAR_MODULE_DTR_PIN_OUTPUT PB_6


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================s

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
PowerManager::PowerManager (ATCommandHandler * AThandler) {
    this->powerChangeDurationTimer = new NonBlockingDelay (POWERCHANGEDURATION  );
    this->ATHandler = AThandler;
    this->currentPowerState = new PowerOFFState (this);

    this->powerStatusInput = new DigitalIn (CELLULAR_MODULE_POWER_STATUS_SIGNAL_PIN_INPUT); 
    this->powerControlButtonInput = new DigitalIn (CELLULAR_MODULE_POWER_CONTROL_BUTTON_PIN_INPUT);
    this->powerKeyOutput = new DigitalOut (CELLULAR_MODULE_POWER_KEY_PIN_OUTPUT);
    this->powerDownOutput = new DigitalOut (CELLULAR_MODULE_POWER_DOWN_PIN_OUTPUT);
    this->powerKeySleepOutput = new DigitalOut (CELLULAR_MODULE_DTR_PIN_OUTPUT);

    this->powerControlButtonInput->mode(PullUp);
    *this->powerKeyOutput = OFF;
    *this->powerKeySleepOutput = OFF;
    *this->powerDownOutput = ON; 
}

PowerManager::~PowerManager () {
    delete this->powerChangeDurationTimer;
    this->powerChangeDurationTimer = nullptr;

    delete this->currentPowerState;
    this->currentPowerState = nullptr;

    delete this->powerStatusInput;
    this->powerStatusInput = nullptr;

    delete this->powerControlButtonInput;
    this->powerControlButtonInput = nullptr;

    delete this->powerKeyOutput;
    this->powerKeyOutput = nullptr;

    delete this->powerDownOutput;
    this->powerDownOutput = nullptr;

    delete this->powerKeySleepOutput;
    this->powerKeySleepOutput = nullptr;

    this->ATHandler = nullptr;
}

powerStatus_t PowerManager::startStopUpdate ( ) {
    return this->currentPowerState->startStopUpdate(this->ATHandler, this->powerChangeDurationTimer);
}

bool PowerManager::turnOff ( ) {
    return this->currentPowerState->turnOff (this->ATHandler, this->powerChangeDurationTimer);
}


bool PowerManager::turnOn ( ) {
    return this->currentPowerState->turnOn (this->ATHandler, this->powerChangeDurationTimer);
}

bool PowerManager::goToSleep ( ) {
    return this->currentPowerState->goToSleep(this->ATHandler, this->powerChangeDurationTimer);
}

void PowerManager::awake ( ) {
    this->currentPowerState->awake(this->ATHandler, this->powerChangeDurationTimer);
}

bool PowerManager::reboot ( ) {
    return this->currentPowerState->reboot(this->ATHandler, this->powerChangeDurationTimer);
}

bool PowerManager::measureBattery (BatteryData * currentBatteryData) {
    return this->currentPowerState->measureBattery(this->ATHandler, this->powerChangeDurationTimer
    , currentBatteryData);
}

void PowerManager::changePowerState (PowerState * newPowerState){
    delete this->currentPowerState;
    this->currentPowerState =  newPowerState;
} 

bool PowerManager::readInputControlButton ( ) {
    return this->powerControlButtonInput->read();
}

bool PowerManager::readPowerStatus ( ) {
    return this->powerStatusInput->read();
}

void PowerManager::changeKeyDigitalSignal (bool newStatus) {
    *this->powerKeyOutput = newStatus; 
}

void PowerManager::changePowerDownSignal (bool newStatus) {
    *this->powerDownOutput  = newStatus; 
}

void PowerManager::changeDTRSignal (bool newStatus) {
    *this->powerKeySleepOutput = newStatus; 
}

 BufferedSerial* PowerManager::getUART () {
     return this->ATHandler->getUART();
}

//=====[Implementations of private functions]==================================

