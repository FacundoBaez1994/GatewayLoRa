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


void FormattingMessage::formatMessage (char * formattedMessage, const CellInformation* aCellInfo,
    const GNSSData* GNSSInfo, const std::vector<CellInformation*> &neighborsCellInformation,
    const IMUData_t * imuData, const std::vector<IMUData_t*> &IMUDataSamples, const BatteryData  * batteryStatus) {
    char StringToSendUSB [50];
    char gatewayEvent [20];

    this->gateway->getMovementEvent(gatewayEvent);

    switch (this->currentStatus ) {
        ///////////// MN Modem Messages //////////////////////////////
        case GATEWAY_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formatting MN,GNSS message\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, GNSSInfo, imuData, batteryStatus, gatewayEvent);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway, this->currentStatus));
            break;

        case GATEWAY_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formatting MN,MN message:\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, 
            neighborsCellInformation, imuData, batteryStatus, gatewayEvent);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway, this->currentStatus));
            break;

        case GATEWAY_STATUS_GNSS_LOADED_MESSAGE:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formatting GNSS message (reloaded):\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo->IMEI, GNSSInfo, imuData, batteryStatus, gatewayEvent);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway, this->currentStatus));
            break;

        case GATEWAY_STATUS_IMU_LOADED_MESSAGE:
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "Formatting IMU message (reloaded)\r\n");
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo->IMEI, imuData, IMUDataSamples, batteryStatus, gatewayEvent);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway, this->currentStatus));
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
void FormattingMessage::formatMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t * imuData,
     const BatteryData  * batteryStatus, char * gatewayEvent) {

    static char tempBuffer[250]; 
    size_t currentLen = 0;

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer,
        "\"Type\":\"MNMN\","
        "\"IMEI\":%lld,"
        "\"EVNT\":\"%s\","
        "\"MCC\":%d,"
        "\"MNC\":%d,"
        "\"LAC\":\"%X\","
        "\"CID\":\"%X\","
        "\"SLVL\":%.2f,"
        "\"TECH\":%d,"
        "\"REGS\":%d,"
        "\"CHNL\":%d,"
        "\"BAND\":\"%s\","
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
        aCellInfo->IMEI,               // 1
        gatewayEvent,                  // 2
        aCellInfo->mcc,                // 2
        aCellInfo->mnc,                 // 3
        aCellInfo->lac,               // 4
        aCellInfo->cellId,            // 5
        aCellInfo->signalLevel,       // 6
        aCellInfo->accessTechnology,  // 7
        aCellInfo->registrationStatus,// 8
        aCellInfo->channel,           // 9
        aCellInfo->band,              // 10
        aCellInfo->timestamp,              // 11
        batteryStatus->batteryChargeStatus, // 12
        batteryStatus->chargeLevel,          // 13
        imuData->status,                //14
        imuData->acceleration.ax,       // 15
        imuData->acceleration.ay,       //16
        imuData->acceleration.az,       //17
        imuData->angles.yaw,            // 18
        imuData->angles.roll,           // 19
        imuData->angles.pitch           // 20
    );

    if (!neighborsCellInformation.empty()) {
        currentLen += snprintf(this->messageBuffer + currentLen, this->sizeOfMessageBuffer - currentLen, ",\"Neighbors\":[");
        
        for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
            CellInformation* neighbor = neighborsCellInformation[i];
            snprintf(tempBuffer, sizeof(tempBuffer),
                "{\"TECH\":%d,\"MCC\":%d,\"MNC\":%d,\"LAC\":\"%X\",\"CID\":\"%X\",\"SLVL\":%.2f}",
                neighbor->tech,
                neighbor->mcc,
                neighbor->mnc,
                neighbor->lac,
                neighbor->cellId,
                neighbor->signalLevel
            );
            strncat(this->messageBuffer, tempBuffer, this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);

            // Si no es el último, agregamos coma
            if (i < neighborsCellInformation.size() - 1) {
                strncat(this->messageBuffer, ",", this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);
            }

            //delete neighborsCellInformation[i];
            //neighborsCellInformation[i] = nullptr;
        }
        //neighborsCellInformation.clear();
        strncat(this->messageBuffer, "]", this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);
    }

   this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';

    this->addMetaData(this->messageBuffer);

    this->jwt->encodeJWT (this->messageBuffer, formattedMessage);

     strcat(formattedMessage, "\n");
}

