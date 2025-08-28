//=====[Libraries]=============================================================
#include "EncrypterBase64.h"
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
EncrypterBase64::EncrypterBase64  () {
    this->aes = new AES ();
    this->nextHandler = nullptr;
}

EncrypterBase64::~EncrypterBase64 () {
    delete this->aes;
    this->aes = nullptr;
    this->nextHandler = nullptr;

    delete [] this->base64_encoded ;
    this->base64_encoded  = nullptr;
}

MessageHandlerStatus_t EncrypterBase64::handleMessage(char* message,  unsigned int sizeOfMessage) {
    static char log [120];
    static bool initialization = false;

    if (initialization  == false) {
        this->base64_encoded = new char [this->sizeOfBuffer];
        initialization = true;
    }

    uartUSB.write("\r\nOriginal message:\r\n", strlen("\r\nOriginal message:\r\n"));
    uartUSB.write(message, strlen(message));  // advertencia: puede fallar si hay '\0'
    uartUSB.write("\r\n", 2);

    this->aes->setup(this->key, AES::KEY_256, AES::MODE_CBC, this->iv);
     int encryptedSize = ((sizeOfMessage + 15) / 16) * 16;
    this->aes->encrypt(message, encryptedSize);
    this->aes->clear();

    uartUSB.write("\r\nEncrypted message:\r\n", strlen("\r\nEncrypted message:\r\n"));
    uartUSB.write(message, strlen(message));  // advertencia: puede fallar si hay '\0'
    uartUSB.write("\r\n", 2);


    size_t olen = 0;
    int ret = mbedtls_base64_encode((unsigned char*)this->base64_encoded, this->sizeOfBuffer, &olen,
                                    (unsigned char*)message, encryptedSize);
    if (ret != 0) {
        snprintf(log, sizeof(log), "\r\nError codificando en base64: %d\r\n", ret);
        uartUSB.write (log, strlen (log));  // debug only
    } else {
        base64_encoded[olen] = '\0';
        //uartUSB.write ("\r\nEncrypted + Base64:\r\n", strlen ("\r\nEncrypted + Base64:\r\n"));  // debug only
        //uartUSB.write (base64_encoded, strlen (base64_encoded));
    }

    strcpy (message, base64_encoded);


    uartUSB.write("\r\nEncrypted + Base64:\r\n", strlen("\r\nEncrypted + Base64:\r\n"));
    uartUSB.write(message, strlen(message));  //
    uartUSB.write("\r\n", 2);


    // Llamada al siguiente handler
    if (this->nextHandler != nullptr) {
        initialization = false;
        delete [] this->base64_encoded ;
        this->base64_encoded  = nullptr;
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    } else {
        initialization = false;
        delete [] this->base64_encoded ;
        this->base64_encoded  = nullptr;
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    }
}

//=====[Implementations of private methods]===================================









