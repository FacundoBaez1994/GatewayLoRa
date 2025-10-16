//=====[Libraries]=============================================================
#include "EncrypterBase64.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE_BUFFER_SIZE 64

#define LOG_ORIGINAL_MESSAGE               "\r\nOriginal message:\r\n"
#define LOG_ORIGINAL_MESSAGE_LEN           (sizeof(LOG_ORIGINAL_MESSAGE)-1)

#define LOG_ENCRYPTED_MESSAGE              "\r\nEncrypted message:\r\n"
#define LOG_ENCRYPTED_MESSAGE_LEN          (sizeof(LOG_ENCRYPTED_MESSAGE)-1)

#define LOG_ENCRYPTED_BASE64_MESSAGE       "\r\nEncrypted + Base64:\r\n"
#define LOG_ENCRYPTED_BASE64_MESSAGE_LEN   (sizeof(LOG_ENCRYPTED_BASE64_MESSAGE)-1)

#define LOG_ERROR_BASE64_ENCODE            "\r\nFailed convertion Error code: %d\r\n"
#define LOG_ERROR_BASE64_ENCODE_LEN(x)     (snprintf(nullptr,0,LOG_ERROR_BASE64_ENCODE,x)) 
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
EncrypterBase64::EncrypterBase64  () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}

EncrypterBase64::~EncrypterBase64 () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t EncrypterBase64::handleMessage(char* message,  unsigned int sizeOfMessage) {
    static char log [LOG_MESSAGE_BUFFER_SIZE];
    static bool initialization = false;

    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }

    uartUSB.write(LOG_ORIGINAL_MESSAGE, LOG_ORIGINAL_MESSAGE_LEN);
    uartUSB.write(message, strlen(message)); 
    uartUSB.write("\r\n", 2);

    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
     int encryptedSize = ((sizeOfMessage + 15) / 16) * 16;
    this->aes->encrypt(message, encryptedSize);
    this->aes->clear();

    uartUSB.write(LOG_ENCRYPTED_MESSAGE, LOG_ENCRYPTED_MESSAGE_LEN);
    uartUSB.write(message, strlen(message));
    uartUSB.write("\r\n", 2);


    size_t olen = 0;
    int ret = mbedtls_base64_encode((unsigned char*)this->base64_encoded, this->sizeOfBuffer, &olen,
                                    (unsigned char*)message, encryptedSize);
    if (ret != 0) {
        snprintf(log, sizeof(log), LOG_ERROR_BASE64_ENCODE, ret);
        uartUSB.write (log, strlen (log));  // debug only
    } else {
        base64_encoded[olen] = '\0';
    }

    strcpy (message, base64_encoded);


    uartUSB.write(LOG_ENCRYPTED_BASE64_MESSAGE, LOG_ENCRYPTED_BASE64_MESSAGE_LEN);
    uartUSB.write(message, strlen(message));  //
    uartUSB.write("\r\n", 2);


    // Llamada al siguiente handler
    if (this->nextHandler != nullptr) {
        initialization = false;
        //delete [] this->base64_encoded ;
        //this->base64_encoded  = nullptr;
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    } else {
        initialization = false;
        //delete [] this->base64_encoded ;
        //this->base64_encoded  = nullptr;
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    }
}

//=====[Implementations of private methods]===================================









