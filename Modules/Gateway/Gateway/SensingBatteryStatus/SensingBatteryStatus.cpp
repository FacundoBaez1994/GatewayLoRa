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


/** 
* @brief
* 
* @param 
*/
SensingBatteryStatus::SensingBatteryStatus (Gateway * gateway) {
    this->gateway = gateway;
}

/** 
* @brief
* 
* @param 
*/
SensingBatteryStatus::~SensingBatteryStatus ( ) {
    this->gateway = NULL;
}

void SensingBatteryStatus::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
    if (cellularTransceiver->measureBattery(currentBatteryStatus) == true){
        this->gateway->changeState  (new GettingGNSSPosition (this->gateway));
        return;
    }
    return;
 }

void SensingBatteryStatus::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return;
}

void SensingBatteryStatus::obtainGNSSPosition (GNSSModule * currentGNSSModule, 
GNSSData * currentGNSSdata) {
    return;
}

void SensingBatteryStatus::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) { 
    return; 
}

void SensingBatteryStatus::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
    return;
}

void SensingBatteryStatus::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    return;
}

void SensingBatteryStatus::goToSleep (CellularModule * cellularTransceiver) {
    return;
}

void SensingBatteryStatus::awake (CellularModule * cellularTransceiver, 
NonBlockingDelay * latency ) {
    return;
}



