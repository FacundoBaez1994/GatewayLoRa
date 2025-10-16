//=====[Libraries]=============================================================
#include "TurningOffGNSS.h" 
#include "GNSSModule.h" 
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================


#define LOG_MESSAGE "Turning Off GNSS\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define LOG_MESSAGE_GNSS_WAS_TURN_OFF "Turn Off GNSS\r\n"
#define LOG_MESSAGE_GNSS_WAS_TURN_OFF_LEN (sizeof(LOG_MESSAGE_GNSS_WAS_TURN_OFF) - 1)

#define AT_CMD_TURNING_OFF_GNSS "AT+QGPS=0"
#define AT_CMD_TURNING_OFF_GNSS_LEN  (sizeof(AT_CMD_TURNING_OFF_GNSS) - 1)

#define AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE "OK"
#define AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE) - 1)

#define AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_THE_MODULE_IS_ALREADY_TURNOFF "+CME ERROR: 505"
#define AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_THE_MODULE_IS_ALREADY_TURNOFF_LEN  (sizeof(AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_THE_MODULE_IS_ALREADY_TURNOFF ) - 1)

#define BUFFER_LEN 128

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================
TurningOffGNSS::TurningOffGNSS () {
    this->currentGNSSModule = nullptr;
    this->readyToSend = true;
}

TurningOffGNSS::TurningOffGNSS   (GNSSModule * aGNSSModule) {
    this->currentGNSSModule = aGNSSModule;
    this->readyToSend = true;
}

TurningOffGNSS::~TurningOffGNSS  () {
    this->currentGNSSModule = nullptr;
}

GNSSState_t TurningOffGNSS::retrivGeopositioning (GNSSData * Geodata, ATCommandHandler * ATHandler,
     NonBlockingDelay * refreshTime)  {
 
    char StringToSend [AT_CMD_TURNING_OFF_GNSS_LEN + 1] = AT_CMD_TURNING_OFF_GNSS;
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE ;
    char AlreadyTurnOffResponse [AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_THE_MODULE_IS_ALREADY_TURNOFF_LEN + 1] = AT_CMD_TURNING_OFF_GNSS_EXPECTED_RESPONSE_THE_MODULE_IS_ALREADY_TURNOFF;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

    if (Geodata == nullptr || ATHandler == nullptr || refreshTime == nullptr) {
        return GNSS_STATE_ERROR_NULL_POINTER;
    }


    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }

    if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (strcmp (StringToBeRead, ExpectedResponse) == 0
        ||strcmp (StringToBeRead, AlreadyTurnOffResponse )  ) {
            uartUSB.write (LOG_MESSAGE_GNSS_WAS_TURN_OFF , strlen (LOG_MESSAGE_GNSS_WAS_TURN_OFF));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only        
            this->currentGNSSModule->changeGeopositioningState (new GNSSUnavailable (this->currentGNSSModule));
            return GNSS_STATE_DISCONNECTED;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
    }
    return GNSS_STATE_DISCONNECTED;
}

void TurningOffGNSS::enableGNSS ()  {
    return;
}
//=====[Implementations of private functions]==================================
