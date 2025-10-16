//=====[Libraries]=============================================================
#include "ConfigurateConstellation.h" 
#include "GNSSModule.h"
#include "Debugger.h" // due to global usbUart
#include "TurningOffGNSS.h"

//=====[Declaration of private defines]========================================
#define MAX_RETRIES  20

#define LOG_MESSAGE "Configuring GNSS Constellations\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define LOG_MESSAGE_NO_GNSS_WARNING "\r\n**GNSS UNAVAILABLE!**\r\n"
#define LOG_MESSAGE_NO_GNSS_WARNING_LEN (sizeof(LOG_MESSAGE_NO_GNSS_WARNING) - 1)

#define AT_CMD_CONFIGURE_GNSS_CONSTELATIONS "AT+QGPSCFG=\"gnssconfig\",4" // GPS + BEIDOU
#define AT_CMD_CONFIGURE_GNSS_CONSTELATIONS_LEN  (sizeof(AT_CMD_CONFIGURE_GNSS_CONSTELATIONS) - 1)

#define AT_CMD_CONFIGURE_GNSS_CONSTELATIONS_EXPECTED_RESPONSE "OK" // GPS + BEIDOU
#define AT_CMD_CONFIGURE_GNSS_CONSTELATIONS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CONFIGURE_GNSS_CONSTELATIONS) - 1)

#define BUFFER_LEN 128
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
 ConfigurateConstellation::ConfigurateConstellation () {
    this->currentGNSSModule = nullptr;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAX_RETRIES;
}

ConfigurateConstellation::ConfigurateConstellation  (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;

    this->numberOfTries = 0;
    this->maxTries = MAX_RETRIES;
}

ConfigurateConstellation::~ConfigurateConstellation  () {
    this->currentGNSSModule = nullptr;
}

GNSSState_t  ConfigurateConstellation::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
    char StringToSend [AT_CMD_CONFIGURE_GNSS_CONSTELATIONS_LEN + 1] = AT_CMD_CONFIGURE_GNSS_CONSTELATIONS; // 4 == Beidou and GPS
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CONFIGURE_GNSS_CONSTELATIONS_EXPECTED_RESPONSE_LEN + 1] =  AT_CMD_CONFIGURE_GNSS_CONSTELATIONS_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

    if (Geodata == nullptr || ATHandler == nullptr || refreshTime == nullptr) {
        return GNSS_STATE_ERROR_NULL_POINTER;
    }

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

    if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
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
    

        if (this->numberOfTries >= this->maxTries) {
             ////   ////   ////   ////   ////   ////
            uartUSB.write (LOG_MESSAGE_NO_GNSS_WARNING , strlen (LOG_MESSAGE_NO_GNSS_WARNING ));  // debug only
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
