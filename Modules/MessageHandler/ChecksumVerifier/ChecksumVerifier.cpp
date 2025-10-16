//=====[Libraries]=============================================================
#include "ChecksumVerifier.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]=====================================
#define LOG_PACKET_TOO_SMALL                  "Error: Packet too small for CRC\r\n"
#define LOG_PACKET_TOO_SMALL_LEN              (sizeof(LOG_PACKET_TOO_SMALL)-1)

#define LOG_RECEIVED_CRC32                    "Received CRC32: 0x%08X\r\n"
#define LOG_RECEIVED_CRC32_LEN                 (sizeof(LOG_RECEIVED_CRC32)-1)

#define LOG_CALCULATED_CRC32                  "Calculated CRC32: 0x%08X\r\n"
#define LOG_CALCULATED_CRC32_LEN               (sizeof(LOG_CALCULATED_CRC32)-1)

#define LOG_CHECKSUM_OK                       "\r\nChecksum OK!\r\n"
#define LOG_CHECKSUM_OK_LEN                   (sizeof(LOG_CHECKSUM_OK)-1)

#define LOG_CHECKSUM_ERROR                    "\r\nChecksum Error\r\n"
#define LOG_CHECKSUM_ERROR_LEN                (sizeof(LOG_CHECKSUM_ERROR)-1)

#define LOG_FAILED_COMPUTE_CRC                "Failed to compute CRC\r\n"
#define LOG_FAILED_COMPUTE_CRC_LEN            (sizeof(LOG_FAILED_COMPUTE_CRC)-1)

#define LOG_MESSAGE_WITHOUT_CRC               "\r\nMessage Without CRC:\r\n"
#define LOG_MESSAGE_WITHOUT_CRC_LEN           (sizeof(LOG_MESSAGE_WITHOUT_CRC)-1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ChecksumVerifier::ChecksumVerifier () {
    this->nextHandler = nullptr;
}

ChecksumVerifier::~ChecksumVerifier () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t ChecksumVerifier::handleMessage(char *message, unsigned int sizeOfMessage) {
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;

    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }

    int packetSize = strlen(message);
    if (packetSize < 8) { // too small CRC has less than 8 caracters
        uartUSB.write(LOG_PACKET_TOO_SMALL, LOG_PACKET_TOO_SMALL_LEN);
        return MESSAGE_HANDLER_STATUS_ERROR_PACKET_TOO_SMALL;
    }

    // separate CRC 
    packetSize -= 8; 
    uint32_t receivedCRC;
    sscanf(&message[packetSize], "%8X", &receivedCRC); // Extract  CRC 

    // Verify
    uint32_t calculatedCRC;
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), packetSize, &calculatedCRC) == 0) {
        // Enviar información sobre el CRC calculado y recibido
        char crcString[64];
        snprintf(crcString, sizeof(crcString), LOG_RECEIVED_CRC32, receivedCRC);
        uartUSB.write(crcString, strlen(crcString));

        snprintf(crcString, sizeof(crcString), LOG_CALCULATED_CRC32, calculatedCRC);
        uartUSB.write(crcString, strlen(crcString));

        if (calculatedCRC == receivedCRC) {
            uartUSB.write(LOG_CHECKSUM_OK, LOG_CHECKSUM_OK_LEN);
        } else {
            uartUSB.write(LOG_CHECKSUM_ERROR, LOG_CHECKSUM_ERROR_LEN);
            return MESSAGE_HANDLER_STATUS_CHECKSUM_ERROR;
        }
    } else {
        uartUSB.write(LOG_FAILED_COMPUTE_CRC, LOG_FAILED_COMPUTE_CRC_LEN);
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    // Erase CRC from original message
    message[packetSize] = '\0';
    uartUSB.write(LOG_MESSAGE_WITHOUT_CRC, LOG_MESSAGE_WITHOUT_CRC_LEN);
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 3);  // Debug

    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    }
}












