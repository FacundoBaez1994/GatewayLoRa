//=====[Libraries]=============================================================
#include "Encrypter.h"
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
Encrypter::Encrypter () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}

Encrypter::~Encrypter () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t Encrypter::handleMessage(char* message,  unsigned int sizeOfMessage) {
    
    uartUSB.write("\r\nOriginal message:\r\n", strlen("\r\nOriginal message:\r\n"));
    uartUSB.write(message, strlen(message));  // advertencia: puede fallar si hay '\0'
    uartUSB.write("\r\n", 2);

    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    this->aes->encrypt(message, sizeOfMessage);
    this->aes->clear();

    uartUSB.write("\r\nEncrypted message:\r\n", strlen("\r\nEncrypted message:\r\n"));
    uartUSB.write(message, strlen(message));  // advertencia: puede fallar si hay '\0'
    uartUSB.write("\r\n", 2);

    // call to the next handler
    if (this->nextHandler != nullptr) {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    } else {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    }
}

//=====[Implementations of private methods]===================================









