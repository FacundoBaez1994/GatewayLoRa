//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart
#include "mbed.h"
#include "UipEthernet.h"
#include "TcpClient.h"

#define IP      "192.168.1.35"
#define GATEWAY "192.168.1.1"
#define NETMASK "255.255.255.0"

const uint8_t   MAC[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
UipEthernet     net(MAC, PB_5, PB_4, PB_3, PA_4);   // mac, mosi, miso, sck, cs
DigitalOut resetEth (PA_1);

/**
 * @brief
 * @note
 * @param
 * @retval
 */


//=====[Declaration of private defines]========================================
#define LATENCY        5000
#define TIMEOUT_MS     5000
#define POWERCHANGEDURATION  700

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
/** 
* @brief Contructor method creates a new trackerGPS instance ready to be used
*/
tracker::tracker () {
    /*
    this->LoRaTransciver = new LoRaClass ();
    if (!this->LoRaTransciver->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    */


}


tracker::~tracker() {

}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    char message[50];
    char buffer[64];

     const time_t    TIMEOUT = 5;    // Connection timeout time
    time_t          timeOut;
    //char            data[] = "GET / HTTP/1.1\r\nHost: ifconfig.io\r\nConnection: close\r\n\r\n";
    char            data[] = "Hola Mundo!\r\n";
    char*           remaining;
    uint8_t*        recvBuf;
    int             result;

    printf("Starting ...\r\n");
    resetEth.write(HIGH);

    //net.set_network(IP, NETMASK, GATEWAY);  // include this to use static IP address
    net.connect();

    // Show the network address
    const char*     ip = net.get_ip_address();
    const char*     netmask = net.get_netmask();
    const char*     gateway = net.get_gateway();
    printf("IP address: %s\n", ip ? ip : "None");
    printf("Netmask: %s\n", netmask ? netmask : "None");
    printf("Gateway: %s\n", gateway ? gateway : "None");

    // Open a socket on the network interface, and create a TCP connection to ifconfig.io
    TcpClient   socket;

    result = socket.open(&net);
    if (result != 0) {
        printf("Error! socket.open() returned: %d\n", result);
    }

    timeOut = time(NULL) + TIMEOUT;
    //printf("Connecting to the 'ifconfig.io' server ...\r\n");
    printf("Connecting to the TCP server ...\r\n");

    result = socket.connect("186.19.62.251", 123); // modificar ip y puerto
    if (result != 0) {
        printf("Error! socket.connect() returned: %d\n", result);
        goto DISCONNECT;
    }

    printf("Server connected.\r\n");
    printf("Sending data to server:\r\n");
    remaining = data;
    result = strlen(remaining);
    while (result) {
        result = socket.send((uint8_t*)remaining, strlen(remaining));
        if (result < 0) {
            printf("Error! socket.send() returned: %d\n", result);
            goto DISCONNECT;
        }
        printf("%.*s", result, remaining);
        remaining += result;
    }

    printf("Waiting for data from server:\r\n");
    while (socket.available() == 0) {
        if (time(NULL) > timeOut) {
            printf("Connection time out.\r\n");
            goto DISCONNECT;
        }
    }

    printf("Data received:\r\n");
    while ((result = socket.available()) > 0) {
        recvBuf = (uint8_t*)malloc(result);
        result = socket.recv(recvBuf, result);
        if (result < 0) {
            printf("Error! socket.recv() returned: %d\n", result);
            goto DISCONNECT;
        }
        printf("%.*s\r\n", result, recvBuf);
        free(recvBuf);
    }

    printf("\r\n");

DISCONNECT:
    // Close the socket to return its memory and bring down the network interface
    socket.close();

    // Bring down the ethernet interface
    net.disconnect();
    printf("Done\n");


/*
      // try to parse packet
    int packetSize = this->LoRaTransciver->parsePacket();
    if (packetSize) {
    // received a packet
     uartUSB.write ("Packet Received!", strlen ("Packet Received!"));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    // read packet
    while (this->LoRaTransciver->available()) {
        ssize_t bytesRead = LoRa.read(buffer, sizeof(buffer)); // Lee hasta 64 bytes o lo que esté disponible
        if (bytesRead > 0) {
            for (ssize_t i = 0; i < bytesRead; i++) {
                uartUSB.write(&buffer[i], 1); // Envía cada byte al puerto serie
            }
        }
    }
    uartUSB.write ( "\r\n",  3 );  // debug only

    // print RSSI of packet
   // Serial.print("' with RSSI ");
    //Serial.println(LoRa.packetRssi());
    }

*/


}

//=====[Implementations of private methods]==================================
char* tracker::formMessage(GNSSData* GNSSInfo) {
    static char message[50]; 
    snprintf(message, sizeof(message), "%.6f,%.6f", GNSSInfo->latitude,
     GNSSInfo->longitude);
    return message;
}

char* tracker::formMessage(CellInformation* aCellInfo, std::vector<CellInformation*> 
&neighborsCellInformation, BatteryData  * batteryStatus) {
    static char message[500];
    char neighbors[50];
    int lac;
    int idCell;
    int tech;
    int mcc;
    int mnc;
    snprintf(message, sizeof(message), 
            "MN,MN,%X,%X,%d,%d,%.2f,%d,%d,%d,%s,%s,%s,%d,%d", 
            aCellInfo->lac,
            aCellInfo->cellId,
            aCellInfo->mcc,
            aCellInfo->mnc,
            aCellInfo->signalLevel,
            aCellInfo->accessTechnology,
            aCellInfo->registrationStatus,
            aCellInfo->channel,
            aCellInfo->band,
            aCellInfo->date,
            aCellInfo->time,
            batteryStatus->batteryChargeStatus,
            batteryStatus->chargeLevel
            );
    snprintf(neighbors, sizeof(neighbors),"size of vector %d", neighborsCellInformation.size()); 
    uartUSB.write (neighbors , strlen (neighbors ));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only        
    for (size_t i = 0; i < neighborsCellInformation.size(); ++i) {
        tech = neighborsCellInformation[i]->tech;
        mcc = neighborsCellInformation[i]->mcc;
        mnc = neighborsCellInformation[i]->mnc;
        lac = neighborsCellInformation[i]->lac;
        idCell = neighborsCellInformation[i]->cellId;
        snprintf(neighbors, sizeof(neighbors),",%dG,%d,%d,%X,%X",tech,mcc,mnc,lac, idCell); 
        uartUSB.write (neighbors , strlen (neighbors ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        // Concatenar el mensaje de la celda vecina al mensaje principal
        strncat(message, neighbors, sizeof(message) - strlen(message) - 1);
    }   
    return message;
}

char* tracker::formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo, BatteryData  * batteryStatus) {
    static char message[200]; 
    snprintf(message, sizeof(message), 
             "MN,GNSS,%.6f,%.6f,%.2f,%.2f,%.2f,%.2f,%X,%X,%d,%d,%.2f,%d,%d,%d,%s,%s,%s,%d,%d", 
            GNSSInfo->latitude,
            GNSSInfo->longitude,
            GNSSInfo->hdop,
            GNSSInfo->altitude,
            GNSSInfo->cog,
            GNSSInfo->spkm,
            aCellInfo->lac,
            aCellInfo->cellId,
            aCellInfo->mcc,
            aCellInfo->mnc,
            aCellInfo->signalLevel,
            aCellInfo->accessTechnology,
            aCellInfo->registrationStatus,
            aCellInfo->channel,
            aCellInfo->band,
            GNSSInfo->date,
            GNSSInfo->utc,
            batteryStatus->batteryChargeStatus,
            batteryStatus->chargeLevel
            );
    return message;
}