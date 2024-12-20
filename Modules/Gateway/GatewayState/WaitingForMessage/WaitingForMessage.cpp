//=====[Libraries]=============================================================

#include "WaitingForMessage.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

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
WaitingForMessage::WaitingForMessage (Gateway * gateway) {
    this->gateway = gateway;
}


/** 
* @brief 
* 
* 
* @returns 
*/
WaitingForMessage::~WaitingForMessage () {
    this->gateway = NULL;
}


void WaitingForMessage::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    static bool messageReceived = false; 
    char buffer[256];
    char message[100];
    char payload[100] = {0}; // Espacio suficiente para almacenar el payload

    int deviceId = 0;
    int messageNumber = 0;
    static int delayCounter = 0;
    static int delayMax = 3; 

    uint8_t receivedBuffer[64];

    if (messageReceived  == false) {
        LoRaModule->disableInvertIQ (); // rx mode -> phase Quadrature invertion
        int packetSize = LoRaModule->parsePacket();
        if (packetSize) {
            uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

            int maxIterations = 100; // Límite para evitar un ciclo infinito
            int iterations = 0;

            // Leer los datos disponibles
            
            while (LoRaModule->available() > 0 && iterations < maxIterations) {
                ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer));
               // ssize_t bytesRead = LoRaModule->read(reinterpret_cast<uint8_t*>(buffer), strlen (buffer));
                if (bytesRead > 0) {
                    // Enviar los bytes leídos al puerto serie
                   // uartUSB.write(buffer, bytesRead);
                    uartUSB.write(buffer, strlen(buffer));
                    uartUSB.write ("\r\n", strlen("\r\n"));
                }
                iterations++;
            }

            if (iterations >= maxIterations) {
                uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
                return;
            }

            if (this->gateway->processMessage(buffer) == false) {
                return;
            }

            if (sscanf(buffer, "%d,%d,%49s", &deviceId, &messageNumber, payload) == 3) {
                // Desglose exitoso
                snprintf(message, sizeof(message), "Device ID: %d\r\n", deviceId);
                uartUSB.write(message, strlen(message));
                this->IdDeviceReceived = deviceId;

                snprintf(message, sizeof(message), "Message Number: %d\r\n", messageNumber);
                uartUSB.write(message, strlen(message));
                this->messageNumberReceived = messageNumber;

                snprintf(message, sizeof(message), "Payload: %s\r\n", payload);
                uartUSB.write(message, strlen(message));
                strcpy (this->payload, payload);
            } else {
                uartUSB.write("Error parsing message.\r\n", strlen("Error parsing message.\r\n"));
                return;
            }
            // Leer el RSSI del paquete recibido
            int packetRSSI = LoRaModule->packetRssi();
            snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
            uartUSB.write(message, strlen(message));
            messageReceived  = true;
        
        }
    }


    if (delay->read () && messageReceived == true) {
        //delayCounter ++;
        //if (delayCounter >= delayMax) {
            delayCounter = 0;
            delayMax = 0;
            messageReceived  = false;
           // uartUSB.write("Changing To Sending ACK State\r\n", strlen("Changing To Sending ACK State\r\n"));
           // this->gateway->changeState (new SendingAck (this->gateway, this->IdDeviceReceived, 
           // this->messageNumberReceived , this->payload));
            return;
        //}
    }


    return;
}

void WaitingForMessage::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void WaitingForMessage::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}

//=====[Implementations of private functions]==================================





/** 
* @brief 
* 
* 
* @returns 
*/
/*
 CellularTransceiverStatus_t  Receiving::exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {

    char ATcommandFirstPart [15] = "AT+QIRD= ";
    char StringToBeSend [16];
    char StringToBeSendUSB [20]  = "RECV Data";
    char noDataResponse [15] = "+QIRD: 0";
    char expectedResponse [15] = "OK";

    char retrivedMessage [200];
    char StringToBeRead [200];

    static bool readyToSend = false;
    static int attempts = 0; 
    static int maxConnectionAttempts = MAXATTEMPTS; 
    static bool thereIsdataToRetriv = false;
    static bool dataRetrieved;

   
    //char protocol[] = "\"TCP\"";
    int noErrorCode = 0;
    int contextID = 1; // Usualmente 1
    int connectID = 0; // Puede ser entre 0 y 11
    int access_mode = 0; // Modo de acceso al buffer


    snprintf(StringToBeSend, sizeof(StringToBeSend ), "%s%d", ATcommandFirstPart, connectID);
    if (readyToSend == true) {
        ATHandler->sendATCommand(StringToBeSend);
        readyToSend  = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeSendUSB , strlen (StringToBeSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToBeSend  , strlen (StringToBeSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( ATHandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead  , strlen (StringToBeRead  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (thereIsdataToRetriv == true) {
            thereIsdataToRetriv = false;
            dataRetrieved = true;
            strcpy (receivedMessage, StringToBeRead);
            char StringToSendUSB [40] = "Message retrived";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
        }

        /// seach for OK confirmation
        if (dataRetrieved == true) {
            if (strcmp (StringToBeRead, expectedResponse) == 0) {

                *newDataAvailable = true; 

                attempts = 0;
                char StringToSendUSB [40] = "Cambiando de estado 80?";
                uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only
                this->mobileNetworkModule->changeTransceiverState
                 (new CloseSocket (this->mobileNetworkModule, true));
                return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
            }
        }

        if ( strcmp (StringToBeRead, noDataResponse) == 0) {
            // No data to recv keep trying 
            attempts++;
            readyToSend  = false;
            return CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND;
        }
        
        if (checkResponse(StringToBeRead, retrivedMessage) == true ) {
            thereIsdataToRetriv =  true;
            ////   ////   ////   ////   ////   ////    
        }
    }



    if (refreshTime->read()) {
        readyToSend = true;
        attempts++;
        if (attempts >= maxConnectionAttempts) {
            attempts = 0;
             this->mobileNetworkModule->changeTransceiverState (new CloseSocket (this->mobileNetworkModule, true));
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
*/