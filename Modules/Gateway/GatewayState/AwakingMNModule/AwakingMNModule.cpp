//=====[Libraries]=============================================================

#include "AwakingMNModule.h"
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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


/** 
* @brief
* 
* @param 
*/
AwakingMNModule::AwakingMNModule (Gateway * gateway) {
    this->gateway = gateway;
}

/** 
* @brief
* 
* @param 
*/
AwakingMNModule::~AwakingMNModule () {
    this->gateway = NULL;
}

void AwakingMNModule::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void AwakingMNModule::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void AwakingMNModule::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}

void AwakingMNModule::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void AwakingMNModule::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void AwakingMNModule::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void AwakingMNModule::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}

void AwakingMNModule::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
    return;
}

void AwakingMNModule::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}

void AwakingMNModule::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void AwakingMNModule::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    cellularTransceiver->awake();
    this->gateway->changeState  (new SensingBatteryStatus (this->gateway));
    return;
 }




//=====[Implementations of private methods]==================================