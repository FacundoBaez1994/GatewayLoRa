//=====[Libraries]=============================================================

#include "GatewayBaseState.h"
//#include "Gateway.h" //debido a declaracion adelantada
//#include "Debugger.h" // due to global usbUart
//#include "SensingBatteryStatus.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
void GatewayBaseState::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    return;
 }

void GatewayBaseState::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void GatewayBaseState::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void GatewayBaseState::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}

void GatewayBaseState::sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, 
NonBlockingDelay * backoffTime) {
    return; 
}

bool GatewayBaseState::waitForMessage (LoRaClass * LoRaModule, char * messageRecieved,
 NonBlockingDelay * timeOut){
    return false;
 }


void GatewayBaseState::formatMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo, GNSSData* receptedGNSSdata,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus) {
    return;
}

void GatewayBaseState::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}

void GatewayBaseState::exchangeMessages (LoRaClass * LoRaModule, char * message, char * receivedMessage) {
    return;
}


void GatewayBaseState::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void GatewayBaseState::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency, 
NonBlockingDelay * silentTimer ) {
    return;
 }

void GatewayBaseState::connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay){
    return;
 }

void GatewayBaseState::exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay, char * payload){
    return;
 }

void GatewayBaseState::queryUTCTimeViaRemoteServer (UipEthernet * ethernetModule, NonBlockingDelay * delay){
    return;
 }

//=====[Implementations of private methods]==================================