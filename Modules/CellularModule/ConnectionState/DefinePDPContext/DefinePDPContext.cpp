//=====[Libraries]=============================================================

#include "DefinePDPContext.h"
#include "CellularModule.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
//#define APN_MOVISTAR "AT+CGDCONT=1,\"IP\",\"internet.movistar.com.ar\"" //APN / username / password   internet.gprs.unifon.com.ar
//#define APN_TUENTI "AT+CGDCONT=1,\"IP\",\"internet.movil\"" // APN de Tuenti Argentina

#define MAXATTEMPTS 20
//#define APN_MOVISTAR "AT+CGDCONT=1,\"IP\",\"internet.movistar.com.ar\"" //APN / username / password  movistar
#define APN "AT+CGDCONT=1,\"IP\",\"internet.movil\"" //APN / username / password   tuenti
#define APN_LEN  (sizeof(APN) - 1)

#define AT_CMD_DEFINE_PDP_CONTEXT_EXPECTED_RESPONSE     "OK"
#define AT_CMD_DEFINE_PDP_CONTEXT_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_DEFINE_PDP_CONTEXT_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE "Defining PDP Context\r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define BUFFER_LEN 128
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

DefinePDPContext::DefinePDPContext () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}

DefinePDPContext::DefinePDPContext (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}


DefinePDPContext::~DefinePDPContext () {
    this->mobileNetworkModule = nullptr;
}


CellularConnectionStatus_t DefinePDPContext::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
CellInformation * currentCellInformation) {
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_DEFINE_PDP_CONTEXT_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_DEFINE_PDP_CONTEXT_EXPECTED_RESPONSE;
    char StringToSend [APN_LEN + 1] = APN;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

    if (ATHandler == nullptr || refreshTime == nullptr || currentCellInformation == nullptr) {
        return CELLULAR_CONNECTION_STATUS_ERROR_NULL_POINTER;
    }


    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend);
        this->readyToSend  = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }

    if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            this->mobileNetworkModule->changeConnectionState (new ConnectedState (this->mobileNetworkModule));
            return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
        }
    }
    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            this->connectionAttempts = 0;
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
