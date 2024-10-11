//=====[Libraries]=============================================================

#include "TurningOnGNSS.h" 
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
TurningOnGNSS::TurningOnGNSS () {
    this->currentGNSSModule = NULL;
    this->readyToSend = true;
}


/** 
* @brief
* 
* @param 
*/
TurningOnGNSS::TurningOnGNSS   (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}


/** 
* @brief 
* 
* 
* @returns 
*/
TurningOnGNSS::~TurningOnGNSS  () {
    this->currentGNSSModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
GNSSState_t TurningOnGNSS::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [15] = "AT+QGPS=1";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char AlreadyLockResponse [20] = "+CME ERROR: 504";
    char StringToSendUSB [40] = "TURNING ON GNSS";



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
        ||strcmp (StringToBeRead, AlreadyLockResponse )  ) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS Locked!";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->currentGNSSModule->changeGeopositioningState (new ObtainingPositionInformation (this->currentGNSSModule));
            return GNSS_STATE_TRYING_TO_CONNECT;
        }


    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
    //
    //
    //
    return GNSS_STATE_TRYING_TO_CONNECT;
}

void TurningOnGNSS::enableGNSS ()  {
    return;
}



//=====[Implementations of private functions]==================================
