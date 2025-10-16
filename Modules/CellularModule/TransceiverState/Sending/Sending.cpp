//=====[Libraries]=============================================================

#include "Sending.h"
#include "CellularModule.h" 
#include "Debugger.h" // due to global usbUart
#include "Receiving.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
#define CHUNK_SIZE 256
#define BUFFER_LEN 256


#define AT_CMD_TCP_SEND "AT+QISEND="
#define AT_CMD_TCP_SEND_LEN  (sizeof(AT_CMD_TCP_SEND) - 1)

#define AT_CMD_TCP_SEND_EXPECTED_RESPONSE "SEND OK"
#define AT_CMD_TCP_SEND_EXPECTED_RESPONSE_LEN  (sizeof(AT_CMD_TCP_SEND_EXPECTED_RESPONSE) - 1)

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================

Sending::Sending () {
    this->mobileNetworkModule = nullptr;
    this->readyToSend = true;
    this->transmissionEnable = false;
    this->watingForConfirmation = false;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS;
}

Sending::Sending (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionEnable = false;
    this->watingForConfirmation = false;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS;
}

Sending::~Sending () {
    this->mobileNetworkModule = nullptr;
}


void Sending::enableTransceiver () {
    return;
}

CellularTransceiverStatus_t Sending::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char buffer[BUFFER_LEN];

    static size_t currentMessagePosition = 0; 
    size_t messageLength = strlen(message);  
    size_t chunkSize = CHUNK_SIZE;        
    static bool debugFlag = false; 

    size_t remainingLength = messageLength - currentMessagePosition;
    size_t sizeToSend = (remainingLength < chunkSize) ? remainingLength : chunkSize;

    if (ATHandler == nullptr ||  refreshTime == nullptr || 
     message == nullptr || receivedMessage == nullptr || serverTargetted == nullptr) {
        return CELLULAR_TRANSCEIVER_STATUS_ERROR_NULL_POINTER;
    }

    if (!debugFlag) {
        snprintf(buffer, sizeof(buffer), "currentMessagePosition = %zu, messageLength = %zu", currentMessagePosition, messageLength);
        uartUSB.write(buffer, strlen(buffer));  // Debug only
        uartUSB.write("\r\n", 3);               // Debug only
        debugFlag = true;
    }

    char messageChunk[chunkSize + 1];
    strncpy(messageChunk, message + currentMessagePosition, sizeToSend);
    messageChunk[sizeToSend] = '\0'; 

    if (this->sendChunck(ATHandler, refreshTime, messageChunk, serverTargetted)) {
        currentMessagePosition += sizeToSend; 
    
        snprintf(buffer, sizeof(buffer), "Sent chunk, new currentMessagePosition = %zu", currentMessagePosition);
        uartUSB.write(buffer, strlen(buffer));  // Debug only
        uartUSB.write("\r\n", 3);               // Debug only

    }

    if (currentMessagePosition >= messageLength) {
        snprintf(buffer, sizeof(buffer), "all the chunks were sent");
        uartUSB.write(buffer, strlen(buffer));  // Debug only
        uartUSB.write("\r\n", 3);       
        currentMessagePosition = 0;  
        debugFlag = false;           
        this->mobileNetworkModule->changeTransceiverState(new Receiving(this->mobileNetworkModule));
        return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND; 
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND; 
}


bool Sending::sendChunck(ATCommandHandler *ATHandler,
    NonBlockingDelay *refreshTime, char *message, RemoteServerInformation* serverTargetted) {
    char StringToBeSend[AT_CMD_TCP_SEND_LEN + 5];
    char StringToBeRead[BUFFER_LEN];
    char ATcommand[AT_CMD_TCP_SEND_LEN + 1] = AT_CMD_TCP_SEND;
    char ExpectedResponse [AT_CMD_TCP_SEND_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_TCP_SEND_EXPECTED_RESPONSE;
    int connectID = 0;
    char confirmationToSend[] = "\x1a";
    char confirmationChar = '>';
    char recievedChar;
    static int counter = 0;

    if (ATHandler == nullptr ||  refreshTime == nullptr || 
     message == nullptr  || serverTargetted == nullptr) {
        return CELLULAR_TRANSCEIVER_STATUS_ERROR_NULL_POINTER;
    }

    int result = snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d,%d", ATcommand, connectID, strlen(message));

    if ( this->transmissionEnable == true) {
        if (ATHandler->readChar(&recievedChar) ==  true) {
            if (recievedChar  == confirmationChar) {
                uartUSB.write("Sending message", strlen("Sending message"));  // debug only
                uartUSB.write("\r\n", 3);  // debug only
                ATHandler->sendATCommand(message);
                uartUSB.write(message, strlen(message));  // debug only
                uartUSB.write("\r\n", 3);  // debug only
                ATHandler->sendATCommand(confirmationToSend);
                this->transmissionEnable = false;  // Reiniciar la bandera después de enviar //CAMBIO
                this->readyToSend = false;
                this->watingForConfirmation = true;
            }
        }
    }

    if (this->readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        uartUSB.write(StringToBeSend, strlen(StringToBeSend));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        refreshTime->restart();
        this->readyToSend = false;
        this->transmissionEnable = true;
    }

    if (this->watingForConfirmation == true) {
        if ( ATHandler->readATResponse ( StringToBeRead, BUFFER_LEN) == true) {
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                counter = 0;
                this->Attempts = 0;
                this->readyToSend = true;
                this->transmissionEnable = false;
                this->watingForConfirmation = false;
               // this->mobileNetworkModule->changeTransceiverState (new CloseSocket (this->mobileNetworkModule, true));
                 return true;
            }
        }
    }

    if (refreshTime->read()) {
        counter++;
        if (counter > 5) {
            this->readyToSend = true;
            counter = 0;
            this->transmissionEnable = false;
            this->watingForConfirmation = false;
        }
        this->Attempts++;
        if (this->Attempts >= this->maxAttempts) {
            this->mobileNetworkModule->changeTransceiverState 
            (new CloseSocket (this->mobileNetworkModule, false));
            return false;
        }
    }

    return false;
}


//=====[Implementations of private functions]==================================
