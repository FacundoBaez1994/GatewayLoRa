//=====[Libraries]=============================================================

#include "Sending.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "Receiving.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
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
Sending::Sending () {
    this->mobileNetworkModule = NULL;
    this->readyToSend = true;
    this->transmissionEnable = false;
    this->watingForConfirmation = false;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS;
}


/** 
* @brief
* 
* @param 
*/
Sending::Sending (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
    this->readyToSend = true;
    this->transmissionEnable = false;
    this->watingForConfirmation = false;
    this->Attempts = 0; 
    this->maxAttempts = MAXATTEMPTS;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Sending::~Sending () {
    this->mobileNetworkModule = NULL;
}



/** 
* @brief 
* 
* 
* @returns 
*/
void Sending::enableTransceiver () {
    return;
}



CellularTransceiverStatus_t Sending::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    char buffer[100];

    static size_t currentMessagePosition = 0;  // Posición actual del mensaje
    size_t messageLength = strlen(message);    // Longitud total del mensaje
    size_t chunkSize = 256;                    // Tamaño del fragmento a enviar
    static bool debugFlag = false;             // Flag para evitar múltiples logs de depuración

    // Asegurarse de que no se pase del final del mensaje
    size_t remainingLength = messageLength - currentMessagePosition;
    size_t sizeToSend = (remainingLength < chunkSize) ? remainingLength : chunkSize;

    // Mostrar depuración solo una vez
    if (!debugFlag) {
        snprintf(buffer, sizeof(buffer), "currentMessagePosition = %zu, messageLength = %zu", currentMessagePosition, messageLength);
        uartUSB.write(buffer, strlen(buffer));  // Debug only
        uartUSB.write("\r\n", 3);               // Debug only
        debugFlag = true;
    }

    // Crear un fragmento del mensaje para enviar
    char messageChunk[chunkSize + 1];
    strncpy(messageChunk, message + currentMessagePosition, sizeToSend);
    messageChunk[sizeToSend] = '\0';  // Asegurarse de que termine en NULL

    // Enviar el fragmento actual
    if (this->sendChunck(ATHandler, refreshTime, messageChunk, socketTargetted)) {
        currentMessagePosition += sizeToSend;  // Avanzar en la posición del mensaje

        // Depuración después de enviar cada fragmento
        
        snprintf(buffer, sizeof(buffer), "Sent chunk, new currentMessagePosition = %zu", currentMessagePosition);
        uartUSB.write(buffer, strlen(buffer));  // Debug only
        uartUSB.write("\r\n", 3);               // Debug only

    }

        // Si ya hemos enviado todo el mensaje
    if (currentMessagePosition >= messageLength) {
        snprintf(buffer, sizeof(buffer), "all the chunks were sent");
        uartUSB.write(buffer, strlen(buffer));  // Debug only
        uartUSB.write("\r\n", 3);       
        currentMessagePosition = 0;  // Reiniciar para el próximo mensaje
        debugFlag = false;           // Reiniciar flag de depuración para el próximo mensaje
        this->mobileNetworkModule->changeTransceiverState(new Receiving(this->mobileNetworkModule));
        return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;  // Mensaje completamente enviado
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;  // Aún quedan fragmentos por enviar
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool Sending::sendChunck(ATCommandHandler *ATHandler,
    NonBlockingDelay *refreshTime, char *message, TcpSocket * socketTargetted) {
    char StringToBeSend[100];
    char StringToBeRead[100];
    char StringToBeSendUSB[] = "SENDING DATA"; 
    char ATcommand[] = "AT+QISEND=";
    char ExpectedResponse [] = "SEND OK";
    int connectID = 0; // Puede ser entre 0 y 11
    char confirmationToSend[] = "\x1a";
    char confirmationChar = '>';
    char recievedChar;
    static int counter = 0;

    // Formatear la cadena final
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
        uartUSB.write(StringToBeSendUSB, strlen(StringToBeSendUSB));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        uartUSB.write(StringToBeSend, strlen(StringToBeSend));  // debug only
        uartUSB.write("\r\n", 3);  // debug only
        refreshTime->restart();
        this->readyToSend = false;
        this->transmissionEnable = true;
    }


    if (this->watingForConfirmation == true) {
        if ( ATHandler->readATResponse ( StringToBeRead) == true) {
            ////   ////   ////   ////   ////   ////
            uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////
            if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
                ////   ////   ////   ////   ////   ////
                char StringToSendUSB [40] = "Cambiando de estado 3";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                counter = 0;
                this->Attempts =0;
                this->readyToSend = true;
                this->transmissionEnable = false;
                this->watingForConfirmation = false;
                ////   ////   ////   ////   ////   ////     
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
