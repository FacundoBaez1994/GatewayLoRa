//=====[Libraries]=============================================================

#include "AttachingToPacketService.h"
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
AttachingToPacketService::AttachingToPacketService () {
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
AttachingToPacketService::AttachingToPacketService (CellularModule * mobileModule) {
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
AttachingToPacketService::~AttachingToPacketService () {
    this->mobileNetworkModule = NULL;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void AttachingToPacketService::enableConnection () {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t AttachingToPacketService::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
 CellInformation * currentCellInformation) {
    char StringToSend [15] = "AT+CGATT=1";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";

    char StringToSendUSB [40] = "ATTACHING TO PACKET SERVICE DOMAIN";

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Cambiando de estado";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->mobileNetworkModule->changeConnectionState (new DefinePDPContext (this->mobileNetworkModule));
            return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            return CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_ATTACH_TO_PACKET_SERVICE;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}


bool AttachingToPacketService::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
    }


//=====[Implementations of private functions]==================================
