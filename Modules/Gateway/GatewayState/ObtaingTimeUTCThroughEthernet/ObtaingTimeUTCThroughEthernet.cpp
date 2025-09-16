//=====[Libraries]=============================================================
#include "ObtaingTimeUTCThroughEthernet.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingForMessage.h"
#include "EthernetInterface.h"   // la interfaz de red cableada
#include "SendingMessageThroughEthernet.h"


//=====[Declaration of private defines]========================================
#define TIMEOUT_MS 80000
#define MAX_RETRIES 20

//=====[Declaration of private data types]=====================================
typedef struct
{
    uint32_t    secs;                           // Transmit Time-stamp seconds.
} ntp_packet;
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
ObtaingTimeUTCThroughEthernet::ObtaingTimeUTCThroughEthernet (Gateway * gateway) {
    this->gateway = gateway;
    this->connectionRetries = 0;

}


/** 
* @brief
* 
* @param 
*/
ObtaingTimeUTCThroughEthernet::~ObtaingTimeUTCThroughEthernet() {
     this->gateway = NULL;
}



void ObtaingTimeUTCThroughEthernet::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void ObtaingTimeUTCThroughEthernet::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void ObtaingTimeUTCThroughEthernet::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
        return;
}

void ObtaingTimeUTCThroughEthernet::connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    return;
}

void ObtaingTimeUTCThroughEthernet::queryUTCTimeViaRemoteServer(UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    // Bring up the ethernet interface
    UdpSocket       socket(ethernetModule);
    SocketAddress   sockAddr;
    char            out_buffer[] = "time";
    char            logMessage[256];   // buffer para logs
    
    if (socket.sendto("time.nist.gov", 37, out_buffer, sizeof(out_buffer)) < 0) {
        sprintf(logMessage, "Error sending data\r\n");
        uartUSB.write(logMessage, strlen(logMessage));
        return;
    }
    
    ntp_packet  in_data;
    socket.recvfrom(&sockAddr, &in_data, sizeof(ntp_packet));
    in_data.secs = ntohl(in_data.secs) - 2208988800;    // 1900-1970
    
    sprintf(logMessage, "Time received = %lu seconds since 1/01/1970 00:00 GMT\r\n", (uint32_t)in_data.secs);
    uartUSB.write(logMessage, strlen(logMessage));
    
    sprintf(logMessage, "Time = %s\r", ctime((const time_t*) &in_data.secs));
    uartUSB.write(logMessage, strlen(logMessage));
    
    sprintf(logMessage, "Time Server Address: %s\r\n", sockAddr.get_ip_address());
    uartUSB.write(logMessage, strlen(logMessage));
    
    sprintf(logMessage, "Time Server Port: %d\r\n", sockAddr.get_port());
    uartUSB.write(logMessage, strlen(logMessage));
    
    // Close the socket and bring down the network interface
    socket.close();
    //net.disconnect();

    
    uartUSB.write("SendingMessageThroughEthernet\r\n", strlen("SendingMessageThroughEthernet\r\n"));
    this->gateway->changeState (new SendingMessageThroughEthernet(this->gateway));

    return;
}

//=====[Implementations of private functions]==================================
