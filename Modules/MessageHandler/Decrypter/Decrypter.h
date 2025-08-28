//=====[#include guards - begin]===============================================

#ifndef _DECRYPTER_H_
#define _DECRYPTER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include "mbedtls/base64.h"
#include "AES.h"
#include <string.h>
#include <stdio.h>
 
//=====[Declaration of public data types]======================================
class MessageHandler; //debido a declaracion adelantada
//struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  Abstract class 
 * 
 */
class Decrypter : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    Decrypter();
    virtual ~Decrypter ();
    virtual MessageHandlerStatus_t handleMessage (char * message, unsigned int sizeOfMessage) override;

private:
    AES  * aes; ///< AES cipher instance
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
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _DECRYPTER_H_