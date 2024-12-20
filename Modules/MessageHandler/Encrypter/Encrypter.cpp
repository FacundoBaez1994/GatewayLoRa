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
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
Encrypter::Encrypter () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}


/** 
* @brief 
* 
* 
* @returns 
*/
Encrypter::~Encrypter () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  Encrypter::handleMessage (char * message) {
    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ("Generating message:\r\n", strlen ("Generating message:\r\n"));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    uartUSB.write ("Original message:\r\n", strlen ("Original message:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only


    // Ajuste del cifrado AES
    //size_t plaintext_len = strlen(plaintext);
    //size_t padded_len = ((plaintext_len + 15) / 16) * 16; // Múltiplo más cercano de 16
   // char ciphertext[300];

    // size_t plaintextLen = strlen(message);
    //applyPadding(message, plaintextLen, 16);
    this->aes->setup(key, AES::KEY_256, AES::MODE_CBC, iv);
    this->aes->encrypt(message, strlen (message) ); // Cifrar con padding
    this->aes->clear();
    

    // Imprimir el mensaje cifrado
    uartUSB.write ("Encrypted message:\r\n", strlen ("Encrypted message:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->nextHandler != nullptr) {
        return this->nextHandler->handleMessage(message);
    } else {
        uartUSB.write ("nextHandler->handleMessage:\r\n", strlen ("nextHandler->handleMessage:\r\n"));  // debug only
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    }
   
}

//=====[Implementations of private methods]===================================









