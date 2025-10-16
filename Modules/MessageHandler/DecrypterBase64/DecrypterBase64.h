//=====[#include guards - begin]===============================================
#ifndef _DECRYPTER_BASE_64_H_
#define _DECRYPTER_BASE_64_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include "mbedtls/base64.h"
#include "AES.h"
#include <string.h>
#include <stdio.h>
#include "BufferSizes.h"
 
//=====[Declaration of public data types]======================================
class MessageHandler;  ///< Forward declaration


#define BUFFER_SIZE_ADDITION_BASE64_DECRYPION 164
//=====[Declaration of public classes]=========================================
/**
 * @class DecrypterBase64
 * @brief Concrete handler in the Chain of Responsibility that decrypts messages.
 * @details This class performs Base64 decoding followed by AES-256 decryption (CBC mode)
 * on an incoming message. It is part of a processing chain for secure message handling.
 */
class DecrypterBase64 : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Construct a new DecrypterBase64 object.
     */
    DecrypterBase64();

    /**
     * @brief Destroy the DecrypterBase64 object and free internal resources.
     */
    virtual ~DecrypterBase64 ();

    /**
     * @brief Handles the decryption and decoding of a message.
     * @param message Pointer to the message buffer.
     * @param sizeOfMessage Length of the buffer to process.
     * @return Status of the message processing.
     */
    virtual MessageHandlerStatus_t handleMessage (char * message, unsigned int sizeOfMessage) override;

private:
//=====[Declaration of privates atributes]=========================================
    char base64_decoded [MESSAGE_BUFFER_SIZE + BUFFER_SIZE_ADDITION_BASE64_DECRYPION]; ///< Buffer used to store Base64-decoded binary data
    size_t sizeOfBuffer = MESSAGE_BUFFER_SIZE + BUFFER_SIZE_ADDITION_BASE64_DECRYPION; ///< Internal buffer size
    AES* aes; ///< AES cipher instance
//=====[Declaration of privates constants]=========================================
    const char key[32] = { ///< AES-256 key
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };
    const char iv[16] = { ///< Initialization Vector for AES CBC
        0x74, 0x11, 0xF0, 0x45, 0xD6, 0xA4, 0x3F, 0x69,
        0x18, 0xC6, 0x75, 0x42, 0xDF, 0x4C, 0xA7, 0x84
    };
//=====[Declaration of privates methods]=========================================

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _DECRYPTER_BASE_64_H_