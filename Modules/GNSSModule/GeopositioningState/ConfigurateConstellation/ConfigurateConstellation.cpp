//=====[Libraries]=============================================================

#include "ConfigurateConstellation.h" 
#include "GNSSModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "TurningOffGNSS.h"

//=====[Declaration of private defines]========================================
#define MAXRETRIES  10

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
 ConfigurateConstellation::ConfigurateConstellation () {
    this->currentGNSSModule = NULL;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAXRETRIES;
    
}


/** 
* @brief
* 
* @param 
*/
ConfigurateConstellation::ConfigurateConstellation  (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAXRETRIES;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ConfigurateConstellation::~ConfigurateConstellation  () {
    this->currentGNSSModule = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
GNSSState_t  ConfigurateConstellation::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [28] = "AT+QGPSCFG=\"gnssconfig\",4";
    char StringToBeRead [50];
    char ExpectedResponse [3] = "OK";
    char StringToSendUSB [40] = "CONFIGURE GNSS CONSTELLATION";

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
         if (strcmp (StringToBeRead, ExpectedResponse) == 0 ) {
            this->currentGNSSModule->changeGeopositioningState (new TurningOnGNSS (this->currentGNSSModule));
            return GNSS_STATE_TRYING_TO_CONNECT;
         }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;    
        this->numberOfTries ++;
    
        char StringToSendUSB [40] = "+1 counter retry";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (this->numberOfTries >= this->maxTries) {
             ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "GNSS UNAVAILABLE, TURNING OFF";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////    
            this->numberOfTries = 0;
            this->currentGNSSModule->changeGeopositioningState (new TurningOffGNSS (this->currentGNSSModule));
            return GNSS_STATE_CONNECTION_UNAVAILABLE;
        }
       
    }
    return GNSS_STATE_TRYING_TO_CONNECT;
}


void ConfigurateConstellation::enableGNSS ()  {
    return;
}



//=====[Implementations of private functions]==================================
