//=====[Libraries]=============================================================

#include "Gateway.h"
#include "Debugger.h" // due to global usbUart





//=====[Declaration of private defines]========================================
#define LATENCY        300
//#define TIMEOUT_MS     3000
#define IP      "192.168.1.35"
#define GATEWAY "192.168.1.1"
#define NETMASK "255.255.255.0"

const uint8_t   MAC[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

#define TIMEOUT_MS     5000


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
Gateway::Gateway () {
    uartUSB.write ("Gateway Initializing\r\n", strlen ("Gateway Initializing\r\n"));  // debug only

    this->LoRaTransciever = new LoRaClass ();
    if (!this->LoRaTransciever->begin (915E6)) {
        uartUSB.write ("LoRa Module Failed to Start!", strlen ("LoRa Module Failed to Start"));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }
    this->LoRaTransciever->setSpreadingFactor(12);   // ranges from 6-12,default 7
    this->LoRaTransciever->setSyncWord(0xF3);  // ranges from 0-0xFF, default 0x34,
    this->LoRaTransciever->setSignalBandwidth(125E3); // 125 kHz

    this->ethernetModule = new UipEthernet (MAC, PB_5, PB_4, PB_3, PA_4);  // mac, mosi, miso, sck, cs
    this->resetEth =  new DigitalOut (PA_1);
    this->resetEth->write(HIGH);

    this->timer = new NonBlockingDelay (LATENCY);

    this->currentState = new WaitingForMessage (this);

    this->encrypter = new Encrypter ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();

    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
    this->decrypter = new Decrypter ();




    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);


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


    this->jwt = new CustomJWT (this->JWTKey, 256);
}


Gateway::~Gateway() {
    delete this->currentState;
    this->currentState = NULL;
    delete this->LoRaTransciever; 
    this->LoRaTransciever = NULL;
    delete this->ethernetModule; 
    this->ethernetModule = NULL;
    delete this->resetEth; 
    this->resetEth = NULL;
    delete this->timer;
    this->timer = NULL;

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
}


/** 
* @brief Main rutine of the tracker device
*   
*
*/
void Gateway::update () {
    static char formattedMessage [2024];
    static char inertialData [200];
    float temperature;
    static char receivedMessage [2024];

    static std::vector<CellInformation*> neighborsCellInformation;
    static int numberOfNeighbors = 0;

    this->currentState->receiveMessage (this->LoRaTransciever, this->timer);
    this->currentState->sendAcknowledgement (this->LoRaTransciever, this->timer);
    this->currentState->sendTCPMessage (this->ethernetModule, this->timer);

    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.kick();

    this->currentState->awake(this->cellularTransceiver, this->latency);
    this->currentState->updatePowerStatus (this->cellularTransceiver, this->batteryStatus);
    this->currentState->obtainGNSSPosition (this->currentGNSSModule, this->currentGNSSdata);
    this->currentState->connectToMobileNetwork (this->cellularTransceiver,
    this->currentCellInformation);
    this->currentState->obtainNeighborCellsInformation (this->cellularTransceiver, 
    neighborsCellInformation, numberOfNeighbors );
    this->currentState->formatMessage (formattedMessage, this->currentCellInformation,
    this->currentGNSSdata, neighborsCellInformation, this->batteryStatus); 
    this->currentState->exchangeMessages (this->cellularTransceiver,
    formattedMessage, this->socketTargetted, receivedMessage ); // agregar modulo LoRa al argumento
    this->currentState->goToSleep (this->cellularTransceiver);
}


void Gateway::changeState  (GatewayState * newState) {
    delete this->currentState;
    this->currentState = newState;
}

bool Gateway::prepareMessage (char * messageOutput) {
    this->encrypter->setNextHandler(this->authgen)->setNextHandler(this->ckgen);
    if (this->encrypter->handleMessage (messageOutput) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Gateway::processMessage (char * incomingMessage) {
    this->checksumVerifier->setNextHandler(this->authVer)->setNextHandler(this->decrypter);
    if (this->checksumVerifier->handleMessage (incomingMessage) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}


void printData(char *data, size_t dataLen) {
    char logMessage [150];
    snprintf(logMessage, sizeof(logMessage), "\n\rData: = %s \n\r", data);
    uartUSB.write(logMessage , strlen(logMessage ));
      snprintf(logMessage, sizeof(logMessage), "\n\rData Length: = %i \n\r", dataLen);
    uartUSB.write(logMessage , strlen(logMessage ));
}


void Gateway::encodeJWT(char * payloadToJWT, char * jwtEncoded) {
    char logMessage [250];
    this->jwt->allocateJWTMemory();
    snprintf(logMessage, sizeof(logMessage), "Generating a JWT"); 
    uartUSB.write(logMessage , strlen(logMessage ));

    this->jwt->encodeJWT( payloadToJWT);

    snprintf(logMessage, sizeof(logMessage), "Header Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->header, this->jwt->headerLength);

    snprintf(logMessage, sizeof(logMessage), "Payload Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->payload, this->jwt->payloadLength);

    snprintf(logMessage, sizeof(logMessage), "Signaure Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));;
    printData(this->jwt->signature, this->jwt->signatureLength);

    snprintf(logMessage, sizeof(logMessage), "Final Output Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));;
    printData(this->jwt->out, this->jwt->outputLength);

    strcpy (jwtEncoded, this->jwt->out);
    jwtEncoded [this->jwt->outputLength] = '\0';
    this->jwt->clear();
}



void Gateway::decodeJWT (char * jwtToDecode, char * payloadRetrived) {
    char logMessage [150];
    this->jwt->allocateJWTMemory();
    //Decode the JWT
    snprintf(logMessage, sizeof(logMessage), "Decoding and verifying the JWT\n\r");
    uartUSB.write(logMessage , strlen(logMessage ));
    snprintf(logMessage, sizeof(logMessage), "JWT Decode ended with result: \n\r");
    uartUSB.write(logMessage , strlen(logMessage ));
    //Code 0: Decode success \n Code 1: Memory not allocated \n Code 2: Invalid JWT \n Code 3: Signature Mismatch
    snprintf(logMessage, sizeof(logMessage), "\n\rCode result = %i \n\r", this->jwt->decodeJWT(jwtToDecode)); 
    uartUSB.write(logMessage , strlen(logMessage ));

    snprintf(logMessage, sizeof(logMessage), "Header Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->header, this->jwt->headerLength);

    snprintf(logMessage, sizeof(logMessage), "Payload Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->payload, this->jwt->payloadLength);

    snprintf(logMessage, sizeof(logMessage), "Signaure Info"); 
    uartUSB.write(logMessage , strlen(logMessage ));
    printData(this->jwt->signature, this->jwt->signatureLength);
    
    strcpy (payloadRetrived, this->jwt->payload);
    this->jwt->clear();
}


//=====[Implementations of private methods]==================================
