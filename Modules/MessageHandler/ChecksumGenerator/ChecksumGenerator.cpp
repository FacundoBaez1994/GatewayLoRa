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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
ChecksumGenerator::ChecksumGenerator () {
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ChecksumGenerator::~ChecksumGenerator () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t ChecksumGenerator::handleMessage(char *message) {
    // Crear una instancia de MbedCRC con el polinomio 32-bit ANSI
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;
    uint32_t crc; // Variable para almacenar el resultado del CRC de 32 bits

    // Calcular el CRC32 del mensaje original (sin el CRC agregado)
    size_t messageLength = strlen(message);  // Longitud del mensaje original
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), messageLength, &crc) == 0) {
        char crcStr[9]; // Espacio para 8 dígitos hexadecimales + terminador nulo
        sprintf(crcStr, "%08X", crc); // Convertir a formato hexadecimal con 8 caracteres

        // Depuración: imprimir el CRC generado
        uartUSB.write("Generated CRC32:\r\n", strlen("Generated CRC32:\r\n"));
        uartUSB.write(crcStr, strlen(crcStr));
        uartUSB.write("\r\n", 3);

        // Calcular la longitud total del mensaje (incluyendo el CRC)
        size_t totalLength = messageLength + strlen(crcStr) + 1; // +1 para el terminador nulo
        if (totalLength >= 256) { // Asegurarse de que el mensaje no supere el límite de 255 caracteres
            uartUSB.write("Message too long to append CRC\r\n", strlen("Message too long to append CRC\r\n"));
            return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
        }

        // Anexar el CRC al final del mensaje
        strcat(message, crcStr);

        // Depuración: imprimir el mensaje completo con CRC
        uartUSB.write("Complete message with CRC:\r\n", strlen("Complete message with CRC:\r\n"));
        uartUSB.write(message, strlen(message));
        uartUSB.write("\r\n", 3);

    } else {
        uartUSB.write("Failed to compute CRC\r\n", strlen("Failed to compute CRC\r\n"));
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    // Pasar el mensaje al siguiente manejador si existe
    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message);
    }
}










