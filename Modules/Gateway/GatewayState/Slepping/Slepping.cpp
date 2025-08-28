//=====[Libraries]=============================================================
#include "Slepping.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "SensingBatteryStatus.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
Slepping::Slepping (Gateway * gateway) {
    this->gateway = gateway;
}

Slepping::~Slepping () {
    this->gateway = nullptr;
}

void Slepping::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void Slepping::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency
,NonBlockingDelay * silentKeepAliveTimer ) {
    static bool timeToWakeUp = false;
    static bool initialize = false;
    static bool initializeKeepAlive = false;
    MovementEvent_t movementEvent;
    OperationMode_t currentOperationMode = this->gateway->getOperationMode();
    this->gateway->updateMovementEvent();
    movementEvent = this->gateway->getMovementEvent();
    if ( initialize == false){
        initialize = true;
        latency->restart();
    }

     if (currentOperationMode == SILENT_OPERATION_MODE ) {
         if (silentKeepAliveTimer->read()) {
            uartUSB.write("\n\r**SILENT MODE TIMEOUT**\n\r", strlen("\n\r**SILENT MODE TIMEOUT**\n\r"));
            this->gateway->setOperationMode(NORMAL_OPERATION_MODE);
        }
     }

    if (currentOperationMode == PURSUIT_OPERATION_MODE ) {
        if (cellularTransceiver->turnOn () ) {
            timeToWakeUp = false;
            initialize = false;
            this->gateway->changeState  (new SensingBatteryStatus (this->gateway));
            return;
        }
         return;
    }

    if (movementEvent == PARKING || movementEvent == MOVEMENT_RESTARTED) {
        if (cellularTransceiver->turnOn () ) {
            timeToWakeUp = false;
            initialize = false;
            this->gateway->changeState  (new SensingBatteryStatus (this->gateway));
            return;
        }
    }

    if (movementEvent == STOPPED) {
        if (initializeKeepAlive == false) {
            this->gateway->actualizeKeepAliveLatency();
            initializeKeepAlive = true;
            return;
        }
        if (silentKeepAliveTimer->read()) {
            timeToWakeUp = true;
        }
        if (timeToWakeUp == true) {
            if (cellularTransceiver->turnOn () ) {
                timeToWakeUp = false;
                initialize = false;
                initializeKeepAlive = false;
                this->gateway->changeState  (new SensingBatteryStatus (this->gateway));
                return;
            }
        }
        return;
    }


    if (movementEvent == MOVING) {
        if (latency->read()) {
            timeToWakeUp = true;
        }
        if (timeToWakeUp == true) {
            if (cellularTransceiver->turnOn () ) {
                timeToWakeUp = false;
                initialize = false;
                this->gateway->changeState  (new SensingBatteryStatus (this->gateway));
                return;
            }
        }
    }
    return;
 }
 
//=====[Implementations of private methods]==================================