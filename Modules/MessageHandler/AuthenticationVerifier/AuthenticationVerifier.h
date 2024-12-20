//=====[#include guards - begin]===============================================

#ifndef _AUTHENTICATION_VERIFIER_H_
#define _AUTHENTICATION_VERIFIER_H_

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
/*
 *  Abstract class 
 * 
 */
class AuthenticationVerifier : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    AuthenticationVerifier();
    virtual ~AuthenticationVerifier ();
    virtual MessageHandlerStatus_t handleMessage (char * message) override;
    //virtual MessageHandler setNextHandler (MessageHandler * nextHandler);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
    void generate_hmac(const unsigned char *key, size_t key_len, const unsigned char *message, size_t message_len, unsigned char *hmac_out);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _AUTHENTICATION_VERIFIER_H_