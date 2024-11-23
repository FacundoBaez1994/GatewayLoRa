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
    char ACKmessage[20] = {0};

    snprintf(ACKmessage, sizeof(ACKmessage), "%d,%d,ACK", this->IdDevice, this->messageNumber);
    LoRaModule->idle();                          // Standby mode
    LoRaModule->enableInvertIQ();                // Enable I/Q inversion for transmission
    uartUSB.write("Sending Acknowledgment Message\r\n", strlen("Sending Acknowledgment Message\r\n")); // Debug
    LoRaModule->beginPacket();
    LoRaModule->write((uint8_t *)ACKmessage, strlen(ACKmessage));
    LoRaModule->endPacket();

    uartUSB.write("Changing To Sending TPC Message State\r\n", strlen("Changing To Sending TPC Message State\r\n\r\n"));
    this->gateway->changeState (new SendingTCPMessage(this->gateway, this->IdDevice, this->messageNumber, this->payload));

    return;
}

void SendingAck::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}



//=====[Implementations of private functions]==================================
