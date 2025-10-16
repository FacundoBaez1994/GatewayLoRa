//=====[Libraries]=============================================================

#include "DeactivatePDP.h"
#include "CellularModule.h" 
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define AT_CMD_DEACTIVATE_PDP "AT+QIDEACT="
#define AT_CMD_DEACTIVATE_PDP_LEN  (sizeof(AT_CMD_DEACTIVATE_PDP) - 1)

#define AT_CMD_DEACTIVATE_PDP_EXPECTED_RESPONSE "OK"
#define AT_CMD_DEACTIVATE_PDP_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_DEACTIVATE_PDP_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE "Deactivate PDP \r\n"
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

DeactivatePDP::DeactivatePDP (CellularModule * mobileModule, bool transmissionWasASuccess ) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionWasASuccess = transmissionWasASuccess;
}

DeactivatePDP::~DeactivatePDP () {
    this->mobileNetworkModule = nullptr;
}
void DeactivatePDP::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t DeactivatePDP::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_DEACTIVATE_PDP_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_DEACTIVATE_PDP_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] =   LOG_MESSAGE;
    char StringToBeSend [AT_CMD_DEACTIVATE_PDP_LEN + 3];
    char ATcommand[AT_CMD_DEACTIVATE_PDP_LEN + 1] = AT_CMD_DEACTIVATE_PDP; 
    int connectID = 0; 
    int contextID = 1; 

    if (ATHandler == nullptr ||  refreshTime == nullptr || 
     message == nullptr || receivedMessage == nullptr || serverTargetted == nullptr) {
        return CELLULAR_TRANSCEIVER_STATUS_ERROR_NULL_POINTER;
    }

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommand, contextID );

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////     
            this->mobileNetworkModule->changeTransceiverState (new 
            TransceiverUnavailable (this->mobileNetworkModule));
            if (this->transmissionWasASuccess == true) {
                return CELLULAR_TRANSCEIVER_STATUS_SEND_OK;
            } else {
                return CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE_TO_SEND;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
