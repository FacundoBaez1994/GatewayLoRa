//=====[Libraries]=============================================================

#include "ATCommandHandler.h"
#include <string>
#include <cwchar>
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
ATCommandHandler::ATCommandHandler ( BufferedSerial * UART) {
    this->bufferIndex = 0;
    this->serialComunicationUART = UART;
}


ATCommandHandler::~ATCommandHandler () {
    delete  this->serialComunicationUART;
}


/** 
* @brief 
* 
* 
* @returns 
*/
void ATCommandHandler::sendATCommand (char * ATCommandToBeSend) {
    this->serialComunicationUART->write (ATCommandToBeSend, strlen (ATCommandToBeSend));  
    this->serialComunicationUART->write ( "\r\n",  3 ); 
}



/** 
* @brief 
* 
* 
* @returns 
*/
void ATCommandHandler::sendATCommand (const char * ATCommandToBeSend, size_t length) {
    this->serialComunicationUART->write(ATCommandToBeSend, length);  
    this->serialComunicationUART->write("\r\n", 3); 
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool ATCommandHandler::readATResponse (char * StringToBeRead) {
    char receivedCharLocal;
    if (this->serialComunicationUART->readable()) { // READ
        this->serialComunicationUART->read(&receivedCharLocal, 1);
        if (receivedCharLocal == '\r' || receivedCharLocal == '\n') { 
            if (this->bufferIndex > 0) {
                this->buffer[this->bufferIndex] = '\0'; // Null-terminate the string
                this->bufferIndex = 0; // Reset buffer index for the next string
                strcpy(StringToBeRead, this->buffer);
                return true;
                }
        } else {
            if (this->bufferIndex < sizeof(this->buffer) - 1) { // Ensure there is space in the buffer
                this->buffer[this->bufferIndex++] = receivedCharLocal;
            }
        }
    }
    return false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
bool ATCommandHandler::readChar (char * charRead) {
    char receivedCharLocal;
    char StringToBeSendUSB [2] = "";
    if (this->serialComunicationUART->readable()) { // READ
        this->serialComunicationUART->read(&receivedCharLocal, 1);
        StringToBeSendUSB [0] = receivedCharLocal;
      //  uartUSB.write(StringToBeSendUSB, strlen(StringToBeSendUSB));  // debug only
      //  uartUSB.write("\r\n", 3);  // debug only

        *charRead = receivedCharLocal;
        return true;
    }
    return false;
}

BufferedSerial* ATCommandHandler::getUART (void) {
    return this->serialComunicationUART;
}


//=====[Implementations of private functions]==================================
