//=====[Libraries]=============================================================
#include "SendingMessageThroughEthernet.h"
#include "Gateway.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart
#include "WaitingForMessage.h"
#include "EthernetInterface.h"   // la interfaz de red cableada
#include "SensingBatteryStatus.h"
#include "ConnectingToMobileNetwork.h"

//=====[Declaration of private defines]========================================
#define TIMEOUT_MS 80000
#define MAX_RETRIES 20

#define ETHERNET_QUERY_TIMEOUT_S          5
#define LOG_BUFFER_SIZE                   512
#define HTTP_REQUEST_BUFFER_SIZE          2500

#define NGROK_HOSTNAME                    "intent-lion-loudly.ngrok-free.app"
#define NGROK_PORT                        80
#define NGROK_PATH                       "/apendice/canal-secundario/envio"    
#define WEBHOOK_HOSTNAME                  "webhook.site"
#define WEBHOOK_PORT                      80
#define WEBHOOK_PATH                      "/17a904ed-4df1-4d29-95ae-9bdfa70ad9bb" // VARIA CON EL TIEMPO

#define HTTP_CONTENT_TYPE_TEXT_PLAIN      "Content-Type: text/plain\r\n"
#define HTTP_CONNECTION_CLOSE             "Connection: close\r\n"
#define CRLF                              "\r\n"

#define LOG_MSG_SOCKET_OPEN_ERROR         "Error! socket.open() returned: %d\n"
#define LOG_MSG_SOCKET_CONNECT_ERROR      "Error! socket.connect() returned: %d\n"
#define LOG_MSG_SOCKET_SEND_ERROR         "Error! socket.send() returned: %d\n"
#define LOG_MSG_SOCKET_RECV_ERROR         "Error! socket.recv() returned: %d\n"
#define LOG_MSG_CONNECTING_TO_SERVER      "Connecting to ngrok server ...\r\n"
#define LOG_MSG_SERVER_CONNECTED          "Server connected.\r\n"
#define LOG_MSG_HTTP_POST_SENT            "HTTP POST sent, waiting for response...\r\n"
#define LOG_MSG_CONNECTION_TIMEOUT        "Connection time out.\r\n"
#define LOG_MSG_DISCONNECTING             "disconnecting\r\n"
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
SendingMessageThroughEthernet::SendingMessageThroughEthernet (Gateway * gateway, gatewayStatus_t gatewayStatus) {
    this->gateway = gateway;
    this->connectionRetries = 0;
    this->currentStatus = gatewayStatus;
}

SendingMessageThroughEthernet::~SendingMessageThroughEthernet() {
     this->gateway = NULL;
}



