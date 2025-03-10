//=====[Libraries]=============================================================

#include "GatheringCellInformation.h"
#include "Tracker.h" //debido a declaracion adelantada
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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


/** 
* @brief
* 
* @param 
*/
GatheringCellInformation::GatheringCellInformation (Tracker * tracker) {
    this->tracker = tracker;
}

/** 
* @brief
* 
* @param 
*/
GatheringCellInformation::~GatheringCellInformation () {
    this->tracker = NULL;
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
     numberOfNeighbors) == true){
         this->tracker->changeState (new FormattingMessage (this->tracker, true, false));
         return;
     }
    // Format message for LoRa 


    return; 
}
    // IMU Method 1
    // IMU Methord 2
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




