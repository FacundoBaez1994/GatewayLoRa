//=====[Libraries]=============================================================
#include "GettingGNSSPosition.h"
#include "Gateway.h"
#include "Debugger.h" // due to global usbUart
#include "ConnectingToMobileNetwork.h"
#include "FormattingMessage.h"
#include "WaitingForMessage.h"

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
   char logMessage [50]; 
   static bool moduleTurningOff = false; 

    // SIN GNSS
    //this->gateway->changeState  (new ConnectingToMobileNetwork (this->gateway, GATEWAY_STATUS_GNSS_UNAVAILABLE));
    //return;
    if (moduleTurningOff == true) {
        if (currentGNSSModule->turnOff()) {
            snprintf(logMessage, sizeof(logMessage), "Waiting For LoRa Message from the Tracker");
            uartUSB.write (logMessage , strlen (logMessage ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->gateway->changeState  (new WaitingForMessage (this->gateway));
            moduleTurningOff = false;
            return;
        }
        return;
    }
    

    currentGNSSModule->enableGNSS();
    GnssCurrentStatus = currentGNSSModule->retrivGeopositioning(currentGNSSdata);
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_OBTAIN ) {
        snprintf(logMessage, sizeof(logMessage), "GNSS OBTAIN!!!!");
        uartUSB.write (logMessage , strlen (logMessage ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        moduleTurningOff = true;
    }
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_UNAVAILABLE ) {
        snprintf(logMessage, sizeof(logMessage), "GNSS UNAVAILABLE!!!!");
        uartUSB.write (logMessage , strlen (logMessage ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        moduleTurningOff = true;
    }

    return;
}

