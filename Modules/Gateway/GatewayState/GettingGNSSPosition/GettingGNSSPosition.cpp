//=====[Libraries]=============================================================
#include "GettingGNSSPosition.h"
#include "Gateway.h"
#include "Debugger.h" // due to global usbUart
#include "ConnectingToMobileNetwork.h"
#include "FormattingMessage.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
GettingGNSSPosition::GettingGNSSPosition (Gateway * gateway) {
    this->gateway = gateway;
}

GettingGNSSPosition::~GettingGNSSPosition () {
    this->gateway = nullptr;
}

void GettingGNSSPosition::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GettingGNSSPosition::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
   static GNSSState_t GnssCurrentStatus;
   char logMessage [40]; 
   OperationMode_t operationMode = this->gateway->getOperationMode();

    // SIN GNSS
    //this->gateway->changeState  (new ConnectingToMobileNetwork (this->gateway, GATEWAY_STATUS_GNSS_UNAVAILABLE));
    //return;

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

