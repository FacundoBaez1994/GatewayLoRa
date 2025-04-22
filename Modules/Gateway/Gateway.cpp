//=====[Libraries]=============================================================

#include "Gateway.h"
#include "Debugger.h" // due to global usbUart
#include "SensingBatteryStatus.h"


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
* @brief Contructor method creates a new gatewayGPS instance ready to be used
*/
Gateway::Gateway () {
    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_MS);
    char StringToSendUSB [50] = "Gateway initialization";
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


    this->currentState =  new SensingBatteryStatus (this);

    this->jwt = new CustomJWT (this->JWTKey, 256);
}


Gateway::~Gateway() {
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
* @brief Main rutine of the gateway device
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
    // agregar dato IMU
    this->currentState->exchangeMessages (this->cellularTransceiver,
    formattedMessage, this->socketTargetted, receivedMessage ); // agregar modulo LoRa al argumento
    this->currentState->goToSleep (this->cellularTransceiver);
    
}


void Gateway::changeState  (GatewayState * newGatewayState) {
    delete this->currentState;
    this->currentState = newGatewayState;
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