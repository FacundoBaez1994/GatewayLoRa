//=====[Libraries]=============================================================

#include "SensingBatteryStatus.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GettingGNSSPosition.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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
SensingBatteryStatus::SensingBatteryStatus (Gateway * gateway) {
    this->gateway = gateway;
}

/** 
* @brief
* 
* @param 
*/
SensingBatteryStatus::~SensingBatteryStatus ( ) {
    this->gateway = nullptr;
}

void SensingBatteryStatus::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
    cellularTransceiver->turnOn();
    if (cellularTransceiver->measureBattery(currentBatteryStatus) == true){
        this->gateway->changeState  (new GettingGNSSPosition (this->gateway));
        return;
    }
    return;
 }


