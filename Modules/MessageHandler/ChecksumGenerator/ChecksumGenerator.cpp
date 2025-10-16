//=====[Libraries]=============================================================
#include "ChecksumGenerator.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE_ON_GENERATOR              "message on ChecksumGenerator:\r\n"
#define LOG_MESSAGE_ON_GENERATOR_LEN          (sizeof(LOG_MESSAGE_ON_GENERATOR) - 1)

#define LOG_GENERATED_CRC32                   "Generated CRC32:\r\n"
#define LOG_GENERATED_CRC32_LEN               (sizeof(LOG_GENERATED_CRC32) - 1)

#define LOG_MESSAGE_TOO_LONG                  "Message too long to append CRC\r\n"
#define LOG_MESSAGE_TOO_LONG_LEN              (sizeof(LOG_MESSAGE_TOO_LONG) - 1)

#define LOG_COMPLETE_MESSAGE_WITH_CRC         "Complete message with CRC:\r\n"
#define LOG_COMPLETE_MESSAGE_WITH_CRC_LEN     (sizeof(LOG_COMPLETE_MESSAGE_WITH_CRC) - 1)

#define LOG_FAILED_COMPUTE_CRC                "Failed to compute CRC\r\n"
#define LOG_FAILED_COMPUTE_CRC_LEN            (sizeof(LOG_FAILED_COMPUTE_CRC) - 1)
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

    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }

    uartUSB.write(LOG_MESSAGE_ON_GENERATOR, LOG_MESSAGE_ON_GENERATOR_LEN);
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 3);

    size_t messageLength = strlen(message); 
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), messageLength, &crc) == 0) {
        char crcStr[9]; 
        sprintf(crcStr, "%08X", crc);

        // Debug
        uartUSB.write(LOG_GENERATED_CRC32, LOG_GENERATED_CRC32_LEN);
        uartUSB.write(crcStr, strlen(crcStr));
        uartUSB.write("\r\n", 3);

        size_t totalLength = messageLength + strlen(crcStr); // 
        if (totalLength >= 4000) { 
            uartUSB.write(LOG_MESSAGE_TOO_LONG, LOG_MESSAGE_TOO_LONG_LEN);
            return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
        }

        size_t crcLen = strlen(crcStr);

        // Copy CRC at the end of the message
        for (size_t i = 0; i < crcLen; ++i) {
            message[messageLength + i] = crcStr[i];
        }

        message[totalLength] = '\0'; 

        uartUSB.write(LOG_COMPLETE_MESSAGE_WITH_CRC, LOG_COMPLETE_MESSAGE_WITH_CRC_LEN);
        uartUSB.write(message, strlen(message));
        uartUSB.write("\r\n", 3);

    } else {
        uartUSB.write(LOG_FAILED_COMPUTE_CRC, LOG_FAILED_COMPUTE_CRC_LEN);
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    }
}



