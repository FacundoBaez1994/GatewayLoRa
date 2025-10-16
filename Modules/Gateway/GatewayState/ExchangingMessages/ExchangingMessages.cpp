//=====[Libraries]=============================================================
#include "ExchangingMessages.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "FormattingMessage.h"
#include "SensingBatteryStatus.h"

//=====[Declaration of private defines]========================================
#define MAXATTEMPTS 20
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ExchangingMessages::ExchangingMessages (Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->currentStatus = gatewayStatus;
}

ExchangingMessages::ExchangingMessages (Gateway * gateway) {
    this->gateway = gateway;
}

ExchangingMessages::~ExchangingMessages () {
    this->gateway = nullptr;
}

void ExchangingMessages::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void ExchangingMessages::exchangeMessages (CellularModule * cellularTransceiver,
    char * message,  RemoteServerInformation* serverTargetted, char * receivedMessage ){
    static CellularTransceiverStatus_t currentTransmitionStatus;
    static bool newDataAvailable = false;
    static bool enableTransceiver = false;
    char logMessage [100];
    char payloadRetrieved [256];
    
    if (enableTransceiver == false) {
        cellularTransceiver->enableTransceiver();
        enableTransceiver = true; 
    }
    currentTransmitionStatus = cellularTransceiver->exchangeMessages (message, serverTargetted,
    receivedMessage, &newDataAvailable);

    if (currentTransmitionStatus == CELLULAR_TRANSCEIVER_STATUS_SEND_OK) {
        snprintf(logMessage, sizeof(logMessage), "The message was send with success");
        uartUSB.write (logMessage , strlen (logMessage));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}

    if (newDataAvailable == true) {

        //////////////////   MESSAGE INTERPRETATION ////////////////
        ////////////////////////////////////////////////////////////////
        if (this->gateway->decodeJWT(receivedMessage, payloadRetrieved) == false) {
            snprintf(logMessage, sizeof(logMessage), "error decoding JWT\r\n");
            uartUSB.write(logMessage, strlen(logMessage));

            newDataAvailable = false;
            enableTransceiver = false;
            this->gateway->changeState (new SensingBatteryStatus (this->gateway));
            return;
        }
        strcpy (receivedMessage, payloadRetrieved);

        newDataAvailable = false;
        enableTransceiver = false;

        char success[30];
        char latency[30];
        char mode[30];
        char timeSilentString [30];
        int timeSilent;

        if (extractField(receivedMessage, "\"SUCS\"", success, sizeof(success)) == false) {
            snprintf(logMessage, sizeof(logMessage), "Corrupted Server Message\r\n");
            uartUSB.write(logMessage, strlen(logMessage));

            newDataAvailable = false;
            enableTransceiver = false;
            // new state formatting Message in order to be saved in memory
            this->gateway->changeState (new SensingBatteryStatus (this->gateway));
            return;
        }
        this->gateway->progressOnHashChain ();
        this->gateway->increaseSequenceNumber ();

        snprintf(logMessage, sizeof(logMessage), "SUCCESS: %s\r\n", success);
        uartUSB.write(logMessage, strlen(logMessage));

        if (strcmp (success, "true") != 0 && strcmp (success, "TRUE") != 0) {
            snprintf(logMessage, sizeof(logMessage), "Server returns error\r\n");
            uartUSB.write(logMessage, strlen(logMessage));

            newDataAvailable = false;
            enableTransceiver = false;
            // new state formatting Message in order to be saved in memory
            this->gateway->changeState (new SensingBatteryStatus (this->gateway));
            return;
        }

        if (extractField(receivedMessage, "\"LTCY\"", latency, sizeof(latency)) == false) {    
            newDataAvailable = false;
            enableTransceiver = false;
            this->gateway->changeState (new SensingBatteryStatus (this->gateway));
            return;
        }
        snprintf(logMessage, sizeof(logMessage), "LATENCY: %s\r\n", latency);
        uartUSB.write(logMessage, strlen(logMessage));
        LatencyLevel_t newLatencyLevel;
        if (this->parseLatencyLevel(latency, &newLatencyLevel)) {
            //this->gateway->setLatency(newLatencyLevel);
        }
        

        if (extractField(receivedMessage, "\"MODE\"", mode, sizeof(mode)) == false) {
            newDataAvailable = false;
            enableTransceiver = false;
            this->gateway->changeState (new SensingBatteryStatus (this->gateway));
            return;
        }
        OperationMode_t newOperationMode;
        int hoursSilentMode;
        if (this->parseOperationMode(mode, &newOperationMode)) {
            if (newOperationMode == SILENT_OPERATION_MODE) {
                if (extractField(receivedMessage, "\"TSOP\"", timeSilentString, sizeof(timeSilentString)) == false) {
                    newDataAvailable = false;
                    enableTransceiver = false;
                    this->gateway->changeState (new SensingBatteryStatus (this->gateway));
                    return;
                }
                timeSilent = std::atoi(timeSilentString);
                if ( timeSilent < 0) {
                    newDataAvailable = false;
                    enableTransceiver = false;
                    this->gateway->changeState (new SensingBatteryStatus (this->gateway));
                    return;
                }
                snprintf(logMessage, sizeof(logMessage), "Setted Silent mode of %i hours\r\n", timeSilent );
                uartUSB.write(logMessage, strlen(logMessage));
                //this->gateway->setOperationMode(newOperationMode);
                //this->gateway->setSilentTimer(timeSilent);
            } else {
                //this->gateway->setOperationMode(newOperationMode);
            }

        }

        snprintf(logMessage, sizeof(logMessage), "MODE: %s\r\n", mode);
        uartUSB.write(logMessage, strlen(logMessage));
        

        newDataAvailable = false;
        enableTransceiver = false;
        this->gateway->changeState (new SensingBatteryStatus (this->gateway));
        return;
        //////////////////   MESSAGE INTERPRETATION ////////////////
        ////////////////////////////////////////////////////////////////
        
        } else {
            snprintf(logMessage, sizeof(logMessage),"No Messages received:");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            newDataAvailable = false;
            enableTransceiver = false;
            this->gateway->changeState (new SensingBatteryStatus (this->gateway));
            return;
        }    
    }  else if (currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND
    && currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {

        snprintf(logMessage, sizeof(logMessage),"The message couldn't be sent");
        uartUSB.write (logMessage , strlen (logMessage));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        newDataAvailable = false;
        enableTransceiver = false;
        this->gateway->changeState (new SensingBatteryStatus (this->gateway));
        return;
    }
    return;
}
 
