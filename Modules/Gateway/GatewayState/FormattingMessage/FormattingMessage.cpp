//=====[Libraries]=============================================================
#include "FormattingMessage.h"
#include "Gateway.h" 
#include "Debugger.h" // due to global usbUart
#include "ExchangingMessages.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================
bool hash_and_base64(const char *input, char *output, size_t output_size) {
    if (!input || !output) return false;

    unsigned char hash[32]; // SHA-256 -> 32 bytes
    size_t olen = 0;

    // 1. Calcular SHA-256
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0); // 0 = SHA-256, 1 = SHA-224
    mbedtls_sha256_update_ret(&ctx, (const unsigned char*)input, strlen(input));
    mbedtls_sha256_finish_ret(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    // 2. Codificar en Base64
    if (mbedtls_base64_encode((unsigned char*)output, output_size, &olen,
                              hash, sizeof(hash)) != 0) {
        return false;
    }

    // Asegurar terminación de string
    if (olen < output_size) {
        output[olen] = '\0';
    } else {
        return false;
    }

    return true;
}


//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
FormattingMessage::FormattingMessage (Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->currentStatus = gatewayStatus;
    this->jwt = new JWTManager ();
    this->sizeOfMessageBuffer = 2248;
    this->messageBuffer = new char [this->sizeOfMessageBuffer];
}

FormattingMessage::FormattingMessage (Gateway * gateway) {
    this->gateway = gateway;
    this->jwt = new JWTManager ();
    this->sizeOfMessageBuffer = 2248;
    this->messageBuffer = new char [this->sizeOfMessageBuffer];
}

FormattingMessage::~FormattingMessage () {
    this->gateway = nullptr;
    delete this->jwt;
    this->jwt = nullptr;

    delete [] this->messageBuffer;
    this->messageBuffer = nullptr;

}

void FormattingMessage::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }


void FormattingMessage::formatMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo, GNSSData* receptedGNSSdata,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus) {
    char StringToSendUSB [50];
    char trackerEvent [20];

    this->gateway->getMovementEvent(trackerEvent);

    ReceptedTypeMessage_t ReceptedTypeMessage = this->gateway->getReceptedTypeMessage ();

    switch (ReceptedTypeMessage ) {
        ///////////// MN Modem Messages //////////////////////////////
        case LORALORA:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formatting LORALORA message\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatLORALORAMessage (formattedMessage, IMEIRecepted, receptedTrackerEvent, RSSI,
            gatewayGNSSInfo, receptedImuData, receptedBatteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway));
            break;

        case LORAGNSS:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formatting LORAGNSS message:\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatLORAGNSSMessage (formattedMessage, IMEIRecepted, receptedTrackerEvent, RSSI, 
            receptedGNSSdata, receptedImuData, receptedBatteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway));
            break;
        default:
            return;
    }

 
    return;
}




//=====[Implementations of private methods]==================================
void FormattingMessage::addMetaData(char *messageToAddMetaData) {
    int sizeOfBuffer = this->sizeOfMessageBuffer;
    int sizeOfTimeStamp = 20;
    int sizeOfHash = 100;
    char * workBuffer;   // buffer auxiliar único y reutilizable
    workBuffer = new char [sizeOfBuffer];
    char * hashCanonicData;
    hashCanonicData = new char [sizeOfHash];
    char * hashCurrentJson;
    hashCurrentJson = new char [sizeOfHash];
    char * urlPathChannel;
    urlPathChannel = new char [sizeOfHash];
    char * deviceIdentifier;
    deviceIdentifier = new char [sizeOfHash];
    char * hashPrevJson;
    hashPrevJson = new char [sizeOfHash];
    char * timestampJson = new char [sizeOfTimeStamp];
    char * timestampJsonExpiration = new char [sizeOfTimeStamp];



    int currentSequenceNumber = this->gateway->getSequenceNumber();
    this->gateway->getUrlPathChannel(urlPathChannel);
    this->gateway->getDeviceIdentifier(deviceIdentifier);
    this->gateway->getPrevHashChain(hashPrevJson);

    // Calcular hash del payload base
    hash_and_base64(messageToAddMetaData, hashCanonicData, sizeOfHash);

    // Timestamp actual y de expiración
    time_t seconds = time(NULL);

    epochToTimestamp(seconds, timestampJson, sizeOfTimeStamp);

    time_t secondsToExpire = seconds + 24 * 60 * 60; // +24h
    epochToTimestamp(secondsToExpire, timestampJsonExpiration, sizeOfTimeStamp);

    // Armar JSON intermedio en workBuffer
    int written = snprintf(workBuffer, sizeOfBuffer,
        "{\"iss\":\"%s\","
        "\"aud\":\"%s\","
        "\"ias\":\"%s\","
        "\"exp\":\"%s\","
        "\"d\":\"%s\","
        "\"seq\":%d,"
        "\"prev\":\"%s\","
        "%s}",
        deviceIdentifier,        // iss
        urlPathChannel,          // aud
        timestampJson,           // ias
        timestampJsonExpiration, // exp
        hashCanonicData,         // d
        currentSequenceNumber,   // seq
        hashPrevJson,            // prev
        messageToAddMetaData                  // payload original
    );

    if (written < 0 || (size_t)written >= sizeOfBuffer) {
        workBuffer[sizeOfBuffer - 1] = '\0'; // protección
    }

    // Calcular hash del JSON completo
    hash_and_base64(workBuffer, hashCurrentJson, sizeOfHash);

    // Quitar llaves externas { }
    size_t len = strlen(workBuffer);
    if (len > 2 && workBuffer[0] == '{' && workBuffer[len - 1] == '}') {
        workBuffer[len - 1] = '\0';                 // saco '}'
        memmove(workBuffer, workBuffer + 1, len-1); // corro eliminando '{'
    }

    // JSON final se escribe directo en message
    written = snprintf(messageToAddMetaData, sizeOfBuffer,
        "{\"curr\":\"%s\",%s}",
        hashCurrentJson,
        workBuffer
    );

    if (written < 0 || (size_t)written >= sizeOfBuffer) {
        messageToAddMetaData[sizeOfBuffer - 1] = '\0'; // protección
    }

    this->gateway->setCurrentHashChain(hashCurrentJson);
    
    delete [] workBuffer;
    workBuffer = nullptr;

    delete [] hashCanonicData;
    hashCanonicData = nullptr;

    delete [] urlPathChannel;
    urlPathChannel = nullptr;

    delete [] hashCurrentJson;
    hashCurrentJson = nullptr;

    delete [] deviceIdentifier;
    deviceIdentifier = nullptr;

    delete [] hashPrevJson;
    hashPrevJson = nullptr;
    
    delete [] timestampJson;
    timestampJson = nullptr;

    delete [] timestampJsonExpiration;
    timestampJsonExpiration = nullptr;
}

//////////////////////// MN messages ///
void FormattingMessage::formatLORALORAMessage (char * formattedMessage, long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* gatewayGNSSInfo,
    const IMUData_t * receptedImuData, const BatteryData  * receptedBatteryStatus) {

    size_t currentLen = 0;

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
        "\"Type\":\"LORALORA\","
        "\"IMEI\":%lld,"
        "\"RSSI\":%d,"
        "\"EVNT\":\"%s\","
        "\"LAT\":%.6f,"
        "\"LONG\":%.6f,"
        "\"ALT\":%.2f,"
        "\"TIME\":\"%s\","
        "\"BSTA\":%d,"
        "\"BLVL\":%d,"
        "\"SIMU\":%d,"
        "\"AX\":%.2f,"
        "\"AY\":%.2f,"
        "\"AZ\":%.2f,"
        "\"YAW\":%.2f,"
        "\"ROLL\":%.2f,"
        "\"PTCH\":%.2f",
        IMEIRecepted,               // 1 "\"IMEI\":%lld,"
        RSSI,                       // 2  "\"RSSI\":%.2f,"
        receptedTrackerEvent,       // 3   "\"EVNT\":\"%s\","
        gatewayGNSSInfo->latitude,  // 4 "\"LAT\":%.6f,"       
        gatewayGNSSInfo->longitude, // 5 "\"LONG\":%.6f,
        gatewayGNSSInfo->altitude,  // 6 "\"ALT\":%.2f,"
        receptedImuData->timestamp, // 7 "\"TIME\":\"%s\","
        receptedBatteryStatus->batteryChargeStatus, // 8 "\"BSTA\":%d,"
        receptedBatteryStatus->chargeLevel,  // 9 "\"BLVL\":%d,"
        receptedImuData->status,             // 10 "\"SIMU\":%d,"
        receptedImuData->acceleration.ax,    // 11 "\"AX\":%.2f,"
        receptedImuData->acceleration.ay,    // 12  "\"AY\":%.2f,"
        receptedImuData->acceleration.az,    // 13 "\"AZ\":%.2f,"
        receptedImuData->angles.yaw,         // 14 "\"YAW\":%.2f,"
        receptedImuData->angles.roll,        // 15 "\"ROLL\":%.2f,"
        receptedImuData->angles.pitch        // 16 "\"PTCH\":%.2f"
    );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';

    this->addMetaData(this->messageBuffer);
    //strcpy(formattedMessage, message);
    this->jwt->encodeJWT (this->messageBuffer, formattedMessage);

    strcat(formattedMessage, "\n");

}

