//=====[Libraries]=============================================================
#include "GNSSModule.h"
#include "Debugger.h"
#include "TurningOnGNSS.h"
//#include "TransmissionUnavailable.h"


//=====[Declaration of private defines]========================================
#define REFRESHTIME  1000
//#define GNSS_MODULE_TX_UART PA_2
//#define GNSS_MODULE_RX_UART PA_3
#define GNSS_MODULE_BAUD_RATE 115200


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================s

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================
/** 
* @brief r
*/


//=====[Implementations of public methods]===================================
/** 
* 
* 
* 
*/
GNSSModule::GNSSModule (PowerManager * aPowerManager, ATCommandHandler * ATHandler) {
    this->refreshTime = new NonBlockingDelay (REFRESHTIME);
    this->ATHandler =  ATHandler;
    this->modulePowerManager = aPowerManager;
    this->currentGeopositioningState = new GNSSUnavailable (this);

    this->currentPowerStatus = POWER_OFF;
}


/** 
* 
* 
* 
*/
GNSSModule::~GNSSModule () {
    delete this->refreshTime;
    this->refreshTime = NULL;
    delete  this->ATHandler;
    this->ATHandler = NULL;
    this->modulePowerManager = NULL;
}    
/** 
* 
* 
* SOFT HARDWARE START STOP
*/
void GNSSModule::startStopUpdate () {
    powerStatus_t newPowerStatus;
    newPowerStatus = this->modulePowerManager->startStopUpdate();
    if (this->currentPowerStatus != newPowerStatus) {
        this->currentPowerStatus = newPowerStatus;
        if (this->currentPowerStatus != POWER_ON) {
            this->changeGeopositioningState (new GNSSUnavailable (this));
       }
    }
}

/** 
* 
* 
* SOFT HARDWARE START STOP
*/
void GNSSModule::reboot () {
    this->modulePowerManager->reboot();
}

/** 
* 
* 
* 
*/

/** 
* @brief 
* 
* @returns 
*/
GNSSState_t GNSSModule::retrivGeopositioning (GNSSData * Geodata) {
    return this->currentGeopositioningState->retrivGeopositioning (Geodata, this->ATHandler,
    this->refreshTime);
}


/** 
* @brief 
* 
* @returns 
*/
void GNSSModule::changeGeopositioningState  (GeopositioningState* newGeopositioningState) {
    delete this->currentGeopositioningState;
    this->currentGeopositioningState = newGeopositioningState;
}


/** 
* @brief 
* 
* @returns 
*/

void GNSSModule::enableGNSS () {
    this->currentGeopositioningState->enableGNSS();
}

/** 
* @brief 
* 
* @returns 
*/
 BufferedSerial* GNSSModule::getUART () {
     return this->ATHandler->getUART();
}

 PowerManager* GNSSModule::getPowerManager (){
    return this->modulePowerManager;
}

//=====[Implementations of private functions]==================================

