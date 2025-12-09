//=====[Libraries]=============================================================
#include "Gateway.h"
#include "Debugger.h" // due to global usbUart
#include "SensingBatteryStatus.h"
#include "WaitingForMessage.h"


//=====[Declaration of private defines]========================================
#define EXTREMELY_LOW_LATENCY_MS   20000          // 20 seconds
#define VERY_LOW_LATENCY_MS        (1 * 60 * 1000)   // 1 minutes
#define LOW_LATENCY_MS             (10 * 60 * 1000)  // 10 minutes
#define MEDIUM_LATENCY_MS          (30 * 60 * 1000)  // 30 minutes
#define HIGH_LATENCY_MS            (60 * 60 * 1000)  // 1 hour
#define VERY_HIGH_LATENCY_MS       (6 * 60 * 60 * 1000)   // 6 hours
#define EXTREMELY_HIGH_LATENCY_MS  (24 * 60 * 60 * 1000)  // 24 hours

#define EXTREMELY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER   180    // 1 hour
#define VERY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER        120   // 2 hour
#define LOW_LATENCY_KEEP_ALIVE_MULTIPLIER            24  // 4 hours
#define MEDIUM_LATENCY_KEEP_ALIVE_MULTIPLIER          12 // 6 hours
#define HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER            8  // 8 hour
#define VERY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER       2  // 12 hours
#define EXTREMELY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER  1 // 24 hours

//#define HOUR_MS  (1 * 60 * 60 * 1000)  // 1 hours
#define HOUR_MS  (3 * 60 * 1000)  // 3 min TEST ONLY

#define TIMEOUT_WATCHDOG_TIMER_MS     5000
#define POWERCHANGEDURATION  700
#define TIME_BETWEEN_IMU_SAMPLES 10 // 10 seconds

#define STANDARD_TIMEOUT 8000

#define URL_PATH_MAIN_CHANNEL "https://intent-lion-loudly.ngrok-free.app/api/canal/envio"
#define URL_PATH_SECONDARY_CHANNEL "https://intent-lion-loudly.ngrok-free.app/apendice/canal-secundario/envio"
#define CURRENT_DEVICE_IDENTIFIER "device/gateway-001"

#define IP      "192.168.1.35"
#define GATEWAY "192.168.1.1"
#define NETMASK "255.255.255.0"

const uint8_t   MAC[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
Gateway::Gateway () {
    // metaData
    this->deviceIdentifier = new char [DEVICE_IDENTIFIER_BUFFER_SIZE];
    strcpy (this->deviceIdentifier, CURRENT_DEVICE_IDENTIFIER);
    this->prevChainHash = new char [HASH_BASE_64_BUFFER_SIZE];
    strcpy (this->prevChainHash, "-");
    this->currChainHash = new char [HASH_BASE_64_BUFFER_SIZE];
    strcpy (this->currChainHash, "-");

    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_WATCHDOG_TIMER_MS);
    char StringToSendUSB [50] = "Gateway initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    // Ethernet
    this->ethernetModule = new UipEthernet (MAC, PB_5, PB_4, PB_3, PA_4);  // mac, mosi, miso, sck, cs
    this->resetEth =  new DigitalOut (PA_1);
    this->resetEth->write(HIGH);


    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)


    this->serverTargetted = new RemoteServerInformation;
    this->serverTargetted->url = new char[URL_SERVER_BUFFER_SIZE]; // 
    strcpy (this->serverTargetted->url, URL_PATH_MAIN_CHANNEL);
    this->serverTargetted->secondaryUrl = new char[URL_SERVER_BUFFER_SIZE]; // 
    strcpy (this->serverTargetted->secondaryUrl, URL_PATH_SECONDARY_CHANNEL);

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->timestamp  = new char [TIMESTAMP_BUFFER_SIZE];
    this->currentCellInformation->band = new char [BAND_STRING_BUFFER_SIZE];

    this->gatewayGNSSdata = new GNSSData;
    this->gatewayGNSSdata->latitude = 0;
    this->gatewayGNSSdata->longitude = 0; 
    this->gatewayGNSSdata->altitude = 0;
    this->receptedGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;
    this->receptedBatteryData = new BatteryData;

    this->receptedImuData = new IMUData_t;
    this->receptedImuData->timestamp = new char [TIMESTAMP_BUFFER_SIZE];
    this->receptedImuData->timeBetweenSamples = TIME_BETWEEN_IMU_SAMPLES;

    this->currentState =  new SensingBatteryStatus (this); //WaitingForMessage
    //this->currentState =  new WaitingForMessage (this);

    this->LoRaTransciever = new LoRaClass ();
    this->LoRaTransciever->setSpreadingFactor(12);   // ranges from 6-12,default 7
    this->LoRaTransciever->setSyncWord(0xF3);  // ranges from 0-0xFF, default 0x34,
    this->LoRaTransciever->setSignalBandwidth(125E3); // 125 kHz

    this->timeout = new NonBlockingDelay (STANDARD_TIMEOUT);

     //this->encrypter = new Encrypter ();
    this->encrypterBase64 = new EncrypterBase64 ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();
    

    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
     //this->decrypter = new Decrypter ();
    this->decrypterBase64 = new DecrypterBase64 ();

    this->jwt = new JWTManager ();
}

