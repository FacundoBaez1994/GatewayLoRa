//=====[Libraries]=============================================================
#include "AuthenticationVerifier.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE_IN_AUTH_VERIFIER             "message in AuthenticationVerifier:\r\n"
#define LOG_MESSAGE_IN_AUTH_VERIFIER_LEN         (sizeof(LOG_MESSAGE_IN_AUTH_VERIFIER) - 1)

#define LOG_HMAC_RECEIVED                        "Hmac received:\r\n"
#define LOG_HMAC_RECEIVED_LEN                    (sizeof(LOG_HMAC_RECEIVED) - 1)

#define LOG_MESSAGE_WITHOUT_HMAC                 "message received without HMAC\r\n"
#define LOG_MESSAGE_WITHOUT_HMAC_LEN             (sizeof(LOG_MESSAGE_WITHOUT_HMAC) - 1)

#define LOG_HMAC_CALCULATED                      "Hmac calculated:\r\n"
#define LOG_HMAC_CALCULATED_LEN                  (sizeof(LOG_HMAC_CALCULATED) - 1)

#define LOG_HMAC_OK                              "HMAC OK!\r\n"
#define LOG_HMAC_OK_LEN                          (sizeof(LOG_HMAC_OK) - 1)

#define LOG_HMAC_ERROR                           "HMAC Error\r\n"
#define LOG_HMAC_ERROR_LEN                       (sizeof(LOG_HMAC_ERROR) - 1)

#define LOG_ENCRYPTED_MSG_WITHOUT_HMAC           "encrypted Message Received Without HMAC:\r\n"
#define LOG_ENCRYPTED_MSG_WITHOUT_HMAC_LEN       (sizeof(LOG_ENCRYPTED_MSG_WITHOUT_HMAC) - 1)

#define HMAC_KEY_VALUE              "KURRRVWWWWAAAAA"
#define HMAC_KEY_VALUE_LEN          (sizeof(HMAC_KEY_VALUE) - 1)

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
AuthenticationVerifier::AuthenticationVerifier () {
    this->nextHandler = nullptr;
}

AuthenticationVerifier::~AuthenticationVerifier () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  AuthenticationVerifier::handleMessage (char * message, unsigned int sizeOfMessage) {
    unsigned char keyhmac [] = HMAC_KEY_VALUE;
    size_t packetSize = strlen (message);
    size_t hmacOffset = packetSize - 32; // last 32 bytes
    unsigned char receivedHMAC[32] = {0};

    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }
    

    uartUSB.write (LOG_MESSAGE_IN_AUTH_VERIFIER, LOG_MESSAGE_IN_AUTH_VERIFIER_LEN);  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    memcpy(receivedHMAC, &message[hmacOffset], 32);

    uartUSB.write (LOG_HMAC_RECEIVED, LOG_HMAC_RECEIVED_LEN);  // debug only
    uartUSB.write (receivedHMAC, sizeof (receivedHMAC));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    unsigned char calculatedHMAC[32];
    
    message [hmacOffset] = '\0';
    
    uartUSB.write (LOG_MESSAGE_WITHOUT_HMAC, LOG_MESSAGE_WITHOUT_HMAC_LEN);  // debug only
    uartUSB.write ( message , strlen(message ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    this->generate_hmac(keyhmac, strlen((char *)keyhmac), (unsigned char *) message, strlen (message), calculatedHMAC);

    uartUSB.write (LOG_HMAC_CALCULATED, LOG_HMAC_CALCULATED_LEN);  // debug only
    uartUSB.write (calculatedHMAC, strlen((char *)calculatedHMAC));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    int nullCount = 0;
    for (int i = 0; i < 32; i++) {
        if (calculatedHMAC[i] == 0) { // 0 per'\0'
            nullCount++;
            calculatedHMAC[i] = '0';
        }
    }

    if (memcmp(receivedHMAC, calculatedHMAC, sizeof(receivedHMAC)) == 0) {
        uartUSB.write(LOG_HMAC_OK, LOG_HMAC_OK_LEN); 
    } else {
        uartUSB.write(LOG_HMAC_ERROR, LOG_HMAC_ERROR_LEN);
        return MESSAGE_HANDLER_STATUS_AUTHENTIFICATION_MISSMATCH;
    }

    uartUSB.write (LOG_ENCRYPTED_MSG_WITHOUT_HMAC, LOG_ENCRYPTED_MSG_WITHOUT_HMAC_LEN);  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    
    if (this->nextHandler == nullptr) {
        return  MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage (message, sizeOfMessage);
    }
    
}

//=====[Implementations of private methods]===================================
void AuthenticationVerifier::generate_hmac(const unsigned char *key, size_t key_len, const unsigned char *message, size_t message_len, unsigned char *hmac_out)
{
    mbedtls_md_context_t ctx;
    const mbedtls_md_info_t *md_info;

    mbedtls_md_init(&ctx);

    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    mbedtls_md_setup(&ctx, md_info, 1);

    mbedtls_md_hmac_starts(&ctx, key, key_len);
    mbedtls_md_hmac_update(&ctx, message, message_len);
    mbedtls_md_hmac_finish(&ctx, hmac_out);

    mbedtls_md_free(&ctx);
}












