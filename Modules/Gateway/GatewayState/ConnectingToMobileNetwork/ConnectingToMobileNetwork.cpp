//=====[Libraries]=============================================================
#include "ConnectingToMobileNetwork.h"
#include "FormattingMessage.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GoingToSleep.h"


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
ConnectingToMobileNetwork::ConnectingToMobileNetwork (Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->currentStatus = gatewayStatus;
    this->gateway = gateway;
}

ConnectingToMobileNetwork::ConnectingToMobileNetwork (Gateway * gateway) {
    this->gateway = gateway;
}

ConnectingToMobileNetwork::~ConnectingToMobileNetwork ( ) {
    this->gateway = nullptr;
}

void ConnectingToMobileNetwork::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

 void ConnectingToMobileNetwork::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    static CellularConnectionStatus_t currentConnectionStatus;
    char logMessage [40];
    
    cellularTransceiver->enableConnection();
    currentConnectionStatus = cellularTransceiver->connectToMobileNetwork (currentCellInformation);
    
    /// test only
   //currentConnectionStatus = CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_ATTACH_TO_PACKET_SERVICE;
   // this->currentStatus = GATEWAY_STATUS_GNSS_OBTAIN;
    /// test only

    if (currentConnectionStatus == CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK){
        this->gateway->changeState (new FormattingMessage (this->gateway));
        return;

    }  else if (currentConnectionStatus != CELLULAR_CONNECTION_STATUS_UNAVAIBLE && 
        currentConnectionStatus != CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT) {
        this->gateway->changeState  (new GoingToSleep (this->gateway));
        return;
    } 
    return;
}






