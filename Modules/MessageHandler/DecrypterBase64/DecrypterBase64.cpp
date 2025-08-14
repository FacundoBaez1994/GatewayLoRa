//=====[Libraries]=============================================================
#include "DecrypterBase64.h"
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
DecrypterBase64::DecrypterBase64 () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}

DecrypterBase64::~DecrypterBase64 () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;

    delete [] this->base64_decoded ;
    this->base64_decoded = nullptr;
}

MessageHandlerStatus_t  DecrypterBase64::handleMessage (char * message, unsigned int sizeOfMessage) {
    static char log [120];
    int ret;
    size_t decoded_len = 0;
    static bool initialization = false;

    if (initialization  == false) {
         this->base64_decoded = new char [this->sizeOfBuffer];
        initialization = true;
    }

    ret = mbedtls_base64_decode((unsigned char*)this->base64_decoded, this->sizeOfBuffer, &decoded_len,
                                (unsigned char*)message, strlen (message));

    if (ret != 0) {
        snprintf(log, sizeof(log), "\r\nError decodificando en base64: %d\r\n", ret);
        uartUSB.write (log, strlen (log));  // debug only
        this->base64_decoded[decoded_len] = '\0';
    }

    uartUSB.write ("\r\nencrypted message:\r\n", strlen ("encrypted message:\r\n"));  // debug only
    uartUSB.write (this->base64_decoded, strlen ( this->base64_decoded));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    
    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
    this->aes->decrypt(this->base64_decoded , sizeOfMessage);
    this->aes->clear();

    int padding = this->base64_decoded[decoded_len - 1]; // last byte = padding quantity
    if (padding > 0 && padding <= 16) {
        decoded_len -= padding; 
    }

    decoded_len = (decoded_len < sizeOfMessage) ? decoded_len : sizeOfMessage - 1;
    memcpy(message, this->base64_decoded, decoded_len);
    message[decoded_len] = '\0'; 

    uartUSB.write ( "\r\n",  3 );
    uartUSB.write ("\r\ndecrypted message:\r\n", strlen ("decrypted message:\r\n"));  // debug only
    uartUSB.write ( message, strlen (  message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->nextHandler == nullptr) {
        initialization = false;
        delete [] this->base64_decoded ;
        this->base64_decoded = nullptr;
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        delete [] this->base64_decoded ;
        initialization = false;
        this->base64_decoded = nullptr;
        return this->nextHandler->handleMessage ( message, sizeOfMessage);
    }
    
}


//=====[Implementations of private methods]===================================









