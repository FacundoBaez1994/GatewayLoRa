//=====[Libraries]=============================================================
#include "AuthenticationGenerator.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define LOG_MESSAGE_HMAC_HEADER                    "HMAC:\r\n"
#define LOG_MESSAGE_HMAC_HEADER_LEN                (sizeof(LOG_MESSAGE_HMAC_HEADER) - 1)

#define LOG_MESSAGE_NEWLINE                        "\r\n"
#define LOG_MESSAGE_NEWLINE_LEN                    (sizeof(LOG_MESSAGE_NEWLINE) - 1)

#define LOG_MESSAGE_ENCRYPTED_WITH_HMAC            "Encrypted message with HMAC:\r\n"
#define LOG_MESSAGE_ENCRYPTED_WITH_HMAC_LEN        (sizeof(LOG_MESSAGE_ENCRYPTED_WITH_HMAC) - 1)

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

AuthenticationGenerator::AuthenticationGenerator () {
    this->nextHandler = nullptr;
}


AuthenticationGenerator::~AuthenticationGenerator () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t AuthenticationGenerator::handleMessage(char* message, unsigned int sizeOfMessage) {
    unsigned char keyhmac[] = HMAC_KEY_VALUE;
    unsigned char hmac[32]; 
    size_t message_len = strlen(message); 

    if (message == nullptr) {
        return MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR;
    }
    
    generate_hmac(keyhmac, strlen((char*)keyhmac), (unsigned char*)message, message_len, hmac);

    uartUSB.write (LOG_MESSAGE_HMAC_HEADER, LOG_MESSAGE_HMAC_HEADER_LEN);  // debug only
    uartUSB.write (hmac, sizeof (hmac));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only


    int nullCount = 0;

    for (int i = 0; i < 32; i++) {
        if (hmac[i] == 0) { // 0 per '\0'
            nullCount++;
            hmac[i] = '0';
        }
    }

    size_t total_len = message_len + sizeof(hmac); 

    // Copy HMAC at the end
    for (size_t i = 0; i < sizeof(hmac); ++i) {
        message[message_len + i] = hmac[i];
    }

    message[ total_len] = '\0';

    uartUSB.write(LOG_MESSAGE_ENCRYPTED_WITH_HMAC, LOG_MESSAGE_ENCRYPTED_WITH_HMAC_LEN);  // debug only
    uartUSB.write(message, total_len);  // debug only
    uartUSB.write("\r\n", 3);  // debug only
    if (this->nextHandler == nullptr) {
        return MESSAGE_HANDLER_STATUS_PROCESSED;
    } else {
        return this->nextHandler->handleMessage(message, sizeOfMessage);
    }
}

//=====[Implementations of private methods]===================================
void AuthenticationGenerator::generate_hmac(const unsigned char *key, size_t key_len, const unsigned char *message, size_t message_len, unsigned char *hmac_out)
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















