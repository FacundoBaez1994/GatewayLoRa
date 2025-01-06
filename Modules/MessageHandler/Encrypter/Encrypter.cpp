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
    char originalMessage [256];
    char decryptedMessage [256];
    bool encryptionSuccess = false; 

    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ("Generating message:\r\n", strlen ("Generating message:\r\n"));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    do {
        strcpy (originalMessage, message);
    
        uartUSB.write ("Original message:\r\n", strlen ("Original message:\r\n"));  // debug only
        uartUSB.write (originalMessage, strlen (originalMessage));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        this->aes->setup (this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
        this->aes->encrypt (message, strlen(message) ); // Cifrar con padding
        this->aes->clear ();

        strcpy (decryptedMessage, message);
        this->aes->setup (this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
        this->aes->decrypt (decryptedMessage, strlen(decryptedMessage) ); // Cifrar con padding
        this->aes->clear ();

        uartUSB.write ("\r\n", strlen ("\r\n"));  // debug only
        uartUSB.write ("First decrip message:\r\n", strlen ("First decrip message:\r\n"));  // debug only
        uartUSB.write (decryptedMessage, strlen (decryptedMessage));  // debug only

        if (strcmp (decryptedMessage, originalMessage) == 0) {
            uartUSB.write ("\r\n", strlen ("\r\n"));  // debug only
            uartUSB.write ("Equals:\r\n", strlen ("Equals\r\n"));  // debug only
            uartUSB.write ("\r\n", strlen ("\r\n"));  // debug only
            encryptionSuccess = true;
        } else {
            uartUSB.write ("\r\n", strlen ("\r\n"));  // debug only
            uartUSB.write ("Not Equals adding Padding\r\n", strlen ("Not Equals adding Padding\r\n"));  // debug only
            uartUSB.write ("\r\n", strlen ("\r\n"));  // debug only

            strcpy (message, originalMessage);
            size_t messageLength = strlen (message);
            message [messageLength] = '@';
            message [messageLength + 1] = '\0';
            encryptionSuccess = false;
        }
     } while (encryptionSuccess == false);
    
     uartUSB.write ("\r\n", strlen ("\r\n"));  // debug only
    uartUSB.write ("Encripted message:\r\n", strlen ("Encripted message:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    
    

    if (this->nextHandler != nullptr) {
        return this->nextHandler->handleMessage(message);
    } else {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    }
   
}

//=====[Implementations of private methods]===================================









