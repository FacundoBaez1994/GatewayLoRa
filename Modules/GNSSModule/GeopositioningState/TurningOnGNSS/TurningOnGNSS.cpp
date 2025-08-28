//=====[Libraries]=============================================================
#include "TurningOnGNSS.h" 
#include "GNSSModule.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE "Turning On GNSS\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define LOG_MESSAGE_GNSS_WAS_TURN_ON "Turn On GNSS\r\n"
#define LOG_MESSAGE_GNSS_WAS_TURN_ON_LEN (sizeof(LOG_MESSAGE_GNSS_WAS_TURN_ON ) - 1)

#define AT_CMD_TURNING_ON_GNSS "AT+QGPS=1"
#define AT_CMD_TURNING_ON_GNSS_LEN  (sizeof(AT_CMD_TURNING_ON_GNSS) - 1)

#define AT_CMD_TURNING_ON_GNSS_EXPECTED_RESPONSE "OK"
#define AT_CMD_TURNING_ON_GNSS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_TURNING_ON_GNSS_EXPECTED_RESPONSE) - 1)

#define AT_CMD_TURNING_ON_GNSS_RESPONSE_ALREADY_ON "+CME ERROR: 504"
#define AT_CMD_TURNING_ON_GNSS_RESPONSE_ALREADY_ON_LEN  (sizeof(AT_CMD_TURNING_ON_GNSS_RESPONSE_ALREADY_ON) - 1)

#define LOG_MESSAGE_GNSS_LOCKED "GNSS Locked!\r\n"
#define LOG_MESSAGE_GNSS_LOCKED_LEN (sizeof(LOG_MESSAGE_GNSS_LOCKED ) - 1)

#define BUFFER_LEN 128

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
TurningOnGNSS::TurningOnGNSS () {
    this->currentGNSSModule = nullptr;
    this->readyToSend = true;
}

TurningOnGNSS::TurningOnGNSS   (GNSSModule* aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}

TurningOnGNSS::~TurningOnGNSS  () {
    this->currentGNSSModule = nullptr;
}

GNSSState_t TurningOnGNSS::retrivGeopositioning (GNSSData* Geodata, ATCommandHandler* ATHandler,
     NonBlockingDelay* refreshTime)  {
 
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_TURNING_ON_GNSS_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_TURNING_ON_GNSS_EXPECTED_RESPONSE;
    char AlreadyLockResponse [AT_CMD_TURNING_ON_GNSS_RESPONSE_ALREADY_ON_LEN + 1] = AT_CMD_TURNING_ON_GNSS_RESPONSE_ALREADY_ON;
    char StringToSend [AT_CMD_TURNING_ON_GNSS_LEN + 1] =  AT_CMD_TURNING_ON_GNSS ;

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
            uartUSB.write (LOG_MESSAGE_GNSS_LOCKED  , strlen (LOG_MESSAGE_GNSS_LOCKED  ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->currentGNSSModule->changeGeopositioningState (new ObtainingPositionInformation (this->currentGNSSModule));
            return GNSS_STATE_TRYING_TO_CONNECT;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }

    return GNSS_STATE_TRYING_TO_CONNECT;
}

void TurningOnGNSS::enableGNSS ()  {
    return;
}



//=====[Implementations of private functions]==================================
