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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
Decrypter::Decrypter () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Decrypter::~Decrypter () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  Decrypter::handleMessage (char * message) {
    char messageFinal [256];
    aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    aes->decrypt(message, messageFinal, strlen (message));
    aes->clear();

    //removePadding( message,  strlen (message));

    uartUSB.write ("decrypted message:\r\n", strlen ("decrypted message:\r\n"));  // debug only
    uartUSB.write ( messageFinal, strlen ( messageFinal));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->nextHandler == nullptr) {
        memcpy (message, messageFinal, strlen (messageFinal));
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage ( messageFinal);
    }
    
}

//=====[Implementations of private methods]===================================









