//=====[Libraries]=============================================================

#include "AttachingToPacketService.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_ATTACHING_TO_PACKET_SERVICE      "AT+CGATT=1"
#define AT_CMD_ATTACHING_TO_PACKET_SERVICE_LEN  (sizeof(AT_CMD_ATTACHING_TO_PACKET_SERVICE) - 1)

#define AT_CMD_ATTACHING_TO_PACKET_SERVICE_EXPECTED_RESPONSE     "OK"
#define AT_CMD_ATTACHING_TO_PACKET_SERVICE_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_ATTACHING_TO_PACKET_SERVICE_EXPECTED_RESPONSE) - 1)

#define BUFFER_LEN 128
#define LOG_MESSAGE "Retriving Neighboor Cells Data\r\n"
#define LOG_MESSAGE_LEN  (sizeof(LOG_MESSAGE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============



//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================


//=====[Implementations of public methods]===================================

AttachingToPacketService::AttachingToPacketService () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}

AttachingToPacketService::AttachingToPacketService (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS;
}


AttachingToPacketService::~AttachingToPacketService () {
    this->mobileNetworkModule = nullptr;
}


void AttachingToPacketService::enableConnection () {
    return;
}


CellularConnectionStatus_t AttachingToPacketService::connect (ATCommandHandler * ATHandler, 
NonBlockingDelay * refreshTime,
 CellInformation * currentCellInformation) {
    char StringToSend [AT_CMD_ATTACHING_TO_PACKET_SERVICE_LEN + 1] = AT_CMD_ATTACHING_TO_PACKET_SERVICE;
    char StringToBeRead [BUFFER_LEN];
    char ExpectedResponse [AT_CMD_ATTACHING_TO_PACKET_SERVICE_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_ATTACHING_TO_PACKET_SERVICE_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_LEN + 1] = LOG_MESSAGE;

    if (ATHandler == nullptr || refreshTime == nullptr || currentCellInformation == nullptr) {
        return CELLULAR_CONNECTION_STATUS_ERROR_NULL_POINTER;
    }

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToSend, AT_CMD_ATTACHING_TO_PACKET_SERVICE_LEN);
        this->readyToSend = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }

    if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) { 
            this->connectionAttempts = 0;       
            this->mobileNetworkModule->changeConnectionState (new DefinePDPContext (this->mobileNetworkModule));
            return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
            this->connectionAttempts = 0; 
            return CELLULAR_CONNECTION_STATUS_UNAVAIBLE_TO_ATTACH_TO_PACKET_SERVICE;
        }
    }
    return CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT;
}


bool AttachingToPacketService::retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors) {
        return false;
    }


//=====[Implementations of private functions]==================================
