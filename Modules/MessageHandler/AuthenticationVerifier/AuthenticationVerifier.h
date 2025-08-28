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
class MessageHandler; //< Forward declaration

//=====[Declaration of public classes]=========================================
/**
 * @class AuthenticationVerifier
 * @brief Concrete handler that validates HMAC-SHA256 for incoming messages.
 * @details This class checks whether the last 32 bytes of a message match
 * the HMAC-SHA256 of the content. It helps ensure authenticity and integrity.
 * It participates in a Chain of Responsibility for message processing.
 */
class AuthenticationVerifier : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor for AuthenticationVerifier.
     */
    AuthenticationVerifier();

    /**
     * @brief Destructor for AuthenticationVerifier.
     */
    virtual ~AuthenticationVerifier ();

    /**
     * @brief Verifies the HMAC signature at the end of the message.
     * @param message Null-terminated string containing the message + HMAC.
     * @param sizeOfMessage Total size of the buffer (may be unused here).
     * @return MessageHandlerStatus_t Result of the verification.
     * @retval MESSAGE_HANDLER_STATUS_PROCESSED If HMAC is valid and no next handler exists.
     * @retval MESSAGE_HANDLER_STATUS_AUTHENTIFICATION_MISSMATCH If verification fails.
     */
    virtual MessageHandlerStatus_t handleMessage (char * message, unsigned int sizeOfMessage) override;

private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Computes HMAC-SHA256 using the specified key and message.
     * @param key Pointer to the HMAC key.
     * @param key_len Length of the key.
     * @param message Pointer to the message content (excluding HMAC).
     * @param message_len Length of the message.
     * @param hmac_out Output buffer for the resulting HMAC (must be at least 32 bytes).
     */
    void generate_hmac(const unsigned char *key, size_t key_len, const unsigned char *message, size_t message_len, unsigned char *hmac_out);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _AUTHENTICATION_VERIFIER_H_