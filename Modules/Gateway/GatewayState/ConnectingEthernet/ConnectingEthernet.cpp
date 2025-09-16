//=====[Libraries]=============================================================
#include "ConnectingEthernet.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingForMessage.h"
#include "EthernetInterface.h"   // la interfaz de red cableada
#include "ObtaingTimeUTCThroughEthernet.h"


//=====[Declaration of private defines]========================================
#define TIMEOUT_MS 80000
#define MAX_RETRIES 20
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
ConnectingEthernet::ConnectingEthernet (Gateway * gateway) {
    this->gateway = gateway;
    this->connectionRetries = 0;

}


/** 
* @brief
* 
* @param 
*/
ConnectingEthernet::~ConnectingEthernet() {
     this->gateway = NULL;
}



void ConnectingEthernet::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void ConnectingEthernet::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void ConnectingEthernet::queryUTCTimeViaRemoteServer (UipEthernet * ethernetModule, NonBlockingDelay * delay){
        return;
}

void ConnectingEthernet::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}


void ConnectingEthernet::connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
     
    const time_t    TIMEOUT = 5;    // Connection timeout time
    time_t          timeOut;
    char*           remaining;
    uint8_t*        recvBuf;
    int             result;
    char logMessage [1024];


    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();

    snprintf(logMessage, sizeof(logMessage), "Connecting Ethernet Module to Local network\n");
    uartUSB.write(logMessage, strlen(logMessage));

    // Conexión de red
    if (ethernetModule->connect(15) != 0) {
        snprintf(logMessage, sizeof(logMessage), "Ethernet connection not available\n");
        uartUSB.write(logMessage, strlen(logMessage));
        return;
    }

    watchdog.kick();

    // Mostrar parámetros de red
    const char* ip = ethernetModule->get_ip_address();
    const char* netmask = ethernetModule->get_netmask();
    const char* gateway = ethernetModule->get_gateway();

    snprintf(logMessage, sizeof(logMessage), "IP address: %s\n", ip ? ip : "None");
    uartUSB.write(logMessage, strlen(logMessage));
    snprintf(logMessage, sizeof(logMessage), "Netmask: %s\n", netmask ? netmask : "None");
    uartUSB.write(logMessage, strlen(logMessage));
    snprintf(logMessage, sizeof(logMessage), "Gateway: %s\n", gateway ? gateway : "None");
    uartUSB.write(logMessage, strlen(logMessage));

    uartUSB.write("Changing To ObtaingTimeUTCThroughEthernet State\r\n", strlen("Changing To ObtaingTimeUTCThroughEthernet State\r\n"));
    this->gateway->changeState (new ObtaingTimeUTCThroughEthernet(this->gateway));
}

//=====[Implementations of private functions]==================================
