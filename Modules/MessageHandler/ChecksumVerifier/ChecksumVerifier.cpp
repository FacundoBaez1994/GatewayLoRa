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

MessageHandlerStatus_t  ChecksumVerifier::handleMessage (char * message) {
    MbedCRC<POLY_32BIT_ANSI, 32> crc32;

    int packetSize = strlen (message);
    // Separar payload y CRC
     if (packetSize < 4) {
        uartUSB.write("Error: Packet too small for CRC\r\n", strlen("Error: Packet too small for CRC\r\n"));
        return  MESSAGE_HANDLER_STATUS_ERROR_PACKET_TOO_SMALL;
    }
        // Extraer el CRC
    packetSize -= 4; // Ajustar tamaño del paquete sin CRC
    uint32_t receivedCRC = (message[packetSize] << 24) |
                            (message[packetSize + 1] << 16) |
                            (message[packetSize + 2] << 8) |
                            message[packetSize + 3];

    // Verificar el CRC
    uint32_t calculatedCRC;
    // Formatear el CRC en hexadecimal
    char crcString[64]; 
   
    if (crc32.compute(reinterpret_cast<uint8_t*>(message), packetSize, &calculatedCRC) == 0) {
        snprintf(crcString, strlen(crcString), "Recieved CRC32: 0x%08X\r\n", receivedCRC);
        
        // Enviar el mensaje por UART
        uartUSB.write(crcString, strlen(crcString));  // Enviar el CRC
        uartUSB.write ( "\r\n",  3 );  // debug only
        // Formatear el CRC en hexadecimal
        snprintf(crcString, strlen(crcString), "calculated CRC32: 0x%08X\r\n", calculatedCRC);
        // Enviar el mensaje por UART
        uartUSB.write(crcString, strlen(crcString));  // Enviar el CRC
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (calculatedCRC == receivedCRC) {
            uartUSB.write ( "\r\n",  3 );  // debug only
            uartUSB.write("Checksum OK!\r\n", strlen("Checksum OK!\r\n")); // Debug
        } else {
            uartUSB.write ( "\r\n",  3 );  // debug only
             uartUSB.write("Checksum Error\r\n", strlen("Checksum Error\r\n")); // Debug
            return MESSAGE_HANDLER_STATUS_CHECKSUM_ERROR; 
            }
    }

    message [packetSize] = '\0';
    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ("Mensaje Without CRC:\r\n", strlen ("Mensaje Without CRC:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    
    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage (message);
    }
}