void FormattingMessage::formatMessage(char * formattedMessage, const CellInformation* aCellInfo,
 const GNSSData* GNSSInfo,  const IMUData_t * imuData,
  const BatteryData  * batteryStatus, char * gatewayEvent) {

    size_t currentLen = 0;

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
        "\"Type\":\"MNGNSS\","
        "\"IMEI\":%lld,"
        "\"EVNT\":\"%s\","
        "\"LAT\":%.6f,"
        "\"LONG\":%.6f,"
        "\"HDOP\":%.2f,"
        "\"ALT\":%.2f,"
        "\"COG\":%.2f,"
        "\"SPD\":%.2f,"
        "\"MNC\":%d,"
        "\"MCC\":%d,"
        "\"LAC\":\"%X\","
        "\"CID\":\"%X\","
        "\"SLVL\":%.2f,"
        "\"TECH\":%d,"
        "\"REGS\":%d,"
        "\"CHNL\":%d,"
        "\"BAND\":\"%s\","
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
        aCellInfo->IMEI,               // 1
        gatewayEvent,                  // 2
        GNSSInfo->latitude,            // 3
        GNSSInfo->longitude,           // 4
        GNSSInfo->hdop,                // 5
        GNSSInfo->altitude,            // 6
        GNSSInfo->cog,                 // 7
        GNSSInfo->spkm,                // 8
        aCellInfo->mnc,                // 9
        aCellInfo->mcc,                // 10
        aCellInfo->lac,                // 11
        aCellInfo->cellId,             // 12
        aCellInfo->signalLevel,        // 13
        aCellInfo->accessTechnology,   // 14
        aCellInfo->registrationStatus, // 15
        aCellInfo->channel,            // 16
        aCellInfo->band,               // 17
        GNSSInfo->timestamp,                // 18
        batteryStatus->batteryChargeStatus, // 20
        batteryStatus->chargeLevel,          // 21
        imuData->status,                // 22
        imuData->acceleration.ax,       // 23
        imuData->acceleration.ay,    // 24
        imuData->acceleration.az,        // 25
        imuData->angles.yaw,             // 26
        imuData->angles.roll,        // 27
        imuData->angles.pitch        // 28
    );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';

    this->addMetaData(this->messageBuffer);
    //strcpy(formattedMessage, message);
    this->jwt->encodeJWT (this->messageBuffer, formattedMessage);

    strcat(formattedMessage, "\n");

}




void FormattingMessage::formatMessage(char * formattedMessage, long long int IMEI,
 const GNSSData* GNSSInfo,  const IMUData_t * imuData, 
 const BatteryData  * batteryStatus , char * gatewayEvent) {

    size_t currentLen = 0;

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer,
        "\"Type\":\"GNSS\","
        "\"IMEI\":%lld,"
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
        IMEI,                // 0
        GNSSInfo->latitude,            // 1
        GNSSInfo->longitude,           // 2
        GNSSInfo->hdop,                // 3
        GNSSInfo->altitude,            // 4
        GNSSInfo->cog,                 // 5
        GNSSInfo->spkm,                // 6
        GNSSInfo->timestamp,                // 7
        batteryStatus->batteryChargeStatus, // 8
        batteryStatus->chargeLevel,          // 9
        imuData->status,                     // 10
        imuData->acceleration.ax,            // 11
        imuData->acceleration.ay,            // 12
        imuData->acceleration.az,            // 13
        imuData->angles.yaw,                 // 14
        imuData->angles.roll,                // 15
        imuData->angles.pitch                // 16
    );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';

    //strcpy(formattedMessage, message);
    this->addMetaData(this->messageBuffer);

    this->jwt->encodeJWT (this->messageBuffer, formattedMessage);

    strcat(formattedMessage, "\n");

}



void FormattingMessage::formatMessage(char * formattedMessage, long long int IMEI, 
    const IMUData_t * inertialData,  const std::vector<IMUData_t*> &IMUDataSamples, 
    const BatteryData  * batteryStatus, char * gatewayEvent) {

    static char tempBuffer[250];
    size_t currentLen = 0;

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer,
        "\"Type\":\"IMU\","
        "\"IMEI\":%lld,"
        "\"EVNT\":\"%s\","        
        "\"TIME\":\"%s\","
        "\"TBWS\":%d,"
        "\"BSTA\":%d,"
        "\"BLVL\":%d,"
        "\"SIMU\":%d,"
        "\"AX\":%.2f,"
        "\"AY\":%.2f,"
        "\"AZ\":%.2f,"
        "\"YAW\":%.2f,"
        "\"ROLL\":%.2f,"
        "\"PTCH\":%.2f",
        IMEI,               // 1
        gatewayEvent,       // 2 %s
        inertialData->timestamp, // 3 %s
        inertialData->timeBetweenSamples, // 4 %d
        batteryStatus->batteryChargeStatus, // 5 %d
        batteryStatus->chargeLevel,          // 6 %d
        inertialData->status,                // 7 %d
        inertialData->acceleration.ax,       // 8 %.2f
        inertialData->acceleration.ay,       // 9 %.2f
        inertialData->acceleration.az,       // 10 %.2f
        inertialData->angles.yaw,            // 11 %.2f
        inertialData->angles.roll,           // 12 %.2f
        inertialData->angles.pitch           // 13 %.2f
    );

    if (!IMUDataSamples.empty()) {
        currentLen += snprintf(this->messageBuffer + currentLen, this->sizeOfMessageBuffer - currentLen, ",\"Samples\":[");
        
        for (size_t i = 0; i < IMUDataSamples.size(); ++i) {
            IMUData_t* sample = IMUDataSamples[i];
            snprintf(tempBuffer, sizeof(tempBuffer),
                "{\"AX\":%.2f,\"AY\":%.2f,\"AZ\":%.2f,\"YAW\":\"%.2f\",\"ROLL\":\"%.2f\",\"PTCH\":%.2f}",
                sample->acceleration.ax,       // 6 %.2f
                sample->acceleration.ay,       // 7 %.2f
                sample->acceleration.az,       // 8 %.2f
                sample->angles.yaw,            // 9 %.2f
                sample->angles.roll,           // 10 %.2f
                sample->angles.pitch           // 11 %.2f
            );
            strncat(this->messageBuffer, tempBuffer, this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);

            if (i < IMUDataSamples.size() - 1) {
                strncat(this->messageBuffer, ",", this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);
            }


        }
        strncat(this->messageBuffer, "]", this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);
    }

   // strncat(message, "}\n", sizeof(message) - strlen(message) - 1);

    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';

    this->addMetaData(this->messageBuffer);
    //strcpy(formattedMessage, message);
    this->jwt->encodeJWT (this->messageBuffer, formattedMessage);


    strcat(formattedMessage, "\n");
}

/////////////////////////// LoRa Messages //////////////////////////////
void FormattingMessage::formatLoRaMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const GNSSData* GNSSInfo, const IMUData_t * imuData,
    const BatteryData  * batteryStatus, char * gatewayEvent) {

  int messageNumber = this->gateway->getLoraMessageNumber ();
    snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
    "LORAGNSS,%lld,%d,%s,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%s,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
        aCellInfo->IMEI,                    // 1 %lld 
        messageNumber,                      // 2 %d  
        gatewayEvent,                        // 2 %s  
        GNSSInfo->latitude,                 // 3 %.6f
        GNSSInfo->longitude,                // 4 %.6f
        GNSSInfo->hdop,                     // 5 %.2f
        GNSSInfo->altitude,                 // 6 %.2f
        GNSSInfo->cog,                      // 7 %.2f
        GNSSInfo->spkm,                     // 8 %.2f
        GNSSInfo->timestamp,                // 9 %s
        batteryStatus->batteryChargeStatus, // 10 %d
        batteryStatus->chargeLevel,         // 11 %d
        imuData->status,                    // 12 %d
        imuData->acceleration.ax,           // 13 %.2f
        imuData->acceleration.ay,           // 14 %.2f
        imuData->acceleration.az,           // 15 %.2f
        imuData->angles.yaw,                // 16 %.2f
        imuData->angles.roll,               // 17 %.2f
        imuData->angles.pitch               // 18 %.2f
            );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';       

    uartUSB.write ("plaintext message:\r\n", strlen ("plaintext message:\r\n"));  // debug only
    uartUSB.write ( this->messageBuffer, strlen ( this->messageBuffer));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->gateway->prepareLoRaMessage ( this->messageBuffer, strlen (this->messageBuffer)) == false) {
        return;
    }

    size_t originalLength = strlen(this->messageBuffer);
    this->messageBuffer[originalLength ] = '|';  // add '||' to indicate the end of the full message 
    this->messageBuffer[originalLength + 1] = '|';     
    this->messageBuffer[originalLength + 2] = '\0';      // Asegurar terminación nula

    strcpy (formattedMessage, this->messageBuffer);
}


void FormattingMessage::formatLoRaMessage (char * formattedMessage, const CellInformation* aCellInfo, 
 const IMUData_t * imuData, const BatteryData  * batteryStatus, char * gatewayEvent) {

    int messageNumber = this->gateway->getLoraMessageNumber (); 
    snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
    "LORALORA,%lld,%d,%s,%lld,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
        aCellInfo->IMEI,                     // 1 %lld
        messageNumber,                      // 2 %d  
        gatewayEvent,                       // 2 %s
        batteryStatus->batteryChargeStatus, // 3 %d
        batteryStatus->chargeLevel,         // 4 %d
        imuData->status,                    // 5 %d
        imuData->acceleration.ax,           // 6 %.2f
        imuData->acceleration.ay,           // 7 %.2f
        imuData->acceleration.az,           // 8 %.2f
        imuData->angles.yaw,                // 9 %.2f
        imuData->angles.roll,               // 10 %.2f
        imuData->angles.pitch               // 11 %.2f
            );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';       

    uartUSB.write ("plaintext message:\r\n", strlen ("plaintext message:\r\n"));  // debug only
    uartUSB.write ( this->messageBuffer, strlen ( this->messageBuffer));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    if (this->gateway->prepareLoRaMessage ( this->messageBuffer, strlen (this->messageBuffer)) == false) {
        return;
    }

    size_t originalLength = strlen(this->messageBuffer);
    this->messageBuffer[originalLength ] = '|';  // add '||' to indicate the end of the full message 
    this->messageBuffer[originalLength + 1] = '|';     
    this->messageBuffer[originalLength + 2] = '\0';      // Asegurar terminación nula

    strcpy (formattedMessage, this->messageBuffer);
}






/////////////////////////// saving cases //////////////////////////////
//// MNMN for save on memory
void FormattingMessage::formatMNMNMemoryMessage(char * formattedMessage, const CellInformation* aCellInfo, 
    const std::vector<CellInformation*> &neighborsCellInformation, const IMUData_t * imuData,
     const BatteryData  * batteryStatus, char * gatewayEvent) {

    static char tempBuffer[250];
    size_t currentLen = 0;


    char StringToSendUSB [100];
    
    snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "MN,MN save:\r\n");
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only}

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer,
        "MNMN,%s,%d,%d,%X,%X,%.2f,%d,%d,%d,%s,%s,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
        gatewayEvent,                   // 1 %s
        aCellInfo->mcc,               // 2 %d
        aCellInfo->mnc,               // 3 %d
        aCellInfo->lac,               // 4 %X
        aCellInfo->cellId,            // 5 %X
        aCellInfo->signalLevel,       // 6 %.2f
        aCellInfo->accessTechnology,  // 7 %d
        aCellInfo->registrationStatus,// 8 %d
        aCellInfo->channel,           // 9 %d
        aCellInfo->band,              // 10 %s
        aCellInfo->timestamp,              // 11 %s
        batteryStatus->batteryChargeStatus, // 12 %d
        batteryStatus->chargeLevel,          // 13 %d
        imuData->status,                //14 %d
        imuData->acceleration.ax,       // 15 %.2f
        imuData->acceleration.ay,       //16 %.2f
        imuData->acceleration.az,       //17 %.2f
        imuData->angles.yaw,            // 18 %.2f
        imuData->angles.roll,           // 19 %.2f
        imuData->angles.pitch           // 20 %.2f
    );
    // inertialData,  //13 status, 13 ax, 14 ay, 15 az, 16 yaw, 17 roll, 18 pitch

    if (!neighborsCellInformation.empty()) {
        for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
            CellInformation* neighbor = neighborsCellInformation[i];
            snprintf(tempBuffer, sizeof(tempBuffer),
                "|%d,%d,%d,%X,%X,%.2f",
                neighbor->tech,
                neighbor->mcc,
                neighbor->mnc,
                neighbor->lac,
                neighbor->cellId,
                neighbor->signalLevel
            );
            strncat(this->messageBuffer, tempBuffer, this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);

            uartUSB.write ( tempBuffer,  strlen (tempBuffer) );  // debug only}
        }
    }
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';
    strcpy(formattedMessage, this->messageBuffer);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}

