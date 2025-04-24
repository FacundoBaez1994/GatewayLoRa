//=====[Libraries]=============================================================

#include "SendingAck.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "SendingTCPMessage.h"

//=====[Declaration of private defines]========================================
#define BACKOFFTIME        300
#define MAX_CHUNK_SIZE     10
#define FLY_TIME           500
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
    static char ACKmessage[226] = {0};
    static bool firstChunkSent = false;
    static bool firstDelayPassed = false;
    static bool messageFormatted = false;
    static bool firstEntryOnThisMethod = true;
    static size_t stringIndex = 0;

    if (firstEntryOnThisMethod == true) {
        delay->write(BACKOFFTIME);
        delay->restart();
        firstEntryOnThisMethod = false;
    }


    if (firstDelayPassed == false) {
        if (delay->read()) {
            firstDelayPassed = true;
            uartUSB.write("Backoff passed\r\n", strlen("Backoff passed\r\n"));
        } else {
            return;
        }
    }

    if (messageFormatted == false) {
        snprintf(ACKmessage, sizeof(ACKmessage), "%d,%d,ACK", this->IdDevice, this->messageNumber); //
        if (this->gateway->prepareMessage(ACKmessage) == false) {
            uartUSB.write("Fail to prepare message to be send\r\n", strlen("Fail to prepare message to be send\r\n"));
            std::fill(std::begin(ACKmessage), std::end(ACKmessage), '\0');
            return;
        }
        size_t originalLength = strlen(ACKmessage);
        // Copiar la cadena original y agregar '|'
        ACKmessage[originalLength ] = '|';  // Agregar '|'
        ACKmessage[originalLength + 1] = '|';      // Asegurar terminación nula
        ACKmessage[originalLength + 2] = '\0';      // Asegurar terminación nula
        messageFormatted = true;
        uartUSB.write("Sending Acknowledgment Message\r\n", strlen("Sending Acknowledgment Message\r\n")); // Debug 
    }

    if (delay->read() || firstChunkSent == false) {
        firstChunkSent = true;
        delay->write(FLY_TIME);
        delay->restart();

        size_t totalLength = strlen(ACKmessage);
        size_t chunkSize = MAX_CHUNK_SIZE;  // Fragmentos de 50 bytes
        LoRaModule->idle();                          // set standby mode
        LoRaModule->enableInvertIQ();             // normal mode
        size_t currentChunkSize = (totalLength - stringIndex  < chunkSize) ? (totalLength - stringIndex ) : chunkSize;
        uartUSB.write("\r\n", strlen("\r\n"));
        uartUSB.write (ACKmessage + stringIndex , currentChunkSize);  // debug only
        uartUSB.write("\r\n", strlen("\r\n"));
        LoRaModule->beginPacket();
        LoRaModule->write((uint8_t*)(ACKmessage +  stringIndex ), currentChunkSize);
        LoRaModule->endPacket();
        stringIndex += chunkSize;
        if (stringIndex  > totalLength) {
            uartUSB.write("Changing To WaitingForMessage State\r\n", strlen("Changing To WaitingForMessage State\r\n"));
            this->gateway->changeState (new WaitingForMessage (this->gateway));
            std::fill(std::begin(ACKmessage), std::end(ACKmessage), '\0');
            firstDelayPassed = false;
            messageFormatted = false;
            stringIndex = 0;
            return;
        }
    }
    return;
}

void SendingAck::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}



//=====[Implementations of private functions]==================================