Gateway::~Gateway() {
    delete [] this->deviceIdentifier;
    this->deviceIdentifier  = nullptr;
    delete [] this->prevChainHash;
    this->prevChainHash = nullptr;
    delete []  this->currChainHash;
    this->currChainHash = nullptr;

    // Ethernet
    delete this->resetEth; 
    this->resetEth = nullptr;
    delete this->ethernetModule; 
    this->ethernetModule = nullptr;

    delete this->receptedImuData->timestamp;
    this->receptedImuData->timestamp = nullptr;
    delete this->receptedImuData;
    this->receptedImuData = nullptr;

    delete this->receptedBatteryData;
    this->receptedBatteryData = nullptr;
    delete this->batteryStatus;
    this->batteryStatus = nullptr;

    delete[] this->currentCellInformation->timestamp;
    this->currentCellInformation->timestamp = nullptr;
    delete[] this->currentCellInformation->band;
    this->currentCellInformation->band = nullptr;
    delete this->currentCellInformation;
    this->currentCellInformation = nullptr;
    delete this->gatewayGNSSdata;
    this->gatewayGNSSdata = nullptr;

    delete  this->receptedGNSSdata;
    this->receptedGNSSdata = nullptr;

    delete [] this->serverTargetted->secondaryUrl;
    this->serverTargetted->secondaryUrl = nullptr;
    delete [] this->serverTargetted->url;
    this->serverTargetted->url = nullptr;
    delete this->serverTargetted;
    this->serverTargetted = nullptr;

    delete this->currentGNSSModule;
    this->currentGNSSModule = nullptr;
    delete this->cellularTransceiver;
    this->cellularTransceiver = nullptr;


    delete this->LoRaTransciever;
    this->LoRaTransciever  = nullptr;
    delete this->timeout;
    this->timeout = nullptr;

    //delete this->encrypter;
    //this->encrypter = nullptr;
    delete this->encrypterBase64;
    this->encrypterBase64 = nullptr;
    delete this->authgen;
    this->authgen = nullptr;
    delete this->ckgen;
    this->ckgen = nullptr;
    delete this->checksumVerifier;
    this->checksumVerifier = nullptr;
    delete this->authVer;
    this->authVer = nullptr;
    //delete this->decrypter;
    //this->decrypter = nullptr;
    delete this->decrypterBase64;
    this->decrypterBase64 = nullptr;

    delete this->jwt;
    this->jwt = nullptr;
}