// envio a wirehook
void SendingMessageThroughEthernet::exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay,
char * payload) {
    const time_t    TIMEOUT = ETHERNET_QUERY_TIMEOUT_S;    // Connection timeout time
    time_t          timeOut;
    char*           remaining;
    uint8_t*        recvBuf;
    int             result;
    char logMessage [LOG_BUFFER_SIZE];


    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
   
    TcpClient socket;
   
    result = socket.open(ethernetModule);
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), LOG_MSG_SOCKET_OPEN_ERROR, result);
        uartUSB.write(logMessage, strlen(logMessage));
        if (this->currentStatus ==  GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_ETHERNET ) {
            this->currentStatus = GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_MOBILE_NETWORK;
            this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
            return;
        }
        if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_ETHERNET) {
            this->currentStatus = GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_MOBILE_NETWORK;
            this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
            return;
        }
        return;
    }

    timeOut = time(NULL) + TIMEOUT;
    snprintf(logMessage, sizeof(logMessage), LOG_MSG_CONNECTING_TO_SERVER);
    uartUSB.write(logMessage, strlen(logMessage));

    watchdog.kick();
  //result = socket.connect(NGROK_HOSTNAME, NGROK_PORT); // HTTP plano
    result = socket.connect(WEBHOOK_HOSTNAME, WEBHOOK_PORT); // HTTP plano  //  LocalHost 5133
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), LOG_MSG_SOCKET_CONNECT_ERROR, result);
        uartUSB.write(logMessage, strlen(logMessage));
        this->connectionRetries ++;
        if (this->connectionRetries >= MAX_RETRIES) {
            this->disconnect (ethernetModule, &socket);
            if (this->currentStatus ==  GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_ETHERNET ) {
                this->currentStatus = GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_MOBILE_NETWORK;
                this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
                return;
            }
            if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_ETHERNET) {
                this->currentStatus = GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_MOBILE_NETWORK;
                this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
                return;
            }
                return;
            }
        return;
    }

    snprintf(logMessage, sizeof(logMessage), LOG_MSG_SERVER_CONNECTED);
    uartUSB.write(logMessage, strlen(logMessage));


    char httpRequest[HTTP_REQUEST_BUFFER_SIZE];
    snprintf(httpRequest, sizeof(httpRequest),
      //  "POST NGROK_PATH  HTTP/1.1\r\n"
      //  "Host: NGROK_HOSTNAME \r\n" 
        "POST WEBHOOK_PATH HTTP/1.1\r\n" // VARIA CON EL TIEMPO
        "Host: WEBHOOK_HOSTNAME \r\n" 
        HTTP_CONTENT_TYPE_TEXT_PLAIN
        "Content-Length: %d\r\n"
         HTTP_CONNECTION_CLOSE
        "\r\n"
        "%s",
        strlen(payload),
        payload
    );

    remaining = httpRequest;
    while ((result = socket.send((uint8_t*)remaining, strlen(remaining))) > 0) {
        remaining += result;
        if (*remaining == '\0') break;
    }
    if (result < 0) {
        snprintf(logMessage, sizeof(logMessage), LOG_MSG_SOCKET_SEND_ERROR, result);
        uartUSB.write(logMessage, strlen(logMessage));
        this->disconnect (ethernetModule, &socket);
        if (this->currentStatus ==  GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_ETHERNET ) {
            this->currentStatus = GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_MOBILE_NETWORK;
            this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
            return;
        }
        if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_ETHERNET) {
            this->currentStatus = GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_MOBILE_NETWORK;
            this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
            return;
        }
        return;
    }

    snprintf(logMessage, sizeof(logMessage), LOG_MSG_HTTP_POST_SENT);
    uartUSB.write(logMessage, strlen(logMessage));

    while (socket.available() == 0) {
        if (time(NULL) > timeOut) {
            snprintf(logMessage, sizeof(logMessage), LOG_MSG_CONNECTION_TIMEOUT);
            uartUSB.write(logMessage, strlen(logMessage));
            this->disconnect (ethernetModule, &socket);
            if (this->currentStatus ==  GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_ETHERNET ) {
                this->currentStatus = GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_MOBILE_NETWORK;
                this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
            return;
            }
            if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_ETHERNET) {
                this->currentStatus = GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_MOBILE_NETWORK;
                this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
                return;
            }
            return;
        }
    }


    while ((result = socket.available()) > 0) {
        recvBuf = (uint8_t*)malloc(result);
        result = socket.recv(recvBuf, result);
        if (result < 0) {
            snprintf(logMessage, sizeof(logMessage),"Error! socket.recv() returned: %d\n", result);
            uartUSB.write(logMessage, strlen(logMessage));
            this->disconnect (ethernetModule, &socket);
            free(recvBuf);
            if (this->currentStatus ==  GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_ETHERNET ) {
                this->currentStatus = GATEWAY_STATUS_RECEPTED_LORALORA_MESSAGE_TRYING_MOBILE_NETWORK;
                this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
                return;
            }
            if (this->currentStatus == GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_ETHERNET) {
                this->currentStatus = GATEWAY_STATUS_RECEPTED_LORAGNSS_MESSAGE_TRYING_MOBILE_NETWORK;
                this->gateway->changeState (new ConnectingToMobileNetwork (this->gateway, this->currentStatus));
                return;
            }
            return;
        }
        snprintf(logMessage, sizeof(logMessage),"%.*s\r\n", result, recvBuf);
        uartUSB.write(logMessage, strlen(logMessage));
        this->gateway->progressOnHashChain ();
        this->gateway->increaseSequenceNumber ();
        free(recvBuf);
    }
    uartUSB.write("\r\n", strlen("\r\n"));

    // ==== Cerrar conexión ====
    this->disconnect(ethernetModule, &socket);
    this->gateway->changeState (new SensingBatteryStatus (this->gateway));
    return;
}


