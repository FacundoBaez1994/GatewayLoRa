//=====[Libraries]=============================================================

#include "CreateSocket.h"
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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
CreateSocket::CreateSocket () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}


/** 
* @brief
* 
* @param 
*/
CreateSocket::CreateSocket (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}


/** 
* @brief 
* 
* 
* @returns 
*/
CreateSocket::~CreateSocket () {
    this->mobileNetworkModule = NULL;
}


void CreateSocket::enableTransceiver () {
    return;
}


/** 
* @brief 
* 
* 
* @returns 
*/
 CellularTransceiverStatus_t  CreateSocket::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeSend [120];
    char StringToBeRead [20];
    char StringToBeSendUSB []  = "CREATE TCP SOCKET"; 
    char ExpectedResponseFirstPart [30] = "+QIOPEN: ";
    char ExpectedResponse [30]; 
    char ATcommand[] = "AT+QIOPEN=";
    char protocol[] = "\"TCP\"";
    int noErrorCode = 0;
    int contextID = 1; // Usualmente 1
    int connectID = 0; // Puede ser entre 0 y 11
    int access_mode = 0; // Modo de acceso al buffer

    // Formatear la cadena final
    int result = snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d,%d,%s,\"%s\",%d,%d", ATcommand, contextID,
     connectID, protocol, socketTargetted->IpDirection , socketTargetted->TcpPort , access_mode);
    snprintf(ExpectedResponse, sizeof(ExpectedResponse), "+QIOPEN: %d,%d", connectID, noErrorCode);

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

        if ((strstr(StringToBeRead, ExpectedResponse) != NULL) ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Cambiando de estado 2";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////    
            this->mobileNetworkModule->changeTransceiverState (new Sending (this->mobileNetworkModule));
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
             this->mobileNetworkModule->changeTransceiverState (new CloseSocket (this->mobileNetworkModule, false));
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