void Gateway::update () {
    static char bufferMessage [MESSAGE_BUFFER_SIZE];
    //static char receivedMessage [MESSAGE_BUFFER_SIZE];

    static int numberOfNeighbors = 0;
    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
    //this->currentState->awake(this->cellularTransceiver, this->latency, this->silentKeepAliveTimer);
    this->currentState->waitForMessage (this->LoRaTransciever, bufferMessage, this->timeout);
    this->currentState->sendAcknowledgement (this->LoRaTransciever,  bufferMessage, this->timeout);
    this->currentState->updatePowerStatus (this->cellularTransceiver, this->batteryStatus);
    this->currentState->obtainGNSSPosition (this->currentGNSSModule, this->gatewayGNSSdata);
    this->currentState->connectToMobileNetwork (this->cellularTransceiver,
    this->currentCellInformation);
    this->currentState->formatMessage (bufferMessage, this->IMEIRecepted,
    this->receptedTrackerEvent, this->RSSI, this->gatewayGNSSdata, this->receptedGNSSdata,
    this->receptedImuData, this->receptedBatteryData); 
    this->currentState->exchangeMessages (this->cellularTransceiver,
    bufferMessage, this->serverTargetted, bufferMessage );

    
    this->currentState->connectEthernetToLocalNetwork (this->ethernetModule, this->timeout);
    this->currentState->queryUTCTimeViaRemoteServer (this->ethernetModule, this->timeout);
    this->currentState->exchangeMessagesThroughEthernet (this->ethernetModule, this->timeout, bufferMessage);

    //this->currentState->goToSleep (this->cellularTransceiver);
    watchdog.kick();
    
}


void Gateway::changeState  (GatewayState * newGatewayState) {
    delete this->currentState;
    this->currentState = newGatewayState;
}

void Gateway::setMovementEvent (char * movementEventString) {
    if (strcmp (movementEventString,"MOVE") == 0) {
        this->currentMovementEvent = MOVING;
        return;
    }
    if (strcmp (movementEventString,"PARK") == 0) {
        this->currentMovementEvent = PARKING;
        return;
    }
    if (strcmp (movementEventString,"STOP") == 0) {
        this->currentMovementEvent = STOPPED;
        return;
    }
    if (strcmp (movementEventString,"MVRS") == 0) {
        this->currentMovementEvent = MOVEMENT_RESTARTED;
        return;
    }
}

ReceptedTypeMessage_t Gateway::getReceptedTypeMessage () {
    return this->receptedTypeMessage;
}


void Gateway::updateMovementEvent () {
    char buffer[100];
    MovementEvent_t newMovementEvent;

    if (this->newMotionStatus == DEVICE_ON_MOTION && this->currentMotionStatus == DEVICE_STATIONARY) {
        newMovementEvent = MOVEMENT_RESTARTED;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: MOVEMENT_RESTARTED\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_STATIONARY && this->currentMotionStatus == DEVICE_ON_MOTION) {
        newMovementEvent = PARKING;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: PARKING\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_ON_MOTION && this->currentMotionStatus == DEVICE_ON_MOTION) {
        newMovementEvent = MOVING;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: MOVING\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }
    else if (this->newMotionStatus == DEVICE_STATIONARY && this->currentMotionStatus == DEVICE_STATIONARY) {
        newMovementEvent = STOPPED;
        if (newMovementEvent != this->currentMovementEvent) {
            snprintf(buffer, sizeof(buffer), "\n\rUpdate movement event: STOPPED\n\r");
            uartUSB.write(buffer, strlen(buffer));
        }
        this->currentMovementEvent = newMovementEvent;
    }

    this->currentMotionStatus = this->newMotionStatus;
}

