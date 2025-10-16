//=====[Libraries]=============================================================

#include "ActivatePDP.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_ACTIVATE_PDP   "AT+CGACT=1,1"
#define AT_CMD_ACTIVATE_PDP_LEN  (sizeof(AT_CMD_ACTIVATE_PDP) - 1)

#define AT_CMD_ACTIVATE_PDP_EXPECTED_RESPONSE   "OK"
#define AT_CMD_ACTIVATE_PDP_EXPECTED_RESPONSE_LEN  (sizeof( AT_CMD_ACTIVATE_PDP_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE "Activating PDP Context\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define BUFFER_LEN 128
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ActivatePDP::ActivatePDP () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
}

ActivatePDP::ActivatePDP (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS; 
}


ActivatePDP::~ActivatePDP () {
    this->mobileNetworkModule = nullptr;
    this->Attempts = 0; 
}

void ActivatePDP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t ActivatePDP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_ACTIVATE_PDP_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_ACTIVATE_PDP_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] =  LOG_MESSAGE;
    char StringToSend [AT_CMD_ACTIVATE_PDP_LEN + 1] =  AT_CMD_ACTIVATE_PDP;

    if (ATHandler == nullptr ||  refreshTime == nullptr || 
     message == nullptr || receivedMessage == nullptr || serverTargetted == nullptr) {
        return CELLULAR_TRANSCEIVER_STATUS_ERROR_NULL_POINTER;
    }
 
    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend  = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }

    if ( ATHandler->readATResponse (StringToBeRead, BUFFER_LEN) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            this->Attempts = 0; 
            this->mobileNetworkModule->changeTransceiverState (new CreateSocket (this->mobileNetworkModule));
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            this->Attempts = 0; 
            return CELLULAR_TRANSCEIVER_STATUS_FAIL_TO_ACTIVATE_PDP;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
