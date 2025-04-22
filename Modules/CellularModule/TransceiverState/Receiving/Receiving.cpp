//=====[Libraries]=============================================================

#include "Receiving.h"
#include "CellularModule.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
#define TIMETORESEND 10
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
Receiving::Receiving () {
    this->mobileNetworkModule = NULL;
}


/** 
* @brief
* 
* @param 
*/
Receiving::Receiving (CellularModule * mobileModule) {
    this->mobileNetworkModule = mobileModule;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Receiving::~Receiving () {
    this->mobileNetworkModule = NULL;
}


void Receiving::enableTransceiver () {
    return;
}


/** 
* @brief 
* 
* 
* @returns 
*/
CellularTransceiverStatus_t Receiving::exchangeMessages(
    ATCommandHandler* ATHandler,
    NonBlockingDelay* refreshTime,
    char* message,
    TcpSocket* socketTargetted,
    char* receivedMessage,
    bool* newDataAvailable) {

    char ATcommandFirstPart[15] = "AT+QIRD=";
    char StringToBeSend[20];
    char StringToBeSendUSB[20] = "RECV Data";
    char noDataResponse[15] = "+QIRD: 0";
    char expectedResponse[5] = "OK";  // Puede ser simplemente "OK"

    char retrivedMessage[200];
    char StringToBeRead[200];

    static bool readyToSend = true;
    static int attempts = 0;
    static const int maxConnectionAttempts = MAXATTEMPTS;
    static bool thereIsDataToRetrieve = false;
    static bool dataRetrieved = false;

    int connectID = 0;

    snprintf(StringToBeSend, sizeof(StringToBeSend), "%s%d", ATcommandFirstPart, connectID);

    if (readyToSend) {
        ATHandler->sendATCommand(StringToBeSend);
        readyToSend = false;

        // Debug USB
        uartUSB.write(StringToBeSendUSB, strlen(StringToBeSendUSB));
        uartUSB.write("\r\n", 3);
        uartUSB.write(StringToBeSend, strlen(StringToBeSend));
        uartUSB.write("\r\n", 3);

        //refreshTime->write(TIMETORESEND);
        //refreshTime->restart();
    }

    if (ATHandler->readATResponse(StringToBeRead)) {
        uartUSB.write(StringToBeRead, strlen(StringToBeRead));
        uartUSB.write("\r\n", 3);

        if (thereIsDataToRetrieve) {
            thereIsDataToRetrieve = false;
            dataRetrieved = true;

            // Copiamos el mensaje útil
            strcpy(receivedMessage, StringToBeRead);

            uartUSB.write("Message retrieved\r\n", strlen("Message retrieved\r\n"));
        }

        // Esperamos el OK que confirma el cierre del ciclo
        if (dataRetrieved && strcmp(StringToBeRead, expectedResponse) == 0) {
            *newDataAvailable = true;

            uartUSB.write("Closing Socket\r\n", strlen("Closing Socket\r\n"));

            // Reset flags y contador
            readyToSend = true;
            dataRetrieved = false;
            thereIsDataToRetrieve = false;
            attempts = 0;

            // Cambio de estado
            this->mobileNetworkModule->changeTransceiverState(
                new CloseSocket(this->mobileNetworkModule, true)
            );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }

        // Si no hay datos
        if (strcmp(StringToBeRead, noDataResponse) == 0) {
            attempts++;
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }

        // Si hay datos para recuperar
        if (checkResponse(StringToBeRead, retrivedMessage)) {
            thereIsDataToRetrieve = true;
        }
    }

    // Manejamos el retardo no bloqueante para reintentar
    if (refreshTime->read()) {
        uartUSB.write("REFRESH\r\n", strlen("REFRESH\r\n"));
        readyToSend = true;
        attempts++;

        if (attempts >= maxConnectionAttempts) {
            uartUSB.write("Closing Socket\r\n", strlen("Closing Socket\r\n"));

            // Reset total
            readyToSend = true;
            dataRetrieved = false;
            thereIsDataToRetrieve = false;
            attempts = 0;

            // Cambio de estado
            this->mobileNetworkModule->changeTransceiverState(
                new CloseSocket(this->mobileNetworkModule, true)
            );
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
    }

    return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
}



//=====[Implementations of private functions]==================================
bool Receiving::checkResponse(char * response, char * retrivMessage) {
    char expectedFistPartResponse[15] = "+QIRD: ";
    
    // Verificar que la respuesta sea del tipo esperado
    if (strncmp(response, expectedFistPartResponse, strlen(expectedFistPartResponse)) == 0) {
        int messageLength = 0;
        
        // Extraer el número entero después de "+QIRD: "
        if (sscanf(response + strlen(expectedFistPartResponse), "%d", &messageLength) == 1) {
            // Verificar si el número es mayor a cero
            if (messageLength > 0) {
                return true;
            }
        }
    }

    return false; // Si no coincide o hay algún error en el parseo
}