//// MNGNSS for save on memory
void FormattingMessage::formatMemoryMessage(char * formattedMessage, const CellInformation* aCellInfo,
 const GNSSData* GNSSInfo,  const IMUData_t * imuData, const BatteryData  * batteryStatus, char * gatewayEvent) {

    snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
    "MNGNSS,%s,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%d,%d,%X,%X,%.2f,%d,%d,%d,%s,%s,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
        gatewayEvent,                       // 1 %s
        GNSSInfo->latitude,                 // 2 %.6f
        GNSSInfo->longitude,                // 3 %.6f
        GNSSInfo->hdop,                     // 4 %.2f
        GNSSInfo->altitude,                 // 5 %.2f
        GNSSInfo->cog,                      // 6 %.2f
        GNSSInfo->spkm,                     // 7 %.2f
        aCellInfo->mnc,                     // 8 %d
        aCellInfo->mcc,                     // 9 %d
        aCellInfo->lac,                     // 10 %X
        aCellInfo->cellId,                  // 11 %X
        aCellInfo->signalLevel,             // 12 %.2f
        aCellInfo->accessTechnology,        // 13 %d
        aCellInfo->registrationStatus,      // 14 %d
        aCellInfo->channel,                 // 15 %d
        aCellInfo->band,                    // 16 %s
        GNSSInfo->timestamp,                // 17 %s
        batteryStatus->batteryChargeStatus, // 18 %d
        batteryStatus->chargeLevel,         // 19 %d
        imuData->status,                    // 20 %d
        imuData->acceleration.ax,           // 21 %.2f
        imuData->acceleration.ay,           // 22 %.2f
        imuData->acceleration.az,           // 23 %.2f
        imuData->angles.yaw,                // 24 %.2f
        imuData->angles.roll,               // 25 %.2f
        imuData->angles.pitch               // 26 %.2f
            );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';       
    strcpy(formattedMessage, this->messageBuffer);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}

//// GNSS for save on memory
void FormattingMessage::formatGNSSMemoryMessage(char * formattedMessage, const GNSSData* GNSSInfo, 
 const IMUData_t * imuData, const BatteryData  * batteryStatus, char * gatewayEvent) {

    snprintf(this->messageBuffer, this->sizeOfMessageBuffer, 
    "GNSS,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f", 
        gatewayEvent,                       // 1 %s
        GNSSInfo->latitude,                 // 2 %.6f
        GNSSInfo->longitude,                // 3 %.6f
        GNSSInfo->hdop,                     // 4 %.2f
        GNSSInfo->altitude,                 // 5 %.2f
        GNSSInfo->cog,                      // 6 %.2f
        GNSSInfo->spkm,                     // 7 %.2f
        GNSSInfo->timestamp,                // 8 %s
        batteryStatus->batteryChargeStatus, // 9 %d
        batteryStatus->chargeLevel,         // 10 %d
        imuData->status,                    // 11 %d
        imuData->acceleration.ax,           // 12 %.2f
        imuData->acceleration.ay,           // 13 %.2f
        imuData->acceleration.az,           // 14 %.2f
        imuData->angles.yaw,                // 15 %.2f
        imuData->angles.roll,               // 16 %.2f
        imuData->angles.pitch               // 17 %.2f
            );
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';       
    strcpy(formattedMessage, this->messageBuffer);
    //formattedMessage[sizeof(formattedMessage) - 1] = '\0';
}

//// IMU for save on memory
void FormattingMessage::formatMemoryMessage(char * formattedMessage, const IMUData_t * imuData,
const std::vector<IMUData_t*> &IMUDataSamples, const BatteryData  * batteryStatus, char * gatewayEvent) {

    static char tempBuffer[250]; 
    size_t currentLen = 0;


    char StringToSendUSB [100];
    
    snprintf(StringToSendUSB, sizeof(StringToSendUSB),  "IMU save:\r\n");
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only}

    currentLen = snprintf(this->messageBuffer, this->sizeOfMessageBuffer,
        "IMU,%s,%s,%d,%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
        gatewayEvent,       // 1 %s
        imuData->timestamp, // 2 %s
        imuData->timeBetweenSamples, // 3 %d
        batteryStatus->batteryChargeStatus, // 4 %d
        batteryStatus->chargeLevel,          // 5 %d
        imuData->status,                // 6 %d
        imuData->acceleration.ax,       // 7 %.2f
        imuData->acceleration.ay,       // 8 %.2f
        imuData->acceleration.az,       // 9 %.2f
        imuData->angles.yaw,            // 10 %.2f
        imuData->angles.roll,           // 11 %.2f
        imuData->angles.pitch           // 12 %.2f
    );

    if (!IMUDataSamples.empty()) {
        for (size_t i = 0; i < IMUDataSamples.size(); ++i) {
            IMUData_t* sample = IMUDataSamples[i];
            snprintf(tempBuffer, sizeof(tempBuffer),
                "|%.2f,%.2f,%.2f,%.2f,%.2f,%.2f",
                sample->acceleration.ax,
                sample->acceleration.ay,
                sample->acceleration.az,
                sample->angles.yaw,
                sample->angles.roll,
                sample->angles.pitch
            );
            strncat(this->messageBuffer, tempBuffer, this->sizeOfMessageBuffer - strlen(this->messageBuffer) - 1);

            uartUSB.write ( tempBuffer,  strlen (tempBuffer) );  // debug only}
        }
    }
    this->messageBuffer[this->sizeOfMessageBuffer - 1] = '\0';
    strcpy(formattedMessage, this->messageBuffer);
}