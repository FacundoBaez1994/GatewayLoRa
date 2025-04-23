//=====[Libraries]=============================================================
#include "SendingMessageEthernet.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingForMessage.h"
#include "SendingAck.h"
#include "Slepping.h"

//=====[Declaration of private defines]========================================
#define MAX_RETRIES 3
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
SendingMessageEthernet::SendingMessageEthernet (Gateway * gateway, int IdDevice, int messageNumber, char * payload) {
    this->gateway = gateway;
    this->IdDevice = IdDevice;
    this->messageNumber = messageNumber;
    this->connectionRetries = 0;

    if (payload != nullptr) {
        strncpy(this->payload, payload, sizeof(this->payload) - 1); // Copiar hasta 49 caracteres
        this->payload[sizeof(this->payload) - 1] = '\0';            // Asegurar terminación nula
    } else {
        this->payload[0] = '\0'; // Si payload es nullptr, dejar vacío
    }
}


/** 
* @brief
* 
* @param 
*/
SendingMessageEthernet::~SendingMessageEthernet() {
     this->gateway = NULL;
}



void SendingMessageEthernet::receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay) {
    return;
}

void SendingMessageEthernet::sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay) {

    return;
}


void SendingMessageEthernet::sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    const time_t    TIMEOUT = 5;    // Connection timeout time
    time_t          timeOut;
    //char            data[] = "GET / HTTP/1.1\r\nHost: ifconfig.io\r\nConnection: close\r\n\r\n";
    char*           remaining;
    uint8_t*        recvBuf;
    int             result;
    char logMessage [251];
    char message [251];
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.kick();

    snprintf(logMessage, sizeof(logMessage), "Sending TCP Message through Ethernet\n");
     uartUSB.write(logMessage, strlen(logMessage)); // Debug

    //net.set_network(IP, NETMASK, GATEWAY);  // include this to use static IP address
    if (ethernetModule->connect(15) != 0) {
        snprintf(logMessage, sizeof(logMessage), "Ethernet connection not available\n");
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        return;
    }

    watchdog.kick();


    // Show the network address
    const char*     ip = ethernetModule->get_ip_address();
    const char*     netmask = ethernetModule->get_netmask();
    const char*     gateway = ethernetModule->get_gateway();

    snprintf(logMessage, sizeof(logMessage), "IP address: %s\n", ip ? ip : "None");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug
    snprintf(logMessage, sizeof(logMessage), "Netmask: %s\n", netmask ? netmask : "None");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug
    snprintf(logMessage, sizeof(logMessage), "Gateway: %s\n", gateway ? gateway : "None");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug

    // Open a socket on the network interface, and create a TCP connection to ifconfig.io
    TcpClient socket;

    result = socket.open(ethernetModule);
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.open() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        this->gateway->changeState (new Slepping (this->gateway));
        return;
        // change state
    }

    timeOut = time(NULL) + TIMEOUT;
    //printf("Connecting to the 'ifconfig.io' server ...\r\n");
    snprintf(logMessage, sizeof(logMessage), "Connecting to the TCP server ...\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug


    watchdog.kick();
    result = socket.connect("186.19.62.251", 123); // modificar ip y puerto
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.connect() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        this->connectionRetries ++;

        if (this->connectionRetries >= MAX_RETRIES) {
            this->disconnect (ethernetModule, &socket);
            snprintf(logMessage, sizeof(logMessage), "Ethernet not available, sending through MN\r\n");
            uartUSB.write(logMessage, strlen(logMessage));
            this->gateway->changeState (new Slepping(this->gateway));
            return;
        }
        return;
    }

    snprintf(logMessage, sizeof(logMessage), "Server connected.\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug
    snprintf(logMessage, sizeof(logMessage), "Sending data to server:\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug

    snprintf(message, sizeof(message), "%d,%s\r\n", this->IdDevice, this->payload);
    remaining = message;
    result = strlen(remaining);
    while (result) {
        result = socket.send((uint8_t*)remaining, strlen(remaining));
        if (result < 0) {
            snprintf(logMessage, sizeof(logMessage), "Error! socket.send() returned: %d\n", result);
            uartUSB.write(logMessage, strlen(logMessage)); // Debug
            this->connectionRetries ++;
            if (this->connectionRetries >= MAX_RETRIES) {
                this->disconnect (ethernetModule, &socket);
                snprintf(logMessage, sizeof(logMessage), "Ethernet not available, sending through MN\r\n");
                uartUSB.write(logMessage, strlen(logMessage));
                this->gateway->changeState (new Slepping(this->gateway));  
            }
            return;
        }
        snprintf(logMessage, sizeof(logMessage), "%.*s", result, remaining);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        remaining += result;
    }

    snprintf(logMessage, sizeof(logMessage), "Waiting for data from server:\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug
    while (socket.available() == 0) {
        if (time(NULL) > timeOut) {
            snprintf(logMessage, sizeof(logMessage),"Connection time out.\r\n");
            uartUSB.write(logMessage, strlen(logMessage)); // Debug
            this->disconnect (ethernetModule, &socket);
            snprintf(logMessage, sizeof(logMessage), "OK Waiting For another Message\r\n");
            uartUSB.write(logMessage, strlen(logMessage));
            this->gateway->changeState (new WaitingForMessage(this->gateway));  
            return;
        }
    }

    snprintf(logMessage, sizeof(logMessage),"Data received:\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug
    while ((result = socket.available()) > 0) {
        recvBuf = (uint8_t*)malloc(result);
        result = socket.recv(recvBuf, result);
        if (result < 0) {
            snprintf(logMessage, sizeof(logMessage),"Error! socket.recv() returned: %d\n", result);
            uartUSB.write(logMessage, strlen(logMessage)); // Debug
            this->disconnect (ethernetModule, &socket);
            snprintf(logMessage, sizeof(logMessage), "OK Waiting For another Message\r\n");
            uartUSB.write(logMessage, strlen(logMessage));
            this->gateway->changeState (new WaitingForMessage(this->gateway));  
            return;
        }
        snprintf(logMessage, sizeof(logMessage),"%.*s\r\n", result, recvBuf);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        free(recvBuf);
    }
    uartUSB.write("\r\n", strlen("\r\n")); // Debug
}



void SendingMessageEthernet::updatePowerStatus (CellularModule * cellularTransceiver,
 BatteryData * currentBatteryStatus) {
    cellularTransceiver->startStopUpdate();
 }

void SendingMessageEthernet::obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata) {
    return;
}

 void SendingMessageEthernet::connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation) {
    return; 
}


void SendingMessageEthernet::obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors ) {
    return;
}


void SendingMessageEthernet::formatMessage (char * formattedMessage, CellInformation* aCellInfo,
    GNSSData* GNSSInfo, std::vector<CellInformation*> &neighborsCellInformation,
    BatteryData  * batteryStatus) {
    return;
}

void SendingMessageEthernet::exchangeMessages (CellularModule * cellularTransceiver,
    char * message, TcpSocket * socketTargetted, char * receivedMessage ){

    return;
}
    // agregar LoRa // exchageMessages (Lora * LoRaModule);
void SendingMessageEthernet::goToSleep (CellularModule * cellularTransceiver ) {
    return;
}

void SendingMessageEthernet::awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency ) {
    return;
 }

//=====[Implementations of private functions]==================================
void SendingMessageEthernet::disconnect (UipEthernet * ethernetModule, TcpClient * socket) {
    uartUSB.write("disconnecting\r\n", strlen("disconnecting\r\n"));
    socket->close();
    ethernetModule->disconnect();
}
