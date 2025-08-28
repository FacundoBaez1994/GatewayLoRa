//=====[#include guards - begin]===============================================

#ifndef _ENCRYPTER_H_
#define _ENCRYPTER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include "AES.h"
#include "mbedtls/base64.h"
#include <string.h>
#include <stdio.h>
 
//=====[Declaration of public data types]======================================
class MessageHandler; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/**
 * @class Encrypter
 * @brief Concrete handler in the Chain of Responsibility for message encryption.
 * @details This class performs AES-256 encryption (CBC mode) on incoming messages
 * before passing them to the next handler. It inherits from BaseMessageHandler and
 * overrides the handleMessage() method to apply encryption.
 */
class Encrypter : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs an Encrypter handler.
     */
    Encrypter();

    /**
     * @brief Destroys the Encrypter and releases internal resources.
     */
    virtual ~Encrypter ();

    /**
     * @brief Handles a message by encrypting it with AES and passing it to the next handler.
     * @param message Pointer to the message buffer to encrypt.
     * @param sizeOfMessage Size of the message in bytes.
     * @return Status code representing the result of the operation.
    */
    virtual MessageHandlerStatus_t handleMessage (char* message,  unsigned int sizeOfMessage) override;

private:
    AES* aes; ///< AES cipher instance used for encryption
    const char key[32] = { ///< AES-256 key (32 bytes)
        0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE,
        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81,
        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7,
        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4
    };
    const char iv[16] = {  ///< AES CBC initialization vector (16 bytes)
        0x74, 0x11, 0xF0, 0x45, 0xD6, 0xA4, 0x3F, 0x69,
        0x18, 0xC6, 0x75, 0x42, 0xDF, 0x4C, 0xA7, 0x84
    };
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ENCRYPTER_H_