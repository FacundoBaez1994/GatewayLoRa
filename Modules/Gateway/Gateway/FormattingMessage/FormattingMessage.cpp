//=====[Libraries]=============================================================

#include "FormattingMessage.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "ExchangingMessages.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================


/** 
* @brief
* 
* @param 
*/
FormattingMessage::FormattingMessage (Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->currentStatus = gatewayStatus;
}

/** 
* @brief
* 
* @param 
*/
FormattingMessage::~FormattingMessage () {
    this->gateway = NULL;
}

void FormattingMessage::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void FormattingMessage::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void FormattingMessage::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void FormattingMessage::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}


void FormattingMessage::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
     BatteryData  * batteryStatus) {
    /// Cifrado iria aca tambien..

    if (this->currentStatus == GATEWAY_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
            char StringToSendUSB [50] = "Formating MN,GNSS message\r\n";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, GNSSInfo, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway, 
            GATEWAY_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK));
            return;
    }
    if (this->currentStatus == GATEWAY_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK) {
            char StringToSendUSB [50] = "Formating MN,MN message:\r\n";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->formatMessage(formattedMessage, aCellInfo, 
            neighborsCellInformation, batteryStatus);
            uartUSB.write (formattedMessage , strlen (formattedMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            snprintf(StringToSendUSB, sizeof(StringToSendUSB),"Switching State to ExchangingMessages"); 
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            this->gateway->changeState (new ExchangingMessages (this->gateway, 
            GATEWAY_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK));
            return;
        }
        
    return;
}

void FormattingMessage::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    return;
}

void FormattingMessage::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void FormattingMessage::awake (CellularModule * cellularTransceiver,
 NonBlockingDelay * latency ) {
    return;
}


//=====[Implementations of private methods]==================================
void FormattingMessage::formatMessage(char * formattedMessage, CellInformation* aCellInfo, 
    std::vector<CellInformation*> &neighborsCellInformation, BatteryData  * batteryStatus) {

    static char message[2024];
    static char tempBuffer[250]; // buffer auxiliar para formateo
    size_t currentLen = 0;

    // Encabezado principal del mensaje JSON con los datos de la celda principal
    currentLen = snprintf(message, sizeof(message),
        "{\"Type\":\"MNMN\","
        "\"MCC\":%d,"
        "\"MNC\":%d,"
        "\"LAC\":\"%X\","
        "\"CID\":\"%X\","
        "\"SLVL\":%.2f,"
        "\"TECH\":%d,"
        "\"REGS\":%d,"
        "\"CHNL\":%d,"
        "\"BAND\":\"%s\","
        "\"DATE\":\"%s\","
        "\"TIME\":\"%s\","
        "\"BSTA\":%d,"
        "\"BLVL\":%d",
        aCellInfo->mcc,               // 1
        aCellInfo->mnc,               // 2
        aCellInfo->lac,               // 3
        aCellInfo->cellId,            // 4
        aCellInfo->signalLevel,       // 5
        aCellInfo->accessTechnology,  // 6
        aCellInfo->registrationStatus,// 7
        aCellInfo->channel,           // 8
        aCellInfo->band,              // 9
        aCellInfo->date,              // 10
        aCellInfo->time,              // 11
        batteryStatus->batteryChargeStatus, // 12
        batteryStatus->chargeLevel          // 13
    );
    // inertialData,  //12 temp, 13 ax, 14 ay, 15 az, 16 yaw, 17 roll, 18 pitch

    // Agregar array de celdas vecinas si existen
    if (!neighborsCellInformation.empty()) {
        currentLen += snprintf(message + currentLen, sizeof(message) - currentLen, ",\"Neighbors\":[");
        
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
            strncat(message, tempBuffer, sizeof(message) - strlen(message) - 1);

            // Si no es el último, agregamos coma
            if (i < neighborsCellInformation.size() - 1) {
                strncat(message, ",", sizeof(message) - strlen(message) - 1);
            }

            //delete neighborsCellInformation[i];
            //neighborsCellInformation[i] = NULL;
        }
        //neighborsCellInformation.clear();
        strncat(message, "]", sizeof(message) - strlen(message) - 1);
    }

    // Cierre del JSON
    strncat(message, "}", sizeof(message) - strlen(message) - 1);

    message[sizeof(message) - 1] = '\0';

    //strcpy(formattedMessage, message);
    this->gateway->encodeJWT (message, formattedMessage);
}


void FormattingMessage::formatMessage(char * formattedMessage, CellInformation* aCellInfo,
 GNSSData* GNSSInfo, BatteryData  * batteryStatus) {

    static char message[512];
    size_t currentLen = 0;

    currentLen = snprintf(message, sizeof(message),
        "{"
        "\"Type\":\"MNGNSS\","
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
        "\"DATE\":\"%s\","
        "\"TIME\":\"%s\","
        "\"BSTA\":%d,"
        "\"BLVL\":%d"
        "}",
        GNSSInfo->latitude,            // 1
        GNSSInfo->longitude,           // 2
        GNSSInfo->hdop,                // 3
        GNSSInfo->altitude,            // 4
        GNSSInfo->cog,                 // 5
        GNSSInfo->spkm,                // 6
        aCellInfo->mnc,                // 7
        aCellInfo->mcc,                // 8
        aCellInfo->lac,                // 9
        aCellInfo->cellId,             // 10
        aCellInfo->signalLevel,        // 11
        aCellInfo->accessTechnology,   // 12
        aCellInfo->registrationStatus, // 13
        aCellInfo->channel,            // 14
        aCellInfo->band,               // 15
        GNSSInfo->date,                // 16
        GNSSInfo->utc,                 // 17
        batteryStatus->batteryChargeStatus, // 19
        batteryStatus->chargeLevel          // 20
    );
    message[sizeof(message) - 1] = '\0';

    //strcpy(formattedMessage, message);
    this->gateway->encodeJWT (message, formattedMessage);

}
