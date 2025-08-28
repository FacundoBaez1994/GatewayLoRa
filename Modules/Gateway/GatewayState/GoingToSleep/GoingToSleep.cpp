//=====[Libraries]=============================================================
#include "GoingToSleep.h"
#include "Slepping.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "SensingBatteryStatus.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
GoingToSleep::GoingToSleep (Gateway * gateway) {
    this->gateway = gateway;
}

GoingToSleep::~GoingToSleep () {
    this->gateway = nullptr;
}

void GoingToSleep::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GoingToSleep::goToSleep (CellularModule * cellularTransceiver ) {
    OperationMode_t operationMode = this->gateway->getOperationMode();
    if (operationMode  == PURSUIT_OPERATION_MODE) {
        this->gateway->updateMovementEvent();
        this->gateway->changeState  (new SensingBatteryStatus (this->gateway));
        return;
    }
    if (cellularTransceiver->turnOff()) {
        this->gateway->changeState  (new Slepping (this->gateway));
        return;
    }
    return;
}





//=====[Implementations of private methods]==================================