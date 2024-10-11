//=====[Libraries]=============================================================

#include "GNSSUnavailable.h" 
#include "GNSSModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

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
GNSSUnavailable::GNSSUnavailable () {
    this->currentGNSSModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
GNSSUnavailable::GNSSUnavailable  (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
GNSSUnavailable::~GNSSUnavailable  () {
    this->currentGNSSModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
GNSSState_t GNSSUnavailable::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
    return GNSS_STATE_DISCONNECTED;
}

void GNSSUnavailable::enableGNSS ()  {
    this->currentGNSSModule->changeGeopositioningState(new ConfigurateConstellation (this->currentGNSSModule));
}

//=====[Implementations of private functions]==================================
