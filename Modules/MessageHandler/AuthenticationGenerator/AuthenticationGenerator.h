//=====[#include guards - begin]===============================================

#ifndef _AUTHENTICATION_GENERATOR_H_
#define _AUTHENTICATION_GENERATOR_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include "mbedtls/md.h"
#include <string.h>
#include <stdio.h>
 
//=====[Declaration of public data types]======================================
class MessageHandler; //debido a declaracion adelantada
//struct CellInformation;

//=====[Declaration of public classes]=========================================

/**
 * @class AuthenticationGenerator
 * @brief Concrete handler that generates an HMAC-SHA256 and appends it to the message.
 * @details This class is part of the Chain of Responsibility. It computes an HMAC-SHA256
 * over the message content and appends the resulting 32-byte digest at the end.
 * The resulting message can be validated later by an AuthenticationVerifier.
 */
class AuthenticationGenerator : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor for AuthenticationGenerator.
     */
    AuthenticationGenerator();
    virtual ~AuthenticationGenerator ();
    virtual MessageHandlerStatus_t handleMessage (char * message, unsigned int sizeOfMessage) override;
    //virtual MessageHandler setNextHandler (MessageHandler * nextHandler);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
    void generate_hmac(const unsigned char *key, size_t key_len, const unsigned char *message, size_t message_len, unsigned char *hmac_out);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _AUTHENTICATION_GENERATOR_H_