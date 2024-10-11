//=====[Libraries]=============================================================

#include "ConnectionUnavailableState.h"
#include "CellularModule.h" //debido a declaracion adelantada
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
ConnectionUnavailableState::ConnectionUnavailableState () {
    this->mobileNetworkModule = NULL;
    this->enableTransceiver = false;
}


/** 
* @brief
* 
* @param 
*/
ConnectionUnavailableState::ConnectionUnavailableState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->enableTransceiver = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConnectionUnavailableState::~ConnectionUnavailableState () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t ConnectionUnavailableState::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation) {
    return CELLULAR_CONNECTION_STATUS_UNAVAIBLE;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void ConnectionUnavailableState::enableConnection () {
    this->mobileNetworkModule->changeConnectionState (new IdleState (this->mobileNetworkModule));
    return;
}



/** 
* @brief 
* 
* 
* @returns 
*/
bool ConnectionUnavailableState::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return true;
}




//=====[Implementations of private functions]==================================