//=====[Implementations of private methods]==================================


bool ExchangingMessages::extractField(const char* json, const char* key, char* output, size_t maxLen) {
    const char* found = strstr(json, key);
    if (!found) return false;

    found = strchr(found, ':');
    if (!found) return false;

    found++; // avanzar al valor
    while (*found == ' ' || *found == '\"') found++; // salteamos espacios o comillas

    size_t i = 0;
    while (*found && *found != '\"' && *found != ',' && i < maxLen - 1) {
        output[i++] = *found++;
    }
    output[i] = '\0';
    return true;
}

bool ExchangingMessages::parseLatencyLevel(const char* latencyStr, LatencyLevel_t * newLatencyLevel) {
    if (strcmp(latencyStr, "ELL") == 0) {
        uartUSB.write("\r\nLatency level: EXTREMELY_LOW_LATENCY\r\n", strlen("\r\nLatency level: EXTREMELY_LOW_LATENCY\r\n"));
        *newLatencyLevel = EXTREMELY_LOW_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "VLL") == 0) {
         uartUSB.write("\r\nLatency level: VERY_LOW_LATENCY\r\n", strlen("\r\nLatency level: VERY_LOW_LATENCY\r\n"));
        *newLatencyLevel = VERY_LOW_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "LL") == 0) {
        uartUSB.write("\r\nLatency level: LOW_LATENCY\r\n", strlen("\r\nLatency level: LOW_LATENCY\r\n"));
        *newLatencyLevel = LOW_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "ML") == 0) {
         uartUSB.write("\r\nLatency level: MEDIUM_LATENCY\r\n", strlen("\r\nLatency level: MEDIUM_LATENCY\r\n"));
        *newLatencyLevel = MEDIUM_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "HL") == 0) {
        uartUSB.write("\r\nLatency level: HIGH_LATENCY\r\n", strlen("\r\nLatency level: MEDIUM_LATENCY\r\n"));
        *newLatencyLevel = HIGH_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "VHL") == 0) {
        uartUSB.write("\r\nLatency level: VERY_HIGH_LATENCY\r\n", strlen("\r\nLatency level: VERY_HIGH_LATENCY\r\n"));
        *newLatencyLevel = VERY_HIGH_LATENCY;
        return true;
    } 
    if (strcmp(latencyStr, "EHL") == 0) {
        uartUSB.write("\r\nLatency level: EXTREMELY_HIGH_LATENCY\r\n", strlen("\r\nLatency level: EXTREMELY_HIGH_LATENCY\r\n"));
        *newLatencyLevel = EXTREMELY_HIGH_LATENCY;
        return true;
    } 
    return false;
}


bool ExchangingMessages::parseOperationMode(const char* operationModeStr, OperationMode_t * newOperationMode) {
    if (strcmp(operationModeStr, "NOPM") == 0) {
         uartUSB.write("\r\nOperation Mode: NORMAL_OPERATION_MODE\r\n", strlen("\r\nOperation Mode: NORMAL_OPERATION_MODE\r\n"));
        *newOperationMode = NORMAL_OPERATION_MODE;
        return true;
    } 
    if (strcmp(operationModeStr, "POPM") == 0) {
         uartUSB.write("\r\nOperation Mode: PURSUIT_OPERATION_MODE\r\n", strlen("\r\nOperation Mode: PURSUIT_OPERATION_MODE\r\n"));
        *newOperationMode = PURSUIT_OPERATION_MODE;
        return true;
    } 
    if (strcmp(operationModeStr, "SOPM") == 0) {
         uartUSB.write("\r\nOperation Mode: SILENT_OPERATION_MODE\r\n", strlen("\r\nOperation Mode: SILENT_OPERATION_MODE\r\n"));
        *newOperationMode = SILENT_OPERATION_MODE;
        return true;
    } 
    return false;
}
