//=====[Libraries]=============================================================
#include "Encrypter.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_ORIGINAL_MESSAGE               "\r\nOriginal message:\r\n"
#define LOG_ORIGINAL_MESSAGE_LEN           (sizeof(LOG_ORIGINAL_MESSAGE)-1)

#define LOG_ENCRYPTED_MESSAGE              "\r\nEncrypted message:\r\n"
#define LOG_ENCRYPTED_MESSAGE_LEN          (sizeof(LOG_ENCRYPTED_MESSAGE)-1)

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
    
    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }
    
    uartUSB.write(LOG_ORIGINAL_MESSAGE, LOG_ORIGINAL_MESSAGE_LEN);
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 2);

    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    this->aes->encrypt(message, sizeOfMessage);
    this->aes->clear();

    uartUSB.write(LOG_ENCRYPTED_MESSAGE, LOG_ENCRYPTED_MESSAGE_LEN);
    uartUSB.write(message, strlen(message)); 
    uartUSB.write("\r\n", 2);

    // call to the next handler
    if (this->nextHandler != nullptr) {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    } else {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    }
}

//=====[Implementations of private methods]===================================














