//=====[Libraries]=============================================================

#include "ExchangingMessages.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "GoingToSleep.h"

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
ExchangingMessages::ExchangingMessages (Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->currentStatus = gatewayStatus;
}

/** 
* @brief
* 
* @param 
*/
ExchangingMessages::~ExchangingMessages () {
    this->gateway = NULL;
}

void ExchangingMessages::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void ExchangingMessages::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void ExchangingMessages::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void ExchangingMessages::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}

void ExchangingMessages::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
    return;
}

void ExchangingMessages::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){
    static CellularTransceiverStatus_t currentTransmitionStatus;
    static bool newDataAvailable = false;
    static bool enableTransceiver = false;
    static bool rebootTransceiver = false;
    char logMessage [50];
    
    if (this->currentStatus == GATEWAY_STATUS_GNSS_UNAVAILABLE_CONNECTED_TO_MOBILE_NETWORK
     || this->currentStatus == GATEWAY_STATUS_GNSS_OBTAIN_CONNECTED_TO_MOBILE_NETWORK) {
        if (enableTransceiver == false) {
            cellularTransceiver->enableTransceiver();
            enableTransceiver = true; 
        }
       currentTransmitionStatus = cellularTransceiver->exchangeMessages (message, socketTargetted,
       receivedMessage, &newDataAvailable);

        if (currentTransmitionStatus == CELLULAR_TRANSCEIVER_STATUS_SEND_OK) {
            snprintf(logMessage, sizeof(logMessage), "The message was send with success");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}

        if (newDataAvailable == true) {

            snprintf(logMessage, sizeof(logMessage),"new Message received:");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}

            snprintf(logMessage, sizeof(logMessage), "%s",  receivedMessage);
            uartUSB.write (logMessage , strlen (logMessage ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            newDataAvailable = false;
            enableTransceiver = false;
            this->gateway->changeState (new GoingToSleep (this->gateway));
            return;
            } else {
                snprintf(logMessage, sizeof(logMessage),"No Messages received:");
                uartUSB.write (logMessage , strlen (logMessage));  // debug only
                uartUSB.write ( "\r\n",  3 );  // debug only}
                newDataAvailable = false;
                enableTransceiver = false;
                this->gateway->changeState (new GoingToSleep (this->gateway));
                return;
            }    
        }  else if (currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND
        && currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {

            snprintf(logMessage, sizeof(logMessage),"The message couldn't be sent");
            uartUSB.write (logMessage , strlen (logMessage));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only}
            rebootTransceiver = true;
        }
    } else {
        // try with LoRa
    }

    if (rebootTransceiver == true) {
        if (cellularTransceiver->reboot()) {
            // save message in memory..
            newDataAvailable = false;
            enableTransceiver = false;
            rebootTransceiver = false; 
            this->gateway->changeState (new GoingToSleep (this->gateway));
            return;
        }
    }
    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void ExchangingMessages::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void ExchangingMessages::awake (CellularModule * cellularTransceiver,
 NonBlockingDelay * latency ) {
    return;
}
//=====[Implementations of private methods]==================================