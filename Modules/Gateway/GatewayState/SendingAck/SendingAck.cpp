//=====[Libraries]=============================================================
#include "SendingAck.h"
#include "Gateway.h" // debido a declaración adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingForMessage.h"
#include "ConnectingToMobileNetwork.h"
#include "ConnectingEthernet.h"

//=====[Declaration of private defines]========================================
#define BACKOFF_TIME_MS                 100
#define MAX_CHUNK_SIZE_BYTES            255
#define FLY_TIME_MS                     1000
#define MAX_ACK_SEND_TRIES              3

// Mensajes de log
#define LOG_MSG_BACKOFF_PASSED          "Backoff passed\r\n"
#define LOG_MSG_SENDING_ACK_PLAINTEXT   "Sending plaintext ACKmessage:\r\n"
#define LOG_MSG_SENDING_ACK_CODED       "Coded message ACK:\r\n"
#define LOG_MSG_STATE_WAITING_MESSAGE   "Changing State to WaitingForMessage:\r\n"
#define LOG_MSG_DEBUG_SEPARATOR         "\r\n"

//=====[Implementations of public methods]===================================

SendingAck::SendingAck(Gateway * gateway, long long int deviceId, int messageNumber, gatewayStatus_t gatewayStatus) {
    this->currentGateway = gateway;
    this->IdDevice = deviceId;
    this->messageNumber = messageNumber;
    this->currentStatus = gatewayStatus;
}

SendingAck::SendingAck(Gateway * gateway, long long int deviceId, int messageNumber) {
    this->currentGateway = gateway;
    this->IdDevice = deviceId;
    this->messageNumber = messageNumber;
}

SendingAck::~SendingAck() {
    this->currentGateway = nullptr;
}

void SendingAck::sendAcknowledgement(LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime) {
    static char buffer[2500];
    static bool firstChunkSent = false;
    static bool firstDelayPassed = false;
    static bool messageFormatted = false;
    static bool firstEntryOnThisMethod = true;
    static size_t stringIndex = 0;
    static size_t numberOffAckSendings = 0;

    // Inicialización del retardo de backoff
    if (firstEntryOnThisMethod) {
        backoffTime->write(BACKOFF_TIME_MS);
        backoffTime->restart();
        firstEntryOnThisMethod = false;
    }

    // Esperar a que pase el backoff inicial
    if (!firstDelayPassed) {
        if (backoffTime->read()) {
            firstDelayPassed = true;
            uartUSB.write(LOG_MSG_BACKOFF_PASSED, strlen(LOG_MSG_BACKOFF_PASSED));
        } else {
            return;
        }
    }

    // Formatear el mensaje ACK
    if (!messageFormatted) {
        uartUSB.write(LOG_MSG_SENDING_ACK_PLAINTEXT, strlen(LOG_MSG_SENDING_ACK_PLAINTEXT));

        snprintf(buffer, sizeof(buffer), "%lld,%d,ACK", this->IdDevice, this->messageNumber);

        uartUSB.write(buffer, strlen(buffer));
        uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));

        if (!this->currentGateway->prepareLoRaMessage(buffer, strlen(buffer))) {
            return;
        }

        uartUSB.write(LOG_MSG_SENDING_ACK_CODED, strlen(LOG_MSG_SENDING_ACK_CODED));
        uartUSB.write(buffer, strlen(buffer));
        uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));

        // Agregar delimitadores "||"
        size_t originalLength = strlen(buffer);
        buffer[originalLength]     = '|';
        buffer[originalLength + 1] = '|';
        buffer[originalLength + 2] = '\0';

        uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));
        uartUSB.write(buffer, strlen(buffer));
        uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));

        messageFormatted = true;
    }

    // Enviar por LoRa en fragmentos
    if (backoffTime->read() || !firstChunkSent) {
        firstChunkSent = true;
        backoffTime->write(FLY_TIME_MS);
        backoffTime->restart();

        size_t totalLength = strlen(buffer);
        size_t chunkSize = MAX_CHUNK_SIZE_BYTES;
        size_t currentChunkSize = (totalLength - stringIndex < chunkSize)
                                    ? (totalLength - stringIndex)
                                    : chunkSize;

        LoRaModule->idle();
        LoRaModule->enableInvertIQ();

        uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));
        uartUSB.write(buffer, strlen(buffer));
        uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));

        LoRaModule->beginPacket();
        LoRaModule->write((uint8_t*)(buffer + stringIndex), currentChunkSize);
        LoRaModule->endPacket();

        stringIndex += chunkSize;

        if (stringIndex > totalLength) {
            numberOffAckSendings++;
            stringIndex = 0;
            backoffTime->restart();

            if (numberOffAckSendings < MAX_ACK_SEND_TRIES) {
                return;
            }

            // Restaurar estado
            numberOffAckSendings = 0;
            uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));
            uartUSB.write(LOG_MSG_STATE_WAITING_MESSAGE, strlen(LOG_MSG_STATE_WAITING_MESSAGE));
            uartUSB.write(LOG_MSG_DEBUG_SEPARATOR, strlen(LOG_MSG_DEBUG_SEPARATOR));

            firstDelayPassed = false;
            messageFormatted = false;
            firstEntryOnThisMethod = true;
            stringIndex = 0;

            this->currentGateway->changeState(
                new ConnectingEthernet(this->currentGateway, this->currentStatus)
            );
            return;
        }
    }
}

bool SendingAck::waitForMessage(LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut) {
    return false;
}
