//=====[Libraries]=============================================================

#include "TransceiverUnavailable.h"
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
TransceiverUnavailable::TransceiverUnavailable () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
TransceiverUnavailable::TransceiverUnavailable (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
TransceiverUnavailable::~TransceiverUnavailable () {
    this->mobileNetworkModule = NULL;
}

/**
* @brief 
* 
* 
* @returns 
*/
void TransceiverUnavailable::enableTransceiver () {
    this->mobileNetworkModule->changeTransceiverState (new 
        ActivatePDP (this->mobileNetworkModule));
    return;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CellularTransceiverStatus_t TransceiverUnavailable::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    return CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE;
}



//=====[Implementations of private functions]==================================
