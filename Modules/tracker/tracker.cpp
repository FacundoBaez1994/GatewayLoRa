//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart





//=====[Declaration of private defines]========================================
#define LATENCY        2000
#define TIMEOUT_MS     2000
#define POWERCHANGEDURATION  700
#define IP      "192.168.1.35"
#define GATEWAY "192.168.1.1"
#define NETMASK "255.255.255.0"



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
    this->LoRaTransciver = new LoRaClass ();
    if (!this->LoRaTransciver->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    } else {
        uartUSB.write ("LoRa init ok", strlen ("LoRa init ok"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    this->latency = new NonBlockingDelay (LATENCY  );

    /*
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Tracker initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    
    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->socketTargetted = new TcpSocket;
    this->socketTargetted->IpDirection = new char[16]; // 
    strcpy(this->socketTargetted->IpDirection, "186.19.62.251");
    this->socketTargetted->TcpPort = 123;

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->date  = new char [10];
    this->currentCellInformation->time  = new char [10];
    this->currentCellInformation->band = new char [20];

    this->currentGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;
    */
}


tracker::~tracker() {
    /*
    delete[] this->currentCellInformation->date;
    this->currentCellInformation->date = NULL;
    delete[] this->currentCellInformation->time;
    this->currentCellInformation->time = NULL;
    delete[] this->currentCellInformation->band;
    this->currentCellInformation->band = NULL;
    delete this->currentCellInformation;
    this->currentCellInformation = NULL;
    delete this->currentGNSSdata;
    delete[] this->socketTargetted->IpDirection; // Libera la memoria asignada a IpDirection
    this->socketTargetted->IpDirection = NULL;
    delete this->socketTargetted; // Libera la memoria asignada al socketTargetted
    this->socketTargetted = NULL;
    delete this->latency;
    this->latency = NULL; 
    delete this->currentGNSSModule;
    this->currentGNSSModule = NULL;
    delete this->cellularTransceiver;
    this->cellularTransceiver = NULL;
    */
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void tracker::update () {
    char ACKmessage[20] = {0};
    char message[50];
    char buffer[64];
    static bool messageReceived = false;
    static bool ACKSent = false; 
    int deviceId = 0;
    int messageNumber = 0;
    char payload[50] = {0}; // Espacio suficiente para almacenar el payload

    const time_t    TIMEOUT = 5;    // Connection timeout time
    time_t          timeOut;
    //char            data[] = "GET / HTTP/1.1\r\nHost: ifconfig.io\r\nConnection: close\r\n\r\n";
    char*           remaining;
    uint8_t*        recvBuf;
    int             result;


    // Intenta analizar el paquete
    if (messageReceived  == false) {
        this->LoRa_rxMode ();
        int packetSize = this->LoRaTransciver->parsePacket();
        if (packetSize) {
            uartUSB.write("Packet Received!\r\n", strlen("Packet Received!\r\n")); // Debug

            int maxIterations = 100; // Límite para evitar un ciclo infinito
            int iterations = 0;

            // Leer los datos disponibles
            while (this->LoRaTransciver->available() > 0 && iterations < maxIterations) {
                ssize_t bytesRead = this->LoRaTransciver->read(reinterpret_cast<uint8_t*>(buffer), sizeof(buffer));
                if (bytesRead > 0) {
                    // Enviar los bytes leídos al puerto serie
                    uartUSB.write(buffer, bytesRead);
                    uartUSB.write ("\r\n", strlen("\r\n"));
                }
                iterations++;
            }

            if (iterations >= maxIterations) {
                uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
            }
            //
            if (sscanf(buffer, "%d,%d,%49s", &deviceId, &messageNumber, payload) == 3) {
                // Desglose exitoso
                snprintf(message, sizeof(message), "Device ID: %d\r\n", deviceId);
                uartUSB.write(message, strlen(message));

                snprintf(message, sizeof(message), "Message Number: %d\r\n", messageNumber);
                uartUSB.write(message, strlen(message));

                snprintf(message, sizeof(message), "Payload: %s\r\n", payload);
                uartUSB.write(message, strlen(message));
            } else {
                uartUSB.write("Error parsing message.\r\n", strlen("Error parsing message.\r\n"));
            }
            // Leer el RSSI del paquete recibido
            int packetRSSI = this->LoRaTransciver->packetRssi();
            snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
            uartUSB.write(message, strlen(message));
            wait_us (200000); // eliminar bloqueo
            messageReceived  = true;
            this->latency->restart();
        }
    }

    /// ACK Sending
    if (messageReceived  == true && this->latency->read())  {
        snprintf(ACKmessage, sizeof(ACKmessage), "%d,%d,ACK", deviceId, messageNumber);
        this->LoRa_txMode ();
        uartUSB.write("Sending Acknowledgment Message\r\n", strlen("Sending Acknowledgment Message\r\n")); // Debug
        this->LoRaTransciver->beginPacket();
        this->LoRaTransciver->write((uint8_t *)ACKmessage, strlen(ACKmessage));
        this->LoRaTransciver->endPacket();
        messageReceived = false; 
    }
}

//=====[Implementations of private methods]==================================
void tracker::LoRa_rxMode() {
    LoRa.disableInvertIQ();               // Disable I/Q inversion for reception
   // LoRa.receive();                       // Set receive mode
}

void tracker::LoRa_txMode() {
    LoRa.idle();                          // Standby mode
    LoRa.enableInvertIQ();                // Enable I/Q inversion for transmission
}