//=====[Libraries]=============================================================
#include "DecrypterBase64.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE_BUFFER_SIZE 64

#define LOG_ERROR_BASE64_DECODE          "\r\nError decodificando en base64: %d\r\n"
#define LOG_ERROR_BASE64_DECODE_LEN(x)   (snprintf(nullptr,0,LOG_ERROR_BASE64_DECODE,x))

#define LOG_ENCRYPTED_MESSAGE            "\r\nencrypted message:\r\n"
#define LOG_ENCRYPTED_MESSAGE_LEN        (sizeof(LOG_ENCRYPTED_MESSAGE)-1)

#define LOG_DECRYPTED_MESSAGE            "\r\ndecrypted message:\r\n"
#define LOG_DECRYPTED_MESSAGE_LEN        (sizeof(LOG_DECRYPTED_MESSAGE)-1)


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

}

MessageHandlerStatus_t  DecrypterBase64::handleMessage (char * message, unsigned int sizeOfMessage) {
    static char log [LOG_MESSAGE_BUFFER_SIZE];
    int ret;
    size_t decoded_len = 0;
    static bool initialization = false;

    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }
    
    ret = mbedtls_base64_decode((unsigned char*)this->base64_decoded, this->sizeOfBuffer, &decoded_len,
                                (unsigned char*)message, strlen (message));

    if (ret != 0) {
        snprintf(log, sizeof(log), LOG_ERROR_BASE64_DECODE, ret);
        uartUSB.write (log, strlen (log));  // debug only
        this->base64_decoded[decoded_len] = '\0';
    }

    uartUSB.write (LOG_ENCRYPTED_MESSAGE, LOG_ENCRYPTED_MESSAGE_LEN);  // debug only
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
    uartUSB.write (LOG_DECRYPTED_MESSAGE, LOG_DECRYPTED_MESSAGE_LEN);  // debug only
    uartUSB.write ( message, strlen (  message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->nextHandler == nullptr) {
        initialization = false;
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        initialization = false;
        return this->nextHandler->handleMessage ( message, sizeOfMessage);
    }
    
}


//=====[Implementations of private methods]===================================









