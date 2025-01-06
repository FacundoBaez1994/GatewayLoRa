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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
ChecksumVerifier::ChecksumVerifier () {
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
ChecksumVerifier::~ChecksumVerifier () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t ChecksumVerifier::handleMessage(char *message) {
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;


    int packetSize = strlen(message);
    if (packetSize < 8) { // Si el mensaje es demasiado corto para contener un CRC de 8 caracteres
        uartUSB.write("Error: Packet too small for CRC\r\n", strlen("Error: Packet too small for CRC\r\n"));
        return MESSAGE_HANDLER_STATUS_ERROR_PACKET_TOO_SMALL;
    }

    // Separar el CRC (últimos 8 caracteres del mensaje)
    packetSize -= 8; // Ajustamos el tamaño del paquete para que solo tenga el mensaje sin el CRC
    uint32_t receivedCRC;
    sscanf(&message[packetSize], "%8X", &receivedCRC); // Extraemos el CRC como valor hexadecimal de 8 caracteres

    // Verificar el CRC calculado
    uint32_t calculatedCRC;
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), packetSize, &calculatedCRC) == 0) {
        // Enviar información sobre el CRC calculado y recibido
        char crcString[64];
        snprintf(crcString, sizeof(crcString), "Received CRC32: 0x%08X\r\n", receivedCRC);
        uartUSB.write(crcString, strlen(crcString));

        snprintf(crcString, sizeof(crcString), "Calculated CRC32: 0x%08X\r\n", calculatedCRC);
        uartUSB.write(crcString, strlen(crcString));

        // Verificar si el CRC coincide
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

    // Eliminar el CRC del mensaje original
    message[packetSize] = '\0';  // Asegurarse de que el mensaje tenga un terminador nulo
    uartUSB.write("\r\nMessage Without CRC:\r\n", strlen("\r\nMessage Without CRC:\r\n"));
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 3);  // Debug

    // Pasar el mensaje al siguiente manejador si existe
    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message);
    }
}






