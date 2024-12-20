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

MessageHandlerStatus_t  ChecksumGenerator::handleMessage (char * message) {
  
    // Crear una instancia de MbedCRC con el polinomio 32-bit ANSI
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;
    // Variable para almacenar el resultado del CRC
    uint32_t crc;

    if (crc32.compute(reinterpret_cast<uint8_t*>(message), strlen(message), &crc) == 0) {
        char crcStr[12]; // Espacio suficiente para almacenar un uint32_t en hexadecimal
        sprintf(crcStr, "0x%08X", crc); // Convertir a formato hexadecimal
        uartUSB.write("generated CRC32:\r\n", strlen("generated CRC32:\r\n")); // Imprimir etiqueta
        uartUSB.write(crcStr, strlen(crcStr)); // Imprimir el CRC en formato texto
        uartUSB.write("\r\n", 3); // Nueva línea

        size_t messageLength = strlen(message);

        // Añadir el CRC al final del buffer
        message[messageLength]     = (crc >> 24) & 0xFF;
        message[messageLength + 1] = (crc >> 16) & 0xFF;
        message[messageLength + 2] = (crc >> 8) & 0xFF;
        message[messageLength + 3] = crc & 0xFF;

        // Asegurarte de ajustar el tamaño total
        messageLength += 4;
        message[messageLength] = '\0'; 


        uartUSB.write ("Complete message with CRC:\r\n", strlen ("Complete message with CRC:\r\n"));  // debug only
        uartUSB.write (message, messageLength);  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    } else {
        uartUSB.write ("Failed to compute CRC\r\n", strlen ("Failed to compute CRC\r\n"));  // debug only
        return MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM;
    }

    
    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage (message);
    }
}










