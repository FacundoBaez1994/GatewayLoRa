//=====[Libraries]=============================================================

#include "IdleState.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_CHECK_MODULE_RESPONSE    "ATI"
#define AT_CMD_CHECK_MODULE_RESPONSE_LEN  (sizeof(AT_CMD_CHECK_MODULE_RESPONSE) - 1)

#define AT_CMD_CHECK_MODULE_RESPONSE_EXPECTED_RESPONSE     "OK"
#define AT_CMD_CHECK_MODULE_RESPONSE_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_CHECK_MODULE_RESPONSE_EXPECTED_RESPONSE ) - 1)

#define BUFFER_LEN 128

#define LOG_MESSAGE "Idle State\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================

IdleState::IdleState () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}

IdleState::IdleState (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}

IdleState::~IdleState () {
    this->mobileNetworkModule = nullptr;
}

CellularConnectionStatus_t  IdleState::connect (ATCommandHandler * ATHandler, NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    char StringToSend [AT_CMD_CHECK_MODULE_RESPONSE_LEN + 1] = AT_CMD_CHECK_MODULE_RESPONSE;
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_CHECK_MODULE_RESPONSE_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_CHECK_MODULE_RESPONSE_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

    if (ATHandler == nullptr || refreshTime == nullptr || currentCellInformation == nullptr) {
        return CELLULAR_CONNECTION_STATUS_ERROR_NULL_POINTER;
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
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) { 
            this->mobileNetworkModule->changeConnectionState (new CheckingSignalStrength (this->mobileNetworkModule));
        return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            return CELLULAR_CONNECTION_STATUS_MODULE_DISCONNECTED;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}

bool IdleState::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
}

void IdleState::enableConnection () {
    return;
}



//=====[Implementations of private functions]==================================
