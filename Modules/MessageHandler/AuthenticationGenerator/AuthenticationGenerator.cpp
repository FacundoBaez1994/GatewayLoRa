//=====[Libraries]=============================================================
#include "AuthenticationGenerator.h"
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

AuthenticationGenerator::AuthenticationGenerator () {
    this->nextHandler = nullptr;
}


AuthenticationGenerator::~AuthenticationGenerator () {
    this->nextHandler = nullptr;
}

MessageHandlerStatus_t AuthenticationGenerator::handleMessage(char* message, unsigned int sizeOfMessage) {
    unsigned char keyhmac[] = "KURRRVWWWWAAAAA";
    unsigned char hmac[32]; 
    size_t message_len = strlen(message); 
    
    generate_hmac(keyhmac, strlen((char*)keyhmac), (unsigned char*)message, message_len, hmac);

    uartUSB.write ("HMAC:\r\n", strlen ("HMAC:\r\n"));  // debug only
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

    uartUSB.write("Encrypted message with HMAC:\r\n", strlen("Encrypted message with HMAC:\r\n"));  // debug only
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

    // Inicializar el contexto de HMAC
    mbedtls_md_init(&ctx);

    // Seleccionar el tipo de hash (SHA-256, por ejemplo)
    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    // Inicializar el contexto con la información del hash
    mbedtls_md_setup(&ctx, md_info, 1);

    // Procesar el mensaje con la clave para generar el HMAC
    mbedtls_md_hmac_starts(&ctx, key, key_len);
    mbedtls_md_hmac_update(&ctx, message, message_len);
    mbedtls_md_hmac_finish(&ctx, hmac_out);

    // Limpiar el contexto
    mbedtls_md_free(&ctx);
}









