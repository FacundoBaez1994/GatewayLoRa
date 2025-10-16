//=====[#include guards - begin]===============================================
#ifndef _ENCRYPTER_BASE_64_H_
#define _ENCRYPTER_BASE_64_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include "AES.h"
#include "mbedtls/base64.h"
#include <string.h>
#include <stdio.h>
#include "BufferSizes.h"

//=====[Declaration of public defines]=========================================
#define BUFFER_SIZE_ADDITION_BASE64_ENCRYPION 548
 
//=====[Declaration of public data types]======================================
class MessageHandler; //< Forward declaration to avoid circular dependency

//=====[Declaration of public classes]=========================================
/**
 * @class EncrypterBase64
 * @brief Encrypts messages with AES-256-CBC and encodes them in Base64.
 * @details This class implements the Chain of Responsibility design pattern by
 * inheriting from BaseMessageHandler and optionally passing messages to the
 * next handler. It also acts as a Facade over the AES and Base64 encoding 
 * libraries to simplify encryption logic.
 */
class EncrypterBase64 : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs a new EncrypterBase64 object.
     */
    EncrypterBase64 ();

    /**
     * @brief Destroys the EncrypterBase64 object.
     */
    virtual ~EncrypterBase64  ();

    /**
     * @brief Encrypts and Base64-encodes the message.
     * @param message Pointer to the message buffer to encrypt and encode.
     * @param sizeOfMessage Size of the original message in bytes.
     * @return MessageHandlerStatus_t Status after handling the message.
     */
    virtual MessageHandlerStatus_t handleMessage (char* message,  unsigned int sizeOfMessage) override;
private:
//=====[Declaration of privates atributes]=========================================
    char base64_encoded [BUFFER_SIZE_ADDITION_BASE64_ENCRYPION + MESSAGE_BUFFER_SIZE];           //< Pointer to hold Base64-encoded result
    size_t sizeOfBuffer = BUFFER_SIZE_ADDITION_BASE64_ENCRYPION + MESSAGE_BUFFER_SIZE;      //< Internal buffer size for encoded data
    AES* aes;                     //< AES encryption helper instance

//=====[Declaration of privates constants]=========================================
    const char key[32] = {      //< AES-256 encryption key
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };
    const char iv[16] = {       //< AES initialization vector (CBC mode)
        0x74, 0x11, 0xF0, 0x45, 0xD6, 0xA4, 0x3F, 0x69,
        0x18, 0xC6, 0x75, 0x42, 0xDF, 0x4C, 0xA7, 0x84
    };

//=====[Declaration of privates methods]=========================================
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ENCRYPTER_BASE_64_H_