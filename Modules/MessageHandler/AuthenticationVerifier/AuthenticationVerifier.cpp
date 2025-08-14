//=====[Libraries]=============================================================
#include "AuthenticationVerifier.h"
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
AuthenticationVerifier::AuthenticationVerifier () {
    this->nextHandler = nullptr;
}

AuthenticationVerifier::~AuthenticationVerifier () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t  AuthenticationVerifier::handleMessage (char * message, unsigned int sizeOfMessage) {
     unsigned char keyhmac [] = "KURRRVWWWWAAAAA";


    size_t packetSize = strlen (message);
    size_t hmacOffset = packetSize - 32; // last 32 bytes
    unsigned char receivedHMAC[32] = {0};

    uartUSB.write ("message in AuthenticationVerifier:\r\n", strlen ("message in AuthenticationVerifier:\r\n"));  // debug only
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    memcpy(receivedHMAC, &message[hmacOffset], 32);

    uartUSB.write ("Hmac received:\r\n", strlen ("Hmac received:\r\n"));  // debug only
    uartUSB.write (receivedHMAC, sizeof (receivedHMAC));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    unsigned char calculatedHMAC[32];
    
    message [hmacOffset] = '\0';
    
    uartUSB.write ("message received without HMAC\r\n", strlen ("message received without HMAC\r\n"));  // debug only
    uartUSB.write ( message , strlen(message ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    this->generate_hmac(keyhmac, strlen((char *)keyhmac), (unsigned char *) message, strlen (message), calculatedHMAC);

    uartUSB.write ("Hmac calculated:\r\n", strlen ("Hmac calculated:\r\n"));  // debug only
    uartUSB.write (calculatedHMAC, strlen((char *)calculatedHMAC));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    int nullCount = 0;
    for (int i = 0; i < 32; i++) {
        if (calculatedHMAC[i] == 0) { // 0 per'\0'
            nullCount++;
            calculatedHMAC[i] = '0';
        }
    }

    // compare
    if (memcmp(receivedHMAC, calculatedHMAC, sizeof(receivedHMAC)) == 0) {
        uartUSB.write("HMAC OK!\r\n", strlen("HMAC OK!\r\n")); 
    } else {
        uartUSB.write("HMAC Error\r\n", strlen("HMAC Error\r\n"));
        return MESSAGE_HANDLER_STATUS_AUTHENTIFICATION_MISSMATCH;
    }

    uartUSB.write ("encrypted Message Received Without HMAC:\r\n", strlen ("encrypted Message Received Without HMAC:\r\n"));  // debug only
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

    // Init HMAC context
    mbedtls_md_init(&ctx);

    // select Hash Type
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    // initialize context with info
    mbedtls_md_setup(&ctx, md_info, 1);

    // process message with the key in order to generate the hmac
    mbedtls_md_hmac_starts(&ctx, key, key_len);
    mbedtls_md_hmac_update(&ctx, message, message_len);
    mbedtls_md_hmac_finish(&ctx, hmac_out);

    // clear context
    mbedtls_md_free(&ctx);
}









