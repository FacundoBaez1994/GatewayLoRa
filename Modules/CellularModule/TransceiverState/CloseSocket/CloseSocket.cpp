//=====[Libraries]=============================================================

#include "CloseSocket.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================

/** 
* @brief
* 
* @param 
*/
CloseSocket::CloseSocket (CellularModule * mobileModule, bool transmissionWasASuccess) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionWasASuccess = transmissionWasASuccess;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CloseSocket::~CloseSocket () {
    this->mobileNetworkModule = NULL;
}




void CloseSocket::enableTransceiver () {
    return;
}


/** 
* @brief 
* 
* 
* @returns 
*/
 CellularTransceiverStatus_t CloseSocket::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeSend [120];
    char StringToBeRead [20];
    char StringToBeSendUSB []  = "CLOSING TCP SOCKET"; 
    char ExpectedResponse [5] = "OK"; 
    char ATcommand[] = "AT+QICLOSE="; // AT+QICLOSE=<connectID>[,<timeout>] 
    // timeout default = 10 (seconds)
    int connectID = 0; // Puede ser entre 0 y 11

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommand, connectID);

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeSendUSB , strlen (StringToBeSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }


    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////
        if ((strcmp (StringToBeRead, ExpectedResponse) == 0) ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Cambiando de estado 4";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////    
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
