//=====[Libraries]=============================================================

#include "GettingGNSSPosition.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "ConnectingToMobileNetwork.h"

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
GettingGNSSPosition::GettingGNSSPosition (Gateway * gateway) {
    this->gateway = gateway;
}

/** 
* @brief
* 
* @param 
*/
GettingGNSSPosition::~GettingGNSSPosition () {
    this->gateway = NULL;
}


void GettingGNSSPosition::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void GettingGNSSPosition::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void GettingGNSSPosition::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}


void GettingGNSSPosition::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GettingGNSSPosition::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
   static GNSSState_t GnssCurrentStatus;
   char logMessage [40]; 

    currentGNSSModule->enableGNSS();
    GnssCurrentStatus = currentGNSSModule->retrivGeopositioning(currentGNSSdata);
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_OBTAIN ) {
        snprintf(logMessage, sizeof(logMessage), "GNSS OBTAIN!!!!");
        uartUSB.write (logMessage , strlen (logMessage ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        this->gateway->changeState  (new ConnectingToMobileNetwork (this->gateway, GATEWAY_STATUS_GNSS_OBTAIN));
        return;
    }
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_UNAVAILABLE ) {
        snprintf(logMessage, sizeof(logMessage), "GNSS UNAVAILABLE!!!!");
        uartUSB.write (logMessage , strlen (logMessage ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        this->gateway->changeState  (new ConnectingToMobileNetwork (this->gateway, GATEWAY_STATUS_GNSS_UNAVAILABLE));
        return;
    }

    return;
}

void GettingGNSSPosition::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) { 
    return; 
}

void GettingGNSSPosition::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return;
}

void GettingGNSSPosition::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {

    return;
}

void GettingGNSSPosition::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    return;
}

void GettingGNSSPosition::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void GettingGNSSPosition::awake (CellularModule * cellularTransceiver, 
NonBlockingDelay * latency ) {
    return;
}




