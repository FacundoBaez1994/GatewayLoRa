//=====[Libraries]=============================================================

#include "tracker.h"
#include "Debugger.h" // due to global usbUart


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
    this->LoRaTransciver = new LoRaClass ();
    if (!this->LoRaTransciver->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }


    /*
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Tracker initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    this->latency = new NonBlockingDelay (LATENCY);
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
    char message[50];
    char buffer[64];
    static bool messageRead = false;


    // Intenta analizar el paquete
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
            }
            iterations++;
        }

        if (iterations >= maxIterations) {
            uartUSB.write("Warning: Exceeded max iterations\r\n", strlen("Warning: Exceeded max iterations\r\n"));
        }

        // Leer el RSSI del paquete recibido
        int packetRSSI = this->LoRaTransciver->packetRssi();
        snprintf(message, sizeof(message), "packet RSSI: %d\r\n", packetRSSI);
        uartUSB.write(message, strlen(message));
    }


/*
      // try to parse packet
    int packetSize = this->LoRaTransciver->parsePacket();
    if (packetSize) {
    // received a packet
     uartUSB.write ("Packet Received!", strlen ("Packet Received!"));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only

    // read packet
    while (this->LoRaTransciver->available() ) {
        ssize_t bytesRead = LoRa.read(buffer, sizeof(buffer)); // Lee hasta 64 bytes o lo que esté disponible
        if (bytesRead > 0) {
            for (ssize_t i = 0; i < bytesRead; i++) {
                uartUSB.write(&buffer[i], 1); // Envía cada byte al puerto serie
            }
        }
        //int available = this->LoRaTransciver->available();
        //snprintf(message, sizeof(message), "available %d bytesRead: %d", available, bytesRead);
        //uartUSB.write (message, strlen (message));  // debug only
        //uartUSB.write ( "\r\n",  3 );  // debug onl
       
    }
    int packetRSSI = LoRa.packetRssi();
    snprintf(message, sizeof(message), "packet RSSI: %d", packetRSSI);
    uartUSB.write (message, strlen (message));  // debug only
    uartUSB.write ( "\r\n",  3 );  // debug only
    // print RSSI of packet
    }

*/

    /*
    static char* formattedMessage;
    static char receivedMessage [200];
    static GNSSState_t GnssCurrentStatus;
    static CellularConnectionStatus_t currentConnectionStatus;
    static CellularTransceiverStatus_t currentTransmitionStatus;
    static bool newDataAvailable = false;

    static bool GNSSAdquisitionSuccesful = false;
    static bool enableTransmission = false; 
    static bool transimissionSecuenceActive =  false;
    static bool messageFormatted = false;
    static bool batterySensed = false;
    static bool enablingGoingToSleep = false; 

    static std::vector<CellInformation*> neighborsCellInformation;
    static int numberOfNeighbors = 0;
    Watchdog &watchdog = Watchdog::get_instance(); // singletom

    watchdog.kick();
    this->cellularTransceiver->startStopUpdate();
    //this->currentGNSSModule->startStopUpdate();

    if (this->latency->read() && transimissionSecuenceActive == false) { // WRITE
        transimissionSecuenceActive = true;
        batterySensed = false;
        this->cellularTransceiver->awake();
    }

    if (batterySensed == false &&  transimissionSecuenceActive == true) {
        if (this->cellularTransceiver->measureBattery(this->batteryStatus)) {
           batterySensed = true;
            this->currentGNSSModule->enableGNSS();
        }
    }
    
    ////////////////////////// GNSS ////////////////////////////////
    GnssCurrentStatus = this->currentGNSSModule->retrivGeopositioning(this->currentGNSSdata);
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_OBTAIN ) {
        GNSSAdquisitionSuccesful = true;
        char StringToSendUSB [40] = "GNSS OBTAIN!!!!";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        this->cellularTransceiver->enableConnection();
    }
    if (GnssCurrentStatus == GNSS_STATE_CONNECTION_UNAVAILABLE ) {
        GNSSAdquisitionSuccesful = false;
        char StringToSendUSB [40] = "GNSS UNAVAILABLE!!!!";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        this->cellularTransceiver->enableConnection();
    }

    ////////////////////////////////////////////////////////////////////
    
    ////////////////////CELULLAR  CONNECTION/////////////////// 
    currentConnectionStatus = this->cellularTransceiver->connectToMobileNetwork(this->currentCellInformation);
        if (currentConnectionStatus == CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK) { 
            if (GNSSAdquisitionSuccesful == false) {
                if (messageFormatted == false) {
                
                    if (this->cellularTransceiver->retrivNeighborCellsInformation(
                    neighborsCellInformation, numberOfNeighbors)) {
                        formattedMessage = this->formMessage(this->currentCellInformation,
                        neighborsCellInformation, this->batteryStatus);
                        for (auto* cellInfo : neighborsCellInformation) {
                            delete cellInfo;  // Libera la memoria de cada puntero
                            cellInfo = nullptr;
                        }
                        neighborsCellInformation.clear();  // Limpia el vector 
                        messageFormatted = true;
                        uartUSB.write (formattedMessage , strlen (formattedMessage ));  // debug only
                        uartUSB.write ( "\r\n",  3 );  // debug only
                        this->cellularTransceiver->enableTransceiver();
                    } 
                    //messageFormatted = true; // ELIMINAR
                    //this->cellularTransmitter->enableTransmission();
                }
            } else {
                if (messageFormatted == false) {
                    formattedMessage = this->formMessage(this->currentCellInformation,
                     this->currentGNSSdata, this->batteryStatus);
                    messageFormatted = true;
                    uartUSB.write (formattedMessage , strlen (formattedMessage ));  // debug only
                    uartUSB.write ( "\r\n",  3 );  // debug only    
                    this->cellularTransceiver->enableTransceiver();
                }
            }
    } else if (currentConnectionStatus != CELLULAR_CONNECTION_STATUS_UNAVAIBLE && 
    currentConnectionStatus != CELLULAR_CONNECTION_STATUS_TRYING_TO_CONNECT) {
        char StringToSendUSB [50] = "Access to mobile network UNAVAILABLE!!!!";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        messageFormatted = false;
        enablingGoingToSleep = true;
    }
    ///////////////////////////////////////////////////


    ////////////////////CELULLAR  TRANSMISSION/////////////////// 
    currentTransmitionStatus = this->cellularTransceiver->exchangeMessages (formattedMessage,
     this->socketTargetted, receivedMessage, &newDataAvailable);
     if (currentTransmitionStatus == CELLULAR_TRANSCEIVER_STATUS_SEND_OK) {
        char StringToSendUSB [50] = "The message was send with success";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        messageFormatted = false;
        enablingGoingToSleep = true;
     }  else if (currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND
       && currentTransmitionStatus != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {
        char StringToSendUSB [50] = "The message couldn't be sent";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only}
        messageFormatted = false;
        enablingGoingToSleep = true;
     }
     if (newDataAvailable == true) {
        char StringToSendUSB [50] = "new Message received:";
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        snprintf(StringToSendUSB, sizeof(StringToSendUSB), "%s",  receivedMessage);
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        newDataAvailable = false;
     }
     //////////////////////////////////
      
    

    if (enablingGoingToSleep == true) {
        if (this->cellularTransceiver->goToSleep()) { 
            transimissionSecuenceActive = false;
            enablingGoingToSleep = false;
            this->latency->restart();
        }
    }

    watchdog.kick();
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