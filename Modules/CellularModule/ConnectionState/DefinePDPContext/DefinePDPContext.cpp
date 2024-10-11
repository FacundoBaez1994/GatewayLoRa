//=====[Libraries]=============================================================

#include "DefinePDPContext.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define APN_MOVISTAR "AT+CGDCONT=1,\"IP\",\"internet.movistar.com.ar\"" //APN / username / password   internet.gprs.unifon.com.ar
#define MAXATTEMPTS 20
//=====[Declaration of private sdata types]=====================================

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
DefinePDPContext::DefinePDPContext () {
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
DefinePDPContext::DefinePDPContext (CellularModule * mobileModule) {
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
DefinePDPContext::~DefinePDPContext () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t DefinePDPContext::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSend [50] = APN_MOVISTAR;
    char StringToSendUSB [40] = "DEFINING PDP CONTEXT";

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(APN_MOVISTAR);
        this->readyToSend  = false;
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
            char StringToSendUSB [40] = "Cambiando de estado 7";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////        
            this->mobileNetworkModule->changeConnectionState (new ConnectedState (this->mobileNetworkModule));
            return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
        }
    }
    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            return CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_SET_PDP_CONTEXT;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;

}

bool DefinePDPContext::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}

void DefinePDPContext::enableConnection () {
    return;
}

//=====[Implementations of private functions]==================================
