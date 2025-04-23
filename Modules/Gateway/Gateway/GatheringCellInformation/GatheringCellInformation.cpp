//=====[Libraries]=============================================================

#include "GatheringCellInformation.h"
#include "FormattingMessage.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

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
GatheringCellInformation::GatheringCellInformation (Gateway * gateway) {
    this->gateway = gateway;
}

/** 
* @brief
* 
* @param 
*/
GatheringCellInformation::~GatheringCellInformation () {
    this->gateway = NULL;
}


void GatheringCellInformation::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void GatheringCellInformation::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void GatheringCellInformation::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}

void GatheringCellInformation::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GatheringCellInformation::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void GatheringCellInformation::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void GatheringCellInformation::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {

    if (cellularTransceiver->retrivNeighborCellsInformation(neighborsCellInformation,
     numberOfNeighbors) == true) {
         this->gateway->changeState (new FormattingMessage (this->gateway, 
         GATEWAY_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK));
         return;
     }
    // Format message for LoRa 
    return; 
}

void GatheringCellInformation::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
        
    return;
}

void GatheringCellInformation::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    return;
}

void GatheringCellInformation::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void GatheringCellInformation::awake (CellularModule * cellularTransceiver,
 NonBlockingDelay * latency ) {
    return;
}






