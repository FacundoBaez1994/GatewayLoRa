//=====[Libraries]=============================================================

#include "ConsultingSIMCardStatus.h"
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
ConsultingSIMCardStatus::ConsultingSIMCardStatus (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->ATFirstResponseRead  = false;
    this->readyToSend = true;
    this->simCardDetected = false;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConsultingSIMCardStatus::~ConsultingSIMCardStatus () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CellularConnectionStatus_t ConsultingSIMCardStatus::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    static char StringToBeRead [256];
    char expectedResponse [15] = "OK";
    char noSimCardError [20] = "+CME ERROR: 10";
    char noSimCardError2 [20] = "+CME ERROR: 13";
    char simCardReady [20] = "+CPIN: READY";
    char StringToSend [15] = "AT+CPIN?";
   

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        ////   ////   ////   ////   ////   ////
        char StringToSendUSB [40] = "CONSULTING SIM CARD STATUS";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        refreshTime->restart();
        ////   ////   ////   ////   ////   ////   
    }

    if ( this->simCardDetected == false) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true ) {
            
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
             if (strcmp (StringToBeRead, noSimCardError) == 0 ||  strcmp (StringToBeRead, noSimCardError2) == 0) {
                this->mobileNetworkModule->switchSIMCARD();
                char StringToSendUSB [40] = "SWITCHING SIM CARD SLOT";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
            }
            if (strcmp (StringToBeRead, simCardReady ) == 0) {
                this->simCardDetected = true;
            }
            refreshTime->restart();
        }
    } 
     

    if (this->simCardDetected == true) {
        if  (ATHandler->readATResponse ( StringToBeRead) == true) {
            if (strcmp (StringToBeRead, expectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                uartUSB.write (StringToBeRead , strlen (StringToBeRead ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////     
                char StringToSendUSB [40] = "Cambiando de estado 3";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                ////   ////   ////   ////   ////   ////            
                this->mobileNetworkModule->changeConnectionState (new ConsultingNetworkStatus (this->mobileNetworkModule) );
                return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
            }
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->simCardDetected= false;
        this->connectionAttempts ++;
        if ( this->connectionAttempts >= this->maxConnectionAttempts) {
            this->mobileNetworkModule->reboot();
            return CELLULAR_CONNECTION_STATUS_INVALID_SIM;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool ConsultingSIMCardStatus::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}


void ConsultingSIMCardStatus::enableConnection () {
    return;
}