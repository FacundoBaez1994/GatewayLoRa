//=====[Libraries]=============================================================
#include "ChecksumGenerator.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ChecksumGenerator::ChecksumGenerator () {
    this->nextHandler = nullptr;
}

ChecksumGenerator::~ChecksumGenerator () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t ChecksumGenerator::handleMessage(char *message, unsigned int sizeOfMessage) {

    MbedCRC<POLY_32BIT_ANSI, 32> crc32;
    uint32_t crc; 

    uartUSB.write("message on  ChecksumGenerator:\r\n", strlen("message on  ChecksumGenerator:\r\n"));
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 3);

    size_t messageLength = strlen(message);  // Longitud del mensaje original
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), messageLength, &crc) == 0) {
        char crcStr[9]; 
        sprintf(crcStr, "%08X", crc);

        // Debug
        uartUSB.write("Generated CRC32:\r\n", strlen("Generated CRC32:\r\n"));
        uartUSB.write(crcStr, strlen(crcStr));
        uartUSB.write("\r\n", 3);

        size_t totalLength = messageLength + strlen(crcStr); // 
        if (totalLength >= 4000) { 
            uartUSB.write("Message too long to append CRC\r\n", strlen("Message too long to append CRC\r\n"));
            return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
        }

        size_t crcLen = strlen(crcStr);

        // Copy CRC at the end of the message
        for (size_t i = 0; i < crcLen; ++i) {
            message[messageLength + i] = crcStr[i];
        }

        message[totalLength] = '\0'; 

        uartUSB.write("Complete message with CRC:\r\n", strlen("Complete message with CRC:\r\n"));
        uartUSB.write(message, strlen(message));
        uartUSB.write("\r\n", 3);

    } else {
        uartUSB.write("Failed to compute CRC\r\n", strlen("Failed to compute CRC\r\n"));
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    }
}