/*
void SendingMessageThroughEthernet::exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
    const time_t    TIMEOUT = 5;    // Connection timeout time
    time_t          timeOut;
    char*           remaining;
    uint8_t*        recvBuf;
    int             result;
    char logMessage [1024];


    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
   
    // Crear socket TCP
    TcpClient socket;
   
    result = socket.open(ethernetModule);
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.open() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage));
        return;
    }


    // Conectar al servidor ngrok
    timeOut = time(NULL) + TIMEOUT;
    snprintf(logMessage, sizeof(logMessage), "Connecting to ngrok server ...\r\n");
    uartUSB.write(logMessage, strlen(logMessage));

    watchdog.kick();
    result = socket.connect("intent-lion-loudly.ngrok-free.app", 80); // HTTP plano
    //result = socket.connect("webhook.site", 80); // HTTP plano  //  LocalHost 5133
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.connect() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage));
        this->connectionRetries ++;
        if (this->connectionRetries >= MAX_RETRIES) {
            this->disconnect (ethernetModule, &socket);
            return;
        }
        return;
    }

    snprintf(logMessage, sizeof(logMessage), "Server connected.\r\n");
    uartUSB.write(logMessage, strlen(logMessage));

    // ==== Construcción del cuerpo JSON ====
    char body[128];
    snprintf(body, sizeof(body), "{\"id\":%d,\"message\":\"%s\"}", this->IdDevice, this->payload);

    // ==== Construcción del POST HTTP ====
    char httpRequest[512];
    snprintf(httpRequest, sizeof(httpRequest),
      //  "POST /api/canal/envio HTTP/1.1\r\n"
      //  "Host: intent-lion-loudly.ngrok-free.app\r\n"      // LocalHost

        "POST /apendice/canal-secundario/envio HTTP/1.1\r\n"
        "Host: intent-lion-loudly.ngrok-free.app\r\n"       
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        strlen(body),
        body
    );

    // ==== Envío al servidor ====
    remaining = httpRequest;
    while ((result = socket.send((uint8_t*)remaining, strlen(remaining))) > 0) {
        remaining += result;
        if (*remaining == '\0') break;
    }
    if (result < 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.send() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage));
        this->disconnect (ethernetModule, &socket);
        return;
    }

    snprintf(logMessage, sizeof(logMessage), "HTTP POST sent, waiting for response...\r\n");
    uartUSB.write(logMessage, strlen(logMessage));

    // ==== Esperar respuesta del servidor ====
    while (socket.available() == 0) {
        if (time(NULL) > timeOut) {
            snprintf(logMessage, sizeof(logMessage),"Connection time out.\r\n");
            uartUSB.write(logMessage, strlen(logMessage));
            this->disconnect (ethernetModule, &socket);
            return;
        }
    }

    // ==== Leer respuesta HTTP ====
    while ((result = socket.available()) > 0) {
        recvBuf = (uint8_t*)malloc(result);
        result = socket.recv(recvBuf, result);
        if (result < 0) {
            snprintf(logMessage, sizeof(logMessage),"Error! socket.recv() returned: %d\n", result);
            uartUSB.write(logMessage, strlen(logMessage));
            this->disconnect (ethernetModule, &socket);
            free(recvBuf);
            return;
        }
        snprintf(logMessage, sizeof(logMessage),"%.*s\r\n", result, recvBuf);
        uartUSB.write(logMessage, strlen(logMessage));
        free(recvBuf);
    }
    uartUSB.write("\r\n", strlen("\r\n"));

    // ==== Cerrar conexión ====
    this->disconnect(ethernetModule, &socket);

    wait_us (10000);
}
*/



/* TCP
void SendingTCPMessage::exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
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
        return;
        // change state
    }

    timeOut = time(NULL) + TIMEOUT;
    //printf("Connecting to the 'ifconfig.io' server ...\r\n");
    snprintf(logMessage, sizeof(logMessage), "Connecting to the TCP server ...\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug

    //watchdog.stop();
    watchdog.kick();
    result = socket.connect("186.19.62.251", 123); // modificar ip y puerto
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.connect() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        this->connectionRetries ++;
       // watchdog.start(TIMEOUT_MS);
        if (this->connectionRetries >= MAX_RETRIES) {
            this->disconnect (ethernetModule, &socket);
            return;
        }
        return;
    }
    // watchdog.start(TIMEOUT_MS);
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
            return;
        }
        snprintf(logMessage, sizeof(logMessage),"%.*s\r\n", result, recvBuf);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        free(recvBuf);
    }
    uartUSB.write("\r\n", strlen("\r\n")); // Debug
}
*/




//=====[Implementations of private functions]==================================
void SendingMessageThroughEthernet::disconnect (UipEthernet * ethernetModule, TcpClient * socket) {
    uartUSB.write("disconnecting\r\n", strlen("disconnecting\r\n"));
    socket->close();
    ethernetModule->disconnect();

}


/*

void SendingTCPMessage::exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay) {
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
        return;
        // change state
    }

    timeOut = time(NULL) + TIMEOUT;
    //printf("Connecting to the 'ifconfig.io' server ...\r\n");
    snprintf(logMessage, sizeof(logMessage), "Connecting to the TCP server ...\r\n");
    uartUSB.write(logMessage, strlen(logMessage)); // Debug

    //watchdog.stop();
    watchdog.kick();
    result = socket.connect("186.19.62.251", 123); // modificar ip y puerto
    if (result != 0) {
        snprintf(logMessage, sizeof(logMessage), "Error! socket.connect() returned: %d\n", result);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        this->connectionRetries ++;
       // watchdog.start(TIMEOUT_MS);
        if (this->connectionRetries >= MAX_RETRIES) {
            this->disconnect (ethernetModule, &socket);
            return;
        }
        return;
    }
    // watchdog.start(TIMEOUT_MS);
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
            return;
        }
        snprintf(logMessage, sizeof(logMessage),"%.*s\r\n", result, recvBuf);
        uartUSB.write(logMessage, strlen(logMessage)); // Debug
        free(recvBuf);
    }
    uartUSB.write("\r\n", strlen("\r\n")); // Debug
}

*/