void FormattingMessage::formatLORAGNSSMessage (char * formattedMessage, const long long int IMEIRecepted,
    const char * receptedTrackerEvent, const int RSSI, const GNSSData* receptedGNSSdata, const IMUData_t * receptedImuData, 
    const BatteryData  * receptedBatteryStatus) {

    size_t currentLen = 0;

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
        "\"Type\":\"LORAGNSS\","
        "\"IMEI\":%lld,"
        "\"RSSI\":%d,"
        "\"EVNT\":\"%s\","
        "\"LAT\":%.6f,"
        "\"LONG\":%.6f,"
        "\"HDOP\":%.2f,"
        "\"ALT\":%.2f,"
        "\"COG\":%.2f,"
        "\"SPD\":%.2f,"
        "\"TIME\":\"%s\","
        "\"BSTA\":%d,"
        "\"BLVL\":%d,"
        "\"SIMU\":%d,"
        "\"AX\":%.2f,"
        "\"AY\":%.2f,"
        "\"AZ\":%.2f,"
        "\"YAW\":%.2f,"
        "\"ROLL\":%.2f,"
        "\"PTCH\":%.2f",
        IMEIRecepted,               // 1 "\"IMEI\":%lld,"
        RSSI,                        // 2 "\"RSSI\":%.2f,"
        receptedTrackerEvent,        // 3 "\"EVNT\":\"%s\","      
        receptedGNSSdata->latitude,  // 4  "\"LAT\":%.6f,"
        receptedGNSSdata->longitude, // 5 "\"LONG\":%.6f,"
        receptedGNSSdata->hdop,      // 6 "\"HDOP\":%.2f,"
        receptedGNSSdata->altitude,  // 7 "\"ALT\":%.2f,"
        receptedGNSSdata->cog,       // 8 "\"COG\":%.2f,"
        receptedGNSSdata->spkm,      // 9 "\"SPD\":%.2f,"
        receptedGNSSdata->timestamp, // 10 "\"TIME\":\"%s\","
        receptedBatteryStatus->batteryChargeStatus, // 11 "\"BSTA\":%d,"
        receptedBatteryStatus->chargeLevel,         // 12 "\"BLVL\":%d,"
        receptedImuData->status,                    // 13 "\"SIMU\":%d,"
        receptedImuData->acceleration.ax,           // 14 "\"AX\":%.2f,"
        receptedImuData->acceleration.ay,           // 15 "\"AY\":%.2f,"
        receptedImuData->acceleration.az,           // 16 "\"AZ\":%.2f,"
        receptedImuData->angles.yaw,                // 17 "\"YAW\":%.2f,"
        receptedImuData->angles.roll,               // 18 "\"ROLL\":%.2f,"
        receptedImuData->angles.pitch               // 19 "\"PTCH\":%.2f",
    );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';

    this->addMetaData(this->messageBuffer);
    //strcpy(formattedMessage, message);
    this->jwt->encodeJWT (this->messageBuffer, formattedMessage);

    strcat(formattedMessage, "\n");

}




