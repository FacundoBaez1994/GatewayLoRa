//=====[Libraries]=============================================================

#include "Decrypter.h"
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
Decrypter::Decrypter () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}

Decrypter::~Decrypter () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  Decrypter::handleMessage (char * message, unsigned int sizeOfMessage) {

    uartUSB.write ("\r\nencrypted message:\r\n", strlen ("encrypted message:\r\n"));  // debug only
    uartUSB.write (message, strlen ( message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    
    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    this->aes->decrypt(message , sizeOfMessage);
    this->aes->clear();


    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ("\r\ndecrypted message:\r\n", strlen ("decrypted message:\r\n"));  // debug only
    uartUSB.write ( message, strlen (  message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage ( message, sizeOfMessage);
    }
    
}

//=====[Implementations of private methods]===================================









