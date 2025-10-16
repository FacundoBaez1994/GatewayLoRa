//=====[Libraries]=============================================================
#include "ConnectingEthernet.h"
#include "Gateway.h" // debido a declaración adelantada
#include "Debugger.h" // due to global usbUart
#include "GatewayStatus.h"
#include "WaitingForMessage.h"
#include "EthernetInterface.h"
#include "ObtaingTimeUTCThroughEthernet.h"
#include "ConnectingToMobileNetwork.h"

//=====[Declaration of private defines]========================================
#define TIMEOUT_MS_ETHERNET_CONNECTION      80000
#define MAX_ETHERNET_RETRIES                20
#define ETHERNET_CONNECT_TIMEOUT_S          60
#define LOG_BUFFER_SIZE                     128

#define LOG_MSG_ETH_CONNECTING              "Connecting Ethernet Module to Local network\n"
#define LOG_MSG_ETH_NOT_AVAILABLE           "Ethernet connection not available\n"
#define LOG_MSG_ETH_IP                      "IP address: %s\n"
#define LOG_MSG_ETH_NETMASK                 "Netmask: %s\n"
#define LOG_MSG_ETH_GATEWAY                 "Gateway: %s\n"
#define LOG_MSG_CHANGE_TO_TIME_STATE        "Changing To ObtaingTimeUTCThroughEthernet State\r\n"

//=====[Declaration and initialization of private global variables]============
// (ninguna por ahora)

//=====[Implementations of public methods]===================================

ConnectingEthernet::ConnectingEthernet(Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->connectionRetries = 0;
    this->currentStatus = gatewayStatus;
}

ConnectingEthernet::~ConnectingEthernet() {
    this->gateway = NULL;
}

void ConnectingEthernet::connectEthernetToLocalNetwork(UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    const time_t TIMEOUT = 5;
    char logMessage[LOG_BUFFER_SIZE];

    Watchdog &watchdog = Watchdog::get_instance();
    watchdog.kick();

    snprintf(logMessage, sizeof(logMessage), LOG_MSG_ETH_CONNECTING);
    uartUSB.write(logMessage, strlen(logMessage));

    // Intentar conectar
    if (ethernetModule->connect(ETHERNET_CONNECT_TIMEOUT_S) != 0) {
        snprintf(logMessage, sizeof(logMessage), LOG_MSG_ETH_NOT_AVAILABLE);
        uartUSB.write(logMessage, strlen(logMessage));

        if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_ETHERNET) {
            this->currentStatus = GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_MOBILE_NETWORK;
            this->gateway->changeState(new ConnectingToMobileNetwork(this->gateway, this->currentStatus));
            return;
        }
        if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_ETHERNET) {
            this->currentStatus = GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_MOBILE_NETWORK;
            this->gateway->changeState(new ConnectingToMobileNetwork(this->gateway, this->currentStatus));
            return;
        }
        return;
    }

    watchdog.kick();

    // Mostrar parámetros de red
    const char* ip = ethernetModule->get_ip_address();
    const char* netmask = ethernetModule->get_netmask();
    const char* gateway = ethernetModule->get_gateway();

    snprintf(logMessage, sizeof(logMessage), LOG_MSG_ETH_IP, ip ? ip : "None");
    uartUSB.write(logMessage, strlen(logMessage));

    snprintf(logMessage, sizeof(logMessage), LOG_MSG_ETH_NETMASK, netmask ? netmask : "None");
    uartUSB.write(logMessage, strlen(logMessage));

    snprintf(logMessage, sizeof(logMessage), LOG_MSG_ETH_GATEWAY, gateway ? gateway : "None");
    uartUSB.write(logMessage, strlen(logMessage));

    uartUSB.write(LOG_MSG_CHANGE_TO_TIME_STATE, strlen(LOG_MSG_CHANGE_TO_TIME_STATE));
    this->gateway->changeState(new ObtaingTimeUTCThroughEthernet(this->gateway, this->currentStatus));
}


//=====[Implementations of private functions]==================================
