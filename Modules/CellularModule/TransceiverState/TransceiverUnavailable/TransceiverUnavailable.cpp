//=====[Libraries]=============================================================

#include "TransceiverUnavailable.h"
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

TransceiverUnavailable::TransceiverUnavailable () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
}

TransceiverUnavailable::TransceiverUnavailable (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
}


TransceiverUnavailable::~TransceiverUnavailable () {
    this->mobileNetworkModule = nullptr;
}

void TransceiverUnavailable::enableTransceiver () {
   // this->mobileNetworkModule->changeTransceiverState (new  ActivatePDP (this->mobileNetworkModule));
   this->mobileNetworkModule->changeTransceiverState (new ConfigurePDPForHTTP (this->mobileNetworkModule));
   
    return;
}

CellularTransceiverStatus_t TransceiverUnavailable::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    return CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE;
}


//=====[Implementations of private functions]==================================