bool Gateway::parseReceptedLoRaMessage(char * messageToParse) {
    char prefix[16];
    char trackerEvent[20];
    char timestamp[20];
    char logMessage[512];
    int messageNumber;

    // Detectar prefix antes de sscanf
    if (sscanf(messageToParse, "%15[^,]", prefix) != 1) {
        uartUSB.write("Error: no prefix found\r\n", strlen("Error: no prefix found\r\n"));
        return false;
    }

    if (strcmp(prefix, "LORAGNSS") == 0) {

        int tmpStatus, tmpBat, tmpCharge;

        if (sscanf(messageToParse,
                   "LORAGNSS,%lld,%d,%19[^,],%f,%f,%f,%f,%f,%f,%19[^,],%d,%d,%d,%f,%f,%f,%f,%f,%f",
                    &this->IMEIRecepted,
                    &this->loraMessageNumber,
                    trackerEvent,
                    &this->receptedGNSSdata->latitude,
                    &this->receptedGNSSdata->longitude, 
                    &this->receptedGNSSdata->hdop, 
                    &this->receptedGNSSdata->altitude, 
                    &this->receptedGNSSdata->cog, 
                    &this->receptedGNSSdata->spkm,
                    timestamp,
                    &tmpBat,
                    &tmpCharge,
                    &tmpStatus,
                    &this->receptedImuData->acceleration.ax, 
                    &this->receptedImuData->acceleration.ay, 
                    &this->receptedImuData->acceleration.az, 
                    &this->receptedImuData->angles.yaw, 
                    &this->receptedImuData->angles.roll,
                    &this->receptedImuData->angles.pitch) == 19) {

            this->receptedBatteryData->batteryChargeStatus = (uint8_t)tmpBat;
            this->receptedBatteryData->chargeLevel = (uint8_t)tmpCharge;
            this->receptedImuData->status = (uint8_t)tmpStatus;

            strcpy(this->receptedGNSSdata->timestamp, timestamp);   
            strcpy(this->receptedTrackerEvent, trackerEvent);  

            snprintf(logMessage, sizeof(logMessage),
                     "GNSS Msg: IMEI=%lld, MsgNum=%d, Event=%s, "
                     "Lat=%.6f, Lon=%.6f, HDOP=%.2f, Alt=%.2f, "
                     "COG=%.2f, V=%.2f, Time=%s, Bat=%d, Charge=%d, "
                     "IMUStatus=%d, Acc=[%.2f,%.2f,%.2f], "
                     "Ang=[%.2f,%.2f,%.2f]\r\n",
                     this->IMEIRecepted, this->loraMessageNumber, 
                     this->receptedTrackerEvent,
                     this->receptedGNSSdata->latitude, 
                     this->receptedGNSSdata->longitude, 
                     this->receptedGNSSdata->hdop,
                     this->receptedGNSSdata->altitude, 
                     this->receptedGNSSdata->cog, 
                     this->receptedGNSSdata->spkm,
                     this->receptedGNSSdata->timestamp,
                     this->receptedBatteryData->batteryChargeStatus,
                     this->receptedBatteryData->chargeLevel,
                     this->receptedImuData->status,
                     this->receptedImuData->acceleration.ax, 
                     this->receptedImuData->acceleration.ay,
                     this->receptedImuData->acceleration.az, 
                     this->receptedImuData->angles.yaw, 
                     this->receptedImuData->angles.roll,
                     this->receptedImuData->angles.pitch);
            this->receptedTypeMessage = LORAGNSS;
            uartUSB.write(logMessage, strlen(logMessage));
            return true;
        }
    }
    else if (strcmp(prefix, "LORALORA") == 0) {

        int tmpStatus, tmpBat, tmpCharge;

        if (sscanf(messageToParse,
                   "LORALORA,%lld,%d,%19[^,],%d,%d,%19[^,],%d,%f,%f,%f,%f,%f,%f",
                    &this->IMEIRecepted,
                    &this->loraMessageNumber,
                    trackerEvent,
                    &tmpBat,
                    &tmpCharge,
                    timestamp,
                    &tmpStatus,
                    &this->receptedImuData->acceleration.ax, 
                    &this->receptedImuData->acceleration.ay, 
                    &this->receptedImuData->acceleration.az, 
                    &this->receptedImuData->angles.yaw, 
                    &this->receptedImuData->angles.roll,
                    &this->receptedImuData->angles.pitch) == 13) {

            this->receptedBatteryData->batteryChargeStatus = (uint8_t)tmpBat;
            this->receptedBatteryData->chargeLevel = (uint8_t)tmpCharge;
            this->receptedImuData->status    = (uint8_t)tmpStatus;
            strcpy(this->receptedTrackerEvent, trackerEvent);       
            strcpy(this->receptedImuData->timestamp, timestamp);   

            snprintf(logMessage, sizeof(logMessage),
                     "LORA Msg: IMEI=%lld, MsgNum=%d, Event=%s, "
                     "Bat=%d, Charge=%d, IMU=%d, "
                     "Acc=[%.2f,%.2f,%.2f], "
                     "Ang=[%.2f,%.2f,%.2f]"
                     "TIME=%s\r\n",
                     this->IMEIRecepted, 
                     this->loraMessageNumber, 
                     this->receptedTrackerEvent,
                     this->receptedBatteryData->batteryChargeStatus,
                     this->receptedBatteryData->chargeLevel,
                     this->receptedImuData->status, 
                     this->receptedImuData->acceleration.ax, 
                     this->receptedImuData->acceleration.ay, 
                     this->receptedImuData->acceleration.az, 
                     this->receptedImuData->angles.yaw, 
                     this->receptedImuData->angles.roll, 
                     this->receptedImuData->angles.pitch,
                     this->receptedImuData->timestamp);
            this->receptedTypeMessage = LORALORA;
            uartUSB.write(logMessage, strlen(logMessage));
            return true;
        }
    }
    else {
        uartUSB.write("Error: unknown prefix\r\n", strlen("Error: unknown prefix\r\n"));
        return false;
    }

    uartUSB.write("Error parsing message.\r\n", strlen("Error parsing message.\r\n"));
    return false;
}


