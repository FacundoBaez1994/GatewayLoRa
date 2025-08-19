//=====[Libraries]=============================================================

#include "Gateway.h"
#include "Debugger.h" // due to global usbUart


//=====[Declaration of private defines]========================================
#define LATENCY        8000
#define POWERCHANGEDURATION  700
#define MAX_TIMEOUT_COUNTER 5

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
/** 
* @brief Contructor method creates a new gatewayGPS instance ready to be used
*/
Gateway::Gateway () {

    this->LoRaTransciever = new LoRaClass ();
    if (!this->LoRaTransciever->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    //this->LoRaTransciever->setSpreadingFactor(12);   // ranges from 6-12,default 7
    //this->LoRaTransciever->setSyncWord(0xF3);  // ranges from 0-0xFF, default 0x34,
    //this->LoRaTransciever->setSignalBandwidth(125E3); // 125 kHz
    this->timeout = new NonBlockingDelay (LATENCY);

    this->RFState = new WaitingForMessage (this);

    this->encrypter = new EncrypterBase64 ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();
    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
    this->decrypter = new DecrypterBase64 ();


    this->deviceId = 356789104982315;
    this->messageNumber = 1;
    /*
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Gateway initialization";
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


Gateway::~Gateway() {
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


    delete this->encrypter;
    this->encrypter = NULL;
    delete this->authgen;
    this->authgen = NULL;
    delete this->ckgen;
    this->ckgen = NULL;
    delete this->checksumVerifier;
    this->checksumVerifier = NULL;
    delete this->authVer;
    this->authVer = NULL;
    delete this->decrypter;
    this->decrypter = NULL;
}


/** 
* @brief Main rutine of the gateway device
*   
*
*/
void Gateway::update () {
    int deviceIdReceived;
    int messageNumberReceived;
    char payload[50] = {0};
    static char messageToSend[2048];
    //static char plainMessageToSend[256];
    static char ACKMessage [2048];
    char logMessage[50];
    int static timeoutCounter = 0;

    static bool plainTextMessageFormed = false;
    static bool debugFlag = false;
     /*
    if ( plainTextMessageFormed ==  false) {
        snprintf( messageToSend, sizeof(messageToSend), "%d,%d,hello", this->deviceId, this->messageNumber);
        strcpy (plainMessageToSend, messageToSend);
        plainTextMessageFormed =  true; 
    }
     */
    this->RFState->sendAcknowledgement (this->LoRaTransciever,  messageToSend, this->timeout);
    this->RFState->waitForMessage (this->LoRaTransciever, ACKMessage, this->timeout);

 /*
    if (this->RFState->getAcknowledgement (this->LoRaTransciever, ACKMessage, this->timeout) == true) {
       if (this->checkMessageIntegrity (plainMessageToSend, ACKMessage))  {
           this->messageNumber++;
           this->timeout->restart();
            std::fill(std::begin(messageToSend), std::end(messageToSend), '\0');
            plainTextMessageFormed =  false; 
            this->changeState ( new SendingMessage (this));
       }
    }
    */
    /*
    if (this->timeout->read()) {
        uartUSB.write ("Timeout for ACK\r\n", strlen ("Timeout for ACK\r\n"));  // debug only
        std::fill(std::begin(messageToSend), std::end(messageToSend), '\0');
        plainTextMessageFormed =  false; 
        this->changeState ( new SendingMessage (this));
    }
    */
}


void Gateway::changeState  (RFTransicieverState * newState) {
    delete this->RFState;
    this->RFState = newState;
}

 bool Gateway::checkMessageIntegrity ( char *messageReceived) {
    char logMessage [60];

    char payload [60];
    long long int deviceIdReceived;
    int messageNumberReceived; 
    char payloadReceived [60];

    if (sscanf(messageReceived, "%lld,%d,%s", &deviceIdReceived, &messageNumberReceived, payloadReceived) == 3) {
        bool messageCorrect = false;
        uartUSB.write ("\r\n", strlen("\r\n"));
        snprintf(logMessage, sizeof(logMessage), "Device ID Received: %lld\r\n", deviceIdReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (deviceIdReceived == this->deviceId) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), "Message Number Received: %d\r\n", messageNumberReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (messageNumberReceived == this->messageNumber) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        snprintf(logMessage, sizeof(logMessage), "Payload Received: %s\r\n", payloadReceived);
        uartUSB.write(logMessage, strlen(logMessage));
        if (strcmp (payloadReceived, "ACK") == 0 || strcmp (payloadReceived, "ACK\r") == 0 ||
         strcmp (payloadReceived, "ACK\r\n") == 0 ) {
            uartUSB.write("OK\r\n", strlen("OK\r\n"));
        } else {
            uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
            return false;
        }
        this->messageNumber ++;
        return true;
    } else {
        uartUSB.write("ACK invalido\r\n", strlen("ACK invalido\r\n"));
        return false;
    }
 }

bool Gateway::prepareMessage (char * messageOutput, unsigned int sizeOfMessage) {
    this->encrypter->setNextHandler(this->authgen)->setNextHandler(this->ckgen);
    if (this->encrypter->handleMessage (messageOutput, sizeOfMessage) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Gateway::processMessage (char * incomingMessage, unsigned int sizeOfMessage) {
    this->checksumVerifier->setNextHandler(this->authVer)->setNextHandler(this->decrypter);
    if (this->checksumVerifier->handleMessage (incomingMessage, sizeOfMessage) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

//=====[Implementations of private methods]==================================



char* Gateway::formMessage(GNSSData* GNSSInfo) {
    static char message[50]; 
    snprintf(message, sizeof(message), "%.6f,%.6f", GNSSInfo->latitude,
     GNSSInfo->longitude);
    return message;
}

char* Gateway::formMessage(CellInformation* aCellInfo, std::vector<CellInformation*> 
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

char* Gateway::formMessage(CellInformation* aCellInfo, GNSSData* GNSSInfo, BatteryData  * batteryStatus) {
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