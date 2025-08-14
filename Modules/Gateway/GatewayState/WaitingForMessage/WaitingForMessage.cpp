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
    this->gateway = nullptr;
}


void WaitingForMessage::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    static bool messageReceived = false; 
    static std::vector<char> accumulatedBuffer; // Acumulador de fragmentos
    static std::string fullMessage;

    char processedMessageReceived  [1024];
    char buffer[1024];
    char message[100];
    char payload[1024]; // Espacio suficiente para almacenar el payload

    int deviceId = 0;
    int messageNumber = 0;
    static int delayCounter = 0;
    static int delayMax = 3; 

    static int stringInsertCount = 0;

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
                    accumulatedBuffer.insert(accumulatedBuffer.end(), buffer, buffer + bytesRead);
                    stringInsertCount ++;

                    if (stringInsertCount > 51) {
                        // Eliminar todos los elementos del vector
                        accumulatedBuffer.clear();
                        stringInsertCount = 0;

                        // Acción adicional, si es necesario
                        uartUSB.write("Buffer cleared size reach limit\r\n", strlen("Buffer cleared size reach limit\r\n"));
                        messageReceived = false;
                        return;
                    }

                    // Buscar el delimitador `|`
                    auto delimiterPos = std::search(accumulatedBuffer.begin(), accumulatedBuffer.end(), "||", "||" + 2);
                    if (delimiterPos != accumulatedBuffer.end()) {
                        // Reconstruir el mensaje completo
                        fullMessage.assign(accumulatedBuffer.begin(), delimiterPos);
                        accumulatedBuffer.erase(accumulatedBuffer.begin(), delimiterPos + 2); // Eliminar procesado
                        stringInsertCount = 0;

                        // Debug del mensaje completo
                        uartUSB.write("Full Message: ", strlen("Full Message: "));
                        uartUSB.write(fullMessage.c_str(), fullMessage.length());
                        uartUSB.write("\r\n", strlen("\r\n"));
                        messageReceived = true;

                        int packetRSSI = LoRaModule->packetRssi();
                        snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
                        uartUSB.write(message, strlen(message));
                    }
                    iterations++;
                    if (iterations >= maxIterations) {
                        uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
                        messageReceived = false;
                        return;
                    }
                } //  if (bytesRead > 0) end
            } // while (LoRaModule->available() > 0 && iterations < maxIterations)  end
        } //  if (packetSize)  end
    }  else {   // messageReceived  == false if end
        if (fullMessage.empty()) {
            accumulatedBuffer.clear();
            messageReceived = false;
            uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n"));
            return;
        }
        const char* constCharPtr = fullMessage.c_str(); 

        //char* processedMessageReceived = new char[fullMessage.size() + 1]; // +1 para '\0'
        strcpy(processedMessageReceived, constCharPtr);

        if (this->gateway->processMessage(processedMessageReceived, sizeof(processedMessageReceived)) == false) {
            uartUSB.write("Fail to process received message\r\n", strlen("Fail to process received message\r\n")); // Debug
            messageReceived = false;
            return;
        }
        // message interpretation
        if (sscanf(processedMessageReceived, "%d,%d,%s", &deviceId, &messageNumber, payload) == 3) {
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
            messageReceived = false;
            return;
        }
        messageReceived  = false;
        accumulatedBuffer.clear(); // Elimina todos los elementos del vector
        stringInsertCount = 0;
        fullMessage.clear();       // Elimina todo el contenido de la cadena
        uartUSB.write("Changing To Sending ACK State\r\n", strlen("Changing To Sending ACK State\r\n"));
        this->gateway->changeState (new SendingAck (this->gateway, this->IdDeviceReceived, 
        this->messageNumberReceived));
        return;
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