bool Gateway::encryptMessage (char * message, unsigned int messageSize) {
    this->encrypterBase64->setNextHandler(nullptr);
    if (this->encrypterBase64->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Gateway::decryptMessage (char * message, unsigned int messageSize) {
    this->decrypterBase64->setNextHandler(nullptr);
    if (this->decrypterBase64->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Gateway::prepareLoRaMessage (char * message, unsigned int messageSize) {
    this->encrypterBase64->setNextHandler(this->authgen)->setNextHandler(this->ckgen);
    if (this->encrypterBase64->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

bool Gateway::processLoRaMessage (char * message, unsigned int messageSize) {
    this->checksumVerifier->setNextHandler(this->authVer)->setNextHandler(this->decrypterBase64);
    if (this->checksumVerifier->handleMessage (message, messageSize) == MESSAGE_HANDLER_STATUS_PROCESSED) {
        return true;
    } else {
        return false;
    }
}

long long int Gateway::getReceptedIMEI () {
    return this->IMEIRecepted;
}

int Gateway::getLoraMessageNumber () {
    return this->loraMessageNumber;
}

void Gateway::setLoRaMessageNumber (int messageNumber) {
    this->loraMessageNumber = messageNumber;
}


void Gateway::setCurrentRSSI (int newRSSI) {
    this->RSSI = newRSSI;
}

 
void Gateway::getUrlPathMainChannel ( char * urlPathChannel) {
    strcpy (urlPathChannel, this->serverTargetted->url);
}

void Gateway::getUrlPathSecondaryChannel ( char * urlPathChannel) {
    strcpy (urlPathChannel, this->serverTargetted->secondaryUrl);
}
 
char* Gateway::getDeviceIdentifier ( ) {
    return this->deviceIdentifier;
}

void Gateway::getDeviceIdentifier (char * deviceId) {
    strcpy (deviceId, this->deviceIdentifier );
}

int Gateway::getSequenceNumber () {
    return this->sequenceMessageNumber;
}


void Gateway::increaseSequenceNumber () {
    this->sequenceMessageNumber ++;
}

void Gateway::progressOnHashChain () {
    strcpy (this->prevChainHash, this->currChainHash);
}

void Gateway::setCurrentHashChain (char * hashChain) {
    strcpy (this->currChainHash, hashChain );
}

char* Gateway::getPrevHashChain () {
    return this->prevChainHash;
}

void Gateway::getPrevHashChain (char * hashChain) {
    strcpy (hashChain, this->prevChainHash);
}


void Gateway::encodeJWT(char * payloadToJWT, char * jwtEncoded)  {
    this->jwt->encodeJWT (payloadToJWT, jwtEncoded);
}

bool Gateway::decodeJWT(char * jwtToDecode, char * payloadRetrived) {
    return this->jwt->decodeJWT(jwtToDecode, payloadRetrived);
}




//=====[Implementations of private methods]==================================