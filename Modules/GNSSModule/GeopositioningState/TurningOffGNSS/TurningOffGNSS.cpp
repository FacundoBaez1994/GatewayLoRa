//=====[Libraries]=============================================================

#include "TurningOffGNSS.h" 
#include "GNSSModule.h" //debido a declaracion adelantada
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
TurningOffGNSS::TurningOffGNSS () {
    this->currentGNSSModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
TurningOffGNSS::TurningOffGNSS   (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
TurningOffGNSS::~TurningOffGNSS  () {
    this->currentGNSSModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
GNSSState_t TurningOffGNSS::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [15] = "AT+QGPS=0";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char AlreadyTurnOffResponse [20] = "+CME ERROR: 505";
    char StringToSendUSB [40] = "TURNING OFF GNSS";


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
         ////   ////   ////   ////   ////   ///
        if (strcmp (StringToBeRead, ExpectedResponse) == 0
        ||strcmp (StringToBeRead, AlreadyTurnOffResponse )  ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS turned OFF";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->currentGNSSModule->changeGeopositioningState (new GNSSUnavailable (this->currentGNSSModule));
            return GNSS_STATE_DISCONNECTED;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
    //
    //
    //
    return GNSS_STATE_DISCONNECTED;
}

void TurningOffGNSS::enableGNSS ()  {
    return;
}



//=====[Implementations of private functions]==================================
