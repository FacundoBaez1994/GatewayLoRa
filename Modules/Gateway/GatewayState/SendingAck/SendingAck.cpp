//=====[Libraries]=============================================================

#include "SendingAck.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "SendingTCPMessage.h"

//=====[Declaration of private defines]========================================

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
SendingAck::SendingAck (Gateway * gateway, int IdDevice, int messageNumber, char * payload) {
    this->gateway = gateway;
    this->IdDevice = IdDevice;
    this->messageNumber = messageNumber;
    
    if (payload != nullptr) {
        strncpy(this->payload, payload, sizeof(this->payload) - 1); // Copiar hasta 49 caracteres
        this->payload[sizeof(this->payload) - 1] = '\0';            // Asegurar terminación nula
    } else {
        this->payload[0] = '\0'; // Si payload es nullptr, dejar vacío
    }
}


/** 
* @brief
* 
* @param 
*/
SendingAck::~SendingAck () {
     this->gateway = NULL;
}



void SendingAck::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void SendingAck::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    char ACKmessage[226] = {0};

    if (delay->read()) {

            
        snprintf(ACKmessage, sizeof(ACKmessage), "%d,%d,ACK", this->IdDevice, this->messageNumber); //

        if (this->gateway->prepareMessage(ACKmessage) == false) {
            uartUSB.write("Fail to prepare message to be send\r\n", strlen("Fail to prepare message to be send\r\n"));
            std::fill(std::begin(ACKmessage), std::end(ACKmessage), '\0');
            return;
        }

        size_t originalLength = strlen(ACKmessage);
        //size_t newLength = originalLength + 1; // +1 para el carácter '|'

        // Copiar la cadena original y agregar '|'
        ACKmessage[originalLength ] = '|';  // Agregar '|'
        ACKmessage[originalLength + 1] = '|';      // Asegurar terminación nula
        ACKmessage[originalLength + 2] = '\0';      // Asegurar terminación nula


        uartUSB.write("Sending Acknowledgment Message\r\n", strlen("Sending Acknowledgment Message\r\n")); // Debug    
        size_t totalLength = strlen(ACKmessage);
        size_t chunkSize = 5;  // Fragmentos de 50 bytes

        for (size_t i = 0; i < totalLength; i += chunkSize) {
            LoRaModule->idle();                          // set standby mode
            LoRaModule->enableInvertIQ();             // normal mode

            size_t currentChunkSize = (totalLength - i < chunkSize) ? (totalLength - i) : chunkSize;
            uartUSB.write("\r\n", strlen("\r\n"));
            uartUSB.write (ACKmessage + i, currentChunkSize);  // debug only
            uartUSB.write("\r\n", strlen("\r\n"));
            LoRaModule->beginPacket();
            LoRaModule->write((uint8_t*)(ACKmessage + i), currentChunkSize);
            LoRaModule->endPacket();
          
            wait_us(2500000); // bloqueo eliminar luego!
        }


        uartUSB.write("Changing To WaitingForMessage State\r\n", strlen("Changing To WaitingForMessage State\r\n\r\n"));
        this->gateway->changeState (new WaitingForMessage (this->gateway));
        std::fill(std::begin(ACKmessage), std::end(ACKmessage), '\0');
        return;
    }

    return;
}

void SendingAck::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}



//=====[Implementations of private functions]==================================
