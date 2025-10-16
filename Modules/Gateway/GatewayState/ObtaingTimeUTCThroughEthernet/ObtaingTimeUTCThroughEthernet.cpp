//=====[Libraries]=============================================================
#include "ObtaingTimeUTCThroughEthernet.h"
#include "Gateway.h"
#include "Debugger.h" // due to global usbUart
#include "EthernetInterface.h"  
#include "SendingMessageThroughEthernet.h"
#include "FormattingMessage.h"

//=====[Declaration of private defines]========================================
#define TIMEOUT_MS_ETHERNET_UTC         80000
#define MAX_ETHERNET_UTC_RETRIES        20
#define NTP_SERVER_ADDRESS              "time.nist.gov"
#define NTP_SERVER_PORT                 37
#define NTP_EPOCH_OFFSET                2208988800UL  // 1900-1970
#define TIME_BUFFER_LEN                 128 

#define LOG_MSG_ERROR_SENDING_DATA      "Error sending data\r\n"
#define LOG_MSG_TIME_RECEIVED           "Time received = %lu seconds since 1/01/1970 00:00 GMT\r\n"
#define LOG_MSG_TIME_HUMAN              "Time = %s\r"
#define LOG_MSG_TIME_SERVER_ADDRESS     "Time Server Address: %s\r\n"
#define LOG_MSG_TIME_SERVER_PORT        "Time Server Port: %d\r\n"
#define LOG_MSG_FORMATTING_MESSAGE      "FormattingMessage\r\n"

//=====[Declaration of private data types]=====================================
typedef struct {
    uint32_t secs;  // Transmit Time-stamp seconds
} ntp_packet;

//=====[Implementations of public methods]===================================

ObtaingTimeUTCThroughEthernet::ObtaingTimeUTCThroughEthernet(Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->connectionRetries = 0;
    this->currentStatus = gatewayStatus;
}

ObtaingTimeUTCThroughEthernet::~ObtaingTimeUTCThroughEthernet() {
    this->gateway = nullptr;
}

void ObtaingTimeUTCThroughEthernet::queryUTCTimeViaRemoteServer(UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    UdpSocket socket(ethernetModule);
    SocketAddress sockAddr;
    char out_buffer[] = "time";

    if (socket.sendto(NTP_SERVER_ADDRESS, NTP_SERVER_PORT, out_buffer, sizeof(out_buffer)) < 0) {
        uartUSB.write(LOG_MSG_ERROR_SENDING_DATA, strlen(LOG_MSG_ERROR_SENDING_DATA));
        this->gateway->changeState(new FormattingMessage(this->gateway, this->currentStatus));
        return;
    }
    
    ntp_packet in_data;
    socket.recvfrom(&sockAddr, &in_data, sizeof(ntp_packet));
    in_data.secs = ntohl(in_data.secs) - NTP_EPOCH_OFFSET;

    char timeBuffer[TIME_BUFFER_LEN];
    snprintf(timeBuffer, sizeof(timeBuffer), LOG_MSG_TIME_RECEIVED, (uint32_t)in_data.secs);
    uartUSB.write(timeBuffer, strlen(timeBuffer));

    set_time(in_data.secs);

    snprintf(timeBuffer, sizeof(timeBuffer), LOG_MSG_TIME_HUMAN, ctime((const time_t*)&in_data.secs));
    uartUSB.write(timeBuffer, strlen(timeBuffer));

    snprintf(timeBuffer, sizeof(timeBuffer), LOG_MSG_TIME_SERVER_ADDRESS, sockAddr.get_ip_address());
    uartUSB.write(timeBuffer, strlen(timeBuffer));

    snprintf(timeBuffer, sizeof(timeBuffer), LOG_MSG_TIME_SERVER_PORT, sockAddr.get_port());
    uartUSB.write(timeBuffer, strlen(timeBuffer));

    socket.close();
    uartUSB.write(LOG_MSG_FORMATTING_MESSAGE, strlen(LOG_MSG_FORMATTING_MESSAGE));
    this->gateway->changeState(new FormattingMessage(this->gateway, this->currentStatus));
}
