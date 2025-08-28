//=====[Libraries]=============================================================

#include "CreateSocket.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20

#define AT_CMD_TCP_OPEN_SOCKET_FIRST_PART   "AT+QIOPEN="
#define AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_LEN  (sizeof( AT_CMD_TCP_OPEN_SOCKET_FIRST_PART) - 1)

#define AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_EXPECTED_RESPONSE "+QIOPEN: "
#define AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_EXPECTED_RESPONSE) - 1)


#define TCP_HEADER "\"TCP\""
#define TCP_HEADER_LEN  (sizeof(TCP_HEADER) - 1)

#define LOG_MESSAGE "Create TCP Socket \r\n"
#define LOG_MESSAGE_LEN (sizeof(LOG_MESSAGE) - 1)

#define BUFFER_LEN 128

#define IP_MAX_LEN         15  // max 255.255.255.255
#define PORT_MAX_DIGITS    5   // max 65535
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
CreateSocket::CreateSocket () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}

CreateSocket::CreateSocket (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->connectionAttempts = 0; 
    this->maxConnectionAttempts = MAXATTEMPTS; 
}

CreateSocket::~CreateSocket () {
    this->mobileNetworkModule = nullptr;
}


void CreateSocket::enableTransceiver () {
    return;
}

 CellularTransceiverStatus_t  CreateSocket::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char StringToBeSend [IP_MAX_LEN + TCP_HEADER_LEN + 6 + AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_LEN + 1 ];
    char StringToBeRead [BUFFER_LEN];
    char StringToBeSendUSB [LOG_MESSAGE_LEN + 1]  = LOG_MESSAGE; 
    char ExpectedResponseFirstPart [AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_EXPECTED_RESPONSE;
    char ExpectedResponse [AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_EXPECTED_RESPONSE_LEN + 5]; 
    char ATcommand[AT_CMD_TCP_OPEN_SOCKET_FIRST_PART_LEN + 1] = AT_CMD_TCP_OPEN_SOCKET_FIRST_PART;
    char protocol[TCP_HEADER_LEN + 1] = TCP_HEADER;
    int noErrorCode = 0;
    int contextID = 1; // Usualy 1
    int connectID = 0; // could be a value beetween 0 y 11
    int access_mode = 0; // buffer access mode

    int result = snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d,%d,%s,\"%s\",%d,%d", ATcommand, contextID,
     connectID, protocol, socketTargetted->IpDirection , socketTargetted->TcpPort , access_mode);
    snprintf(ExpectedResponse, sizeof(ExpectedResponse), "+QIOPEN: %d,%d", connectID, noErrorCode);

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        this->readyToSend  = false;
        uartUSB.write (StringToBeSendUSB , strlen (StringToBeSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only 
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if ((strstr(StringToBeRead, ExpectedResponse) != nullptr) ) {  
            this->mobileNetworkModule->changeTransceiverState (new Sending (this->mobileNetworkModule));
        }
    }

    if (refreshTime->read()) {
        this->readyToSend = true;
        this->connectionAttempts++;
        if (this->connectionAttempts >= this->maxConnectionAttempts) {
             this->mobileNetworkModule->changeTransceiverState (new CloseSocket (this->mobileNetworkModule, false));
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
