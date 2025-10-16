//=====[Libraries]=============================================================
#include "JWTManager.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

#define LOG_BUFFER                                     256

#define LOG_MESSAGE_DATA_HEADER                        "\r\nData: = %s\r\n"
#define LOG_MESSAGE_DATA_LENGTH                        "\r\nData Length: = %i\r\n"

#define LOG_MESSAGE_GENERATING_JWT                     "Generating a JWT\r\n"
#define LOG_MESSAGE_GENERATING_JWT_LEN                 (sizeof(LOG_MESSAGE_GENERATING_JWT) - 1)

#define LOG_MESSAGE_HEADER_INFO                        "Header Info\r\n"
#define LOG_MESSAGE_HEADER_INFO_LEN                    (sizeof(LOG_MESSAGE_HEADER_INFO) - 1)

#define LOG_MESSAGE_PAYLOAD_INFO                       "Payload Info\r\n"
#define LOG_MESSAGE_PAYLOAD_INFO_LEN                   (sizeof(LOG_MESSAGE_PAYLOAD_INFO) - 1)

#define LOG_MESSAGE_SIGNATURE_INFO                     "Signature Info\r\n"
#define LOG_MESSAGE_SIGNATURE_INFO_LEN                 (sizeof(LOG_MESSAGE_SIGNATURE_INFO) - 1)

#define LOG_MESSAGE_FINAL_OUTPUT_INFO                  "Final Output Info\r\n"
#define LOG_MESSAGE_FINAL_OUTPUT_INFO_LEN              (sizeof(LOG_MESSAGE_FINAL_OUTPUT_INFO) - 1)

#define LOG_MESSAGE_DECODING_JWT                       "Decoding and verifying the JWT\r\n"
#define LOG_MESSAGE_DECODING_JWT_LEN                   (sizeof(LOG_MESSAGE_DECODING_JWT) - 1)

#define LOG_MESSAGE_DECODE_RESULT                      "JWT Decode ended with result:\r\n"
#define LOG_MESSAGE_DECODE_RESULT_LEN                  (sizeof(LOG_MESSAGE_DECODE_RESULT) - 1)

#define LOG_MESSAGE_CODE_RESULT                        "\r\nCode result = %i\r\n"

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private helper functions]===========================
void printData(char *data, size_t dataLen) {
    char logMessage[LOG_BUFFER];
    snprintf(logMessage, sizeof(logMessage), LOG_MESSAGE_DATA_HEADER, data);
    uartUSB.write(logMessage, strlen(logMessage));
    snprintf(logMessage, sizeof(logMessage), LOG_MESSAGE_DATA_LENGTH, dataLen);
    uartUSB.write(logMessage, strlen(logMessage));
}

//=====[Implementations of public methods]=====================================
JWTManager::JWTManager() {
    this->jwt = new CustomJWT(this->JWTKey, 256);
}

JWTManager::~JWTManager() {
    delete this->jwt;
    this->jwt = nullptr;
}

void JWTManager::encodeJWT(char *payloadToJWT, char *jwtEncoded) {
    if (payloadToJWT == nullptr || jwtEncoded == nullptr) {
        return;
    }

    this->jwt->allocateJWTMemory();

    uartUSB.write(LOG_MESSAGE_GENERATING_JWT, LOG_MESSAGE_GENERATING_JWT_LEN);

    this->jwt->encodeJWT(payloadToJWT);

    uartUSB.write(LOG_MESSAGE_HEADER_INFO, LOG_MESSAGE_HEADER_INFO_LEN);
    printData(this->jwt->header, this->jwt->headerLength);

    uartUSB.write(LOG_MESSAGE_PAYLOAD_INFO, LOG_MESSAGE_PAYLOAD_INFO_LEN);
    printData(this->jwt->payload, this->jwt->payloadLength);

    uartUSB.write(LOG_MESSAGE_SIGNATURE_INFO, LOG_MESSAGE_SIGNATURE_INFO_LEN);
    printData(this->jwt->signature, this->jwt->signatureLength);

    uartUSB.write(LOG_MESSAGE_FINAL_OUTPUT_INFO, LOG_MESSAGE_FINAL_OUTPUT_INFO_LEN);
    printData(this->jwt->out, this->jwt->outputLength);

    strncpy(jwtEncoded, this->jwt->out, this->jwt->outputLength);
    jwtEncoded[this->jwt->outputLength] = '\0';
    this->jwt->clear();
}

bool JWTManager::decodeJWT(char *jwtToDecode, char *payloadRetrieved) {
    char logMessage[LOG_BUFFER];

    if (jwtToDecode == nullptr || payloadRetrieved == nullptr) {
        return false;
    }

    this->jwt->allocateJWTMemory();

    uartUSB.write(LOG_MESSAGE_DECODING_JWT, LOG_MESSAGE_DECODING_JWT_LEN);
    uartUSB.write(LOG_MESSAGE_DECODE_RESULT, LOG_MESSAGE_DECODE_RESULT_LEN);

    int responseCode = this->jwt->decodeJWT(jwtToDecode);

    snprintf(logMessage, sizeof(logMessage), LOG_MESSAGE_CODE_RESULT, responseCode);
    uartUSB.write(logMessage, strlen(logMessage));

    if (responseCode != 0) {
        return false;
    }

    uartUSB.write(LOG_MESSAGE_HEADER_INFO, LOG_MESSAGE_HEADER_INFO_LEN);
    printData(this->jwt->header, this->jwt->headerLength);

    uartUSB.write(LOG_MESSAGE_PAYLOAD_INFO, LOG_MESSAGE_PAYLOAD_INFO_LEN);
    printData(this->jwt->payload, this->jwt->payloadLength);

    uartUSB.write(LOG_MESSAGE_SIGNATURE_INFO, LOG_MESSAGE_SIGNATURE_INFO_LEN);
    printData(this->jwt->signature, this->jwt->signatureLength);

    strncpy(payloadRetrieved, this->jwt->payload, this->jwt->payloadLength);
    payloadRetrieved[this->jwt->payloadLength] = '\0';
    this->jwt->clear();

    return true;
}