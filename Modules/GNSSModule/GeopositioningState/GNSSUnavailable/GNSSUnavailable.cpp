//=====[Libraries]=============================================================

#include "GNSSUnavailable.h" 
#include "GNSSModule.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
GNSSUnavailable::GNSSUnavailable () {
    this->currentGNSSModule = nullptr;
    this->readyToSend = true;
}

GNSSUnavailable::GNSSUnavailable  (GNSSModule* aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}

GNSSUnavailable::~GNSSUnavailable  () {
    this->currentGNSSModule = nullptr;
}

GNSSState_t GNSSUnavailable::retrivGeopositioning (GNSSData* Geodata, ATCommandHandler* ATHandler,
     NonBlockingDelay* refreshTime)  {
    return GNSS_STATE_DISCONNECTED;
}

void GNSSUnavailable::enableGNSS ()  {
    this->currentGNSSModule->changeGeopositioningState(new ConfigurateConstellation (this->currentGNSSModule));
}

//=====[Implementations of private functions]==================================
