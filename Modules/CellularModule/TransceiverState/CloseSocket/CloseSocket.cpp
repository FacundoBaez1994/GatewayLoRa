//=====[Libraries]=============================================================

#include "CloseSocket.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_CLOSE_SOCKET   "AT+QICLOSE=" // AT+QICLOSE=<connectID>[,<timeout>] 
#define AT_CMD_CLOSE_SOCKET_LEN  (sizeof(AT_CMD_CLOSE_SOCKET) - 1)

#define AT_CMD_CLOSE_SOCKET_EXPECTED_RESPONSE   "OK"
#define AT_CMD_CLOSE_SOCKET_EXPECTED_RESPONSE_LEN  (sizeof( AT_CMD_CLOSE_SOCKET_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE "Closing Socket\r\n"
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

CloseSocket::CloseSocket (CellularModule * mobileModule, bool transmissionWasASuccess) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionWasASuccess = transmissionWasASuccess;
}

CloseSocket::~CloseSocket () {
    this->mobileNetworkModule = nullptr;
}

void CloseSocket::enableTransceiver () {
    return;
}

 CellularTransceiverStatus_t CloseSocket::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeSend [AT_CMD_CLOSE_SOCKET_LEN + 3];
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CLOSE_SOCKET_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CLOSE_SOCKET_EXPECTED_RESPONSE; 
    char StringToBeSendUSB [LOG_MESSAGE_LEN + 1]  = LOG_MESSAGE; 
    char ATcommand[AT_CMD_CLOSE_SOCKET_LEN + 1] = AT_CMD_CLOSE_SOCKET ; 
    // timeout default = 10 (seconds)
    int connectID = 0; // could be between 0 and 11

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommand, connectID); // AT+QICLOSE=<connectID>[,<timeout>] 

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        uartUSB.write (StringToBeSendUSB , strlen (StringToBeSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }


    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if ((strcmp (StringToBeRead, ExpectedResponse) == 0) ) {
            this->mobileNetworkModule->changeTransceiverState 
            (new DeactivatePDP (this->mobileNetworkModule, this->transmissionWasASuccess) );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            this->mobileNetworkModule->changeTransceiverState 
            (new DeactivatePDP (this->mobileNetworkModule, this->transmissionWasASuccess));
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
