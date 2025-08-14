//=====[Libraries]=============================================================
#include "ChecksumVerifier.h"
#include "Debugger.h" // due to global usbUart

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


    int packetSize = strlen(message);
    if (packetSize < 8) { // too small CRC has 8 caracters
        uartUSB.write("Error: Packet too small for CRC\r\n", strlen("Error: Packet too small for CRC\r\n"));
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
        snprintf(crcString, sizeof(crcString), "Received CRC32: 0x%08X\r\n", receivedCRC);
        uartUSB.write(crcString, strlen(crcString));

        snprintf(crcString, sizeof(crcString), "Calculated CRC32: 0x%08X\r\n", calculatedCRC);
        uartUSB.write(crcString, strlen(crcString));

        if (calculatedCRC == receivedCRC) {
            uartUSB.write("\r\nChecksum OK!\r\n", strlen("\r\nChecksum OK!\r\n"));
        } else {
            uartUSB.write("\r\nChecksum Error\r\n", strlen("\r\nChecksum Error\r\n"));
            return MESSAGE_HANDLER_STATUS_CHECKSUM_ERROR;
        }
    } else {
        uartUSB.write("Failed to compute CRC\r\n", strlen("Failed to compute CRC\r\n"));
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    // Erase CRC from original message
    message[packetSize] = '\0';
    uartUSB.write("\r\nMessage Without CRC:\r\n", strlen("\r\nMessage Without CRC:\r\n"));
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 3);  // Debug

    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    }
}






