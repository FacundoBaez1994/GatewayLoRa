//=====[Libraries]=============================================================
#include "ConnectionUnavailableState.h"
#include "CellularModule.h" 
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
ConnectionUnavailableState::ConnectionUnavailableState () {
    this->mobileNetworkModule = nullptr;
    this->enableTransceiver = false;
}

ConnectionUnavailableState::ConnectionUnavailableState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->enableTransceiver = false;
}

ConnectionUnavailableState::~ConnectionUnavailableState () {
    this->mobileNetworkModule = nullptr;
}

CellularConnectionStatus_t ConnectionUnavailableState::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation) {
    return CELLULAR_CONNECTION_STATUS_UNAVAIBLE;
}

void ConnectionUnavailableState::enableConnection () {
    this->mobileNetworkModule->changeConnectionState (new IdleState (this->mobileNetworkModule));
    return;
}

bool ConnectionUnavailableState::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return true;
}

//=====[Implementations of private functions]==================================
