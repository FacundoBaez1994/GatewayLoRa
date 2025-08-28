//=====[Libraries]=============================================================

#include "GatheringCellInformation.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "FormattingMessage.h"


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
GatheringCellInformation::GatheringCellInformation (Gateway * gateway) {
    this->gateway = gateway;
}

GatheringCellInformation::~GatheringCellInformation () {
    this->gateway = nullptr;
}

void GatheringCellInformation::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void GatheringCellInformation::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {

    if (cellularTransceiver->retrivNeighborCellsInformation(neighborsCellInformation,
     numberOfNeighbors) == true){
         this->gateway->changeState (new FormattingMessage (this->gateway, 
         GATEWAY_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK));
         return;
     }
    return; 
}







