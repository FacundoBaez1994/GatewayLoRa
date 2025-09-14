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

#define URL_PATH_CHANNEL "https://intent-lion-loudly.ngrok-free.app/api/canal/envio"
#define CURRENT_DEVICE_IDENTIFIER "device/gateway-001"


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public methods]===================================
Gateway::Gateway () {
    this->urlPathChannel = new char [100]; 
    strcpy (this->urlPathChannel, URL_PATH_CHANNEL);
    this->deviceIdentifier = new char [100];
    strcpy (this->deviceIdentifier, CURRENT_DEVICE_IDENTIFIER);
    this->prevChainHash = new char [100];
    strcpy (this->prevChainHash, "-");
    this->currChainHash = new char [100];
    strcpy (this->currChainHash, "-");

    Watchdog &watchdog = Watchdog::get_instance(); // singletom
    watchdog.start(TIMEOUT_WATCHDOG_TIMER_MS);
    char StringToSendUSB [50] = "Gateway initialization";
    uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only

    this->currentOperationMode = NORMAL_OPERATION_MODE;
    //this->currentOperationMode = PERSUIT_OPERATION_MODE;
    this->latency = new NonBlockingDelay (EXTREMELY_LOW_LATENCY_MS);
    this->silentKeepAliveTimer = new NonBlockingDelay (HOUR_MS);
    this->cellularTransceiver = new CellularModule ( );
    this->currentGNSSModule = new GNSSModule (this->cellularTransceiver->getPowerManager()
    , this->cellularTransceiver->getATHandler());
    //both share the same power manager and ATHandler (uart)

    this->socketTargetted = new TcpSocket;
    this->socketTargetted->IpDirection = new char[16]; // 
    strcpy(this->socketTargetted->IpDirection, "186.19.62.251");
    this->socketTargetted->TcpPort = 123;

    this->currentCellInformation = new CellInformation;
    this->currentCellInformation->timestamp  = new char [20];
    this->currentCellInformation->band = new char [20];

    this->gatewayGNSSdata = new GNSSData;
    this->gatewayGNSSdata->latitude = 0;
    this->gatewayGNSSdata->longitude = 0; 
    this->gatewayGNSSdata->altitude = 0;
    this->receptedGNSSdata = new GNSSData;
    this->batteryStatus = new BatteryData;
    this->receptedBatteryData = new BatteryData;

    this->receptedImuData = new IMUData_t;
    this->receptedImuData->timestamp = new char [20];
    this->receptedImuData->timeBetweenSamples = TIME_BETWEEN_IMU_SAMPLES;

    this->currentState =  new SensingBatteryStatus (this); //WaitingForMessage
    //this->currentState =  new WaitingForMessage (this);


    this->LoRaTransciever = new LoRaClass ();
    this->timeout = new NonBlockingDelay (STANDARD_TIMEOUT);

     //this->encrypter = new Encrypter ();
    this->encrypterBase64 = new EncrypterBase64 ();
    this->authgen = new AuthenticationGenerator ();
    this->ckgen = new ChecksumGenerator ();
    

    this->checksumVerifier = new ChecksumVerifier ();
    this->authVer = new AuthenticationVerifier ();
     //this->decrypter = new Decrypter ();
    this->decrypterBase64 = new DecrypterBase64 ();
}

Gateway::~Gateway() {
    delete [] this->urlPathChannel; 
    this->urlPathChannel  = nullptr;
    delete [] this->deviceIdentifier;
    this->deviceIdentifier  = nullptr;
    delete [] this->prevChainHash;
    this->prevChainHash = nullptr;
    delete []  this->currChainHash;
    this->currChainHash = nullptr;

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

    delete[] this->socketTargetted->IpDirection; // Libera la memoria asignada a IpDirection
    this->socketTargetted->IpDirection = nullptr;
    delete this->socketTargetted; // Libera la memoria asignada al socketTargetted
    this->socketTargetted = nullptr;
    delete this->latency;
    this->latency = nullptr; 
    delete this->silentKeepAliveTimer;
    this->silentKeepAliveTimer = nullptr;
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
}

void Gateway::update () {
    static char formattedMessage [2048];
    static char receivedMessage [2048];

    static int numberOfNeighbors = 0;
    Watchdog &watchdog = Watchdog::get_instance(); // singleton
    watchdog.kick();
    this->currentState->awake(this->cellularTransceiver, this->latency, this->silentKeepAliveTimer);
    this->currentState->waitForMessage (this->LoRaTransciever, receivedMessage, this->timeout);
    this->currentState->sendAcknowledgement (this->LoRaTransciever,  formattedMessage, this->timeout);
    this->currentState->updatePowerStatus (this->cellularTransceiver, this->batteryStatus);
    this->currentState->obtainGNSSPosition (this->currentGNSSModule, this->gatewayGNSSdata);
    this->currentState->connectToMobileNetwork (this->cellularTransceiver,
    this->currentCellInformation);
    this->currentState->formatMessage (formattedMessage, this->IMEIRecepted,
    this->receptedTrackerEvent, this->RSSI, this->gatewayGNSSdata, this->receptedGNSSdata,
    this->receptedImuData, this->receptedBatteryData); 
    this->currentState->exchangeMessages (this->cellularTransceiver,
    formattedMessage, this->socketTargetted, receivedMessage );
    // this->currentState->exchangeMessages (this->LoRaTransciever, formattedMessage, receivedMessage);
    this->currentState->goToSleep (this->cellularTransceiver);
    watchdog.kick();
    
}


void Gateway::changeState  (GatewayState * newGatewayState) {
    delete this->currentState;
    this->currentState = newGatewayState;
}


void Gateway::getMovementEvent (char * movementEventString) {
    if (this->currentMovementEvent == MOVING) {
        strcpy (movementEventString, "MOVE");
    }
    if (this->currentMovementEvent == PARKING) {
        strcpy (movementEventString, "PARK");
    }
    if (this->currentMovementEvent == STOPPED) {
        strcpy (movementEventString, "STOP");
    }
    if (this->currentMovementEvent == MOVEMENT_RESTARTED) {
        strcpy (movementEventString, "MVRS");
    }
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


MovementEvent_t Gateway::getMovementEvent () {
    return this->currentMovementEvent;
}

ReceptedTypeMessage_t Gateway::getReceptedTypeMessage () {
    return this->receptedTypeMessage;
}

OperationMode_t  Gateway::getOperationMode () {
    return this->currentOperationMode;
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

void Gateway::setOperationMode(OperationMode_t newOperationMode) {
    this->currentOperationMode = newOperationMode;
}

void Gateway::setSilentTimer (int hours) {
    this->silentKeepAliveTimer->write(hours * HOUR_MS);
}

void Gateway::setLatency(LatencyLevel_t level) {
    tick_t newLatency = EXTREMELY_LOW_LATENCY_MS;

    switch (level) {
        case EXTREMELY_LOW_LATENCY:
            this->latencyLevel = EXTREMELY_LOW_LATENCY;
            newLatency = EXTREMELY_LOW_LATENCY_MS;
            break;
        case VERY_LOW_LATENCY:
            this->latencyLevel = VERY_LOW_LATENCY;
            newLatency = VERY_LOW_LATENCY_MS;
            break;
        case LOW_LATENCY:
            this->latencyLevel = LOW_LATENCY;
            newLatency = LOW_LATENCY_MS;
            break;
        case MEDIUM_LATENCY:
            this->latencyLevel = MEDIUM_LATENCY;
            newLatency = MEDIUM_LATENCY_MS;
            break;
        case HIGH_LATENCY:
            this->latencyLevel = HIGH_LATENCY;
            newLatency = HIGH_LATENCY_MS;
            break;
        case VERY_HIGH_LATENCY:
            this->latencyLevel = VERY_HIGH_LATENCY;
            newLatency = VERY_HIGH_LATENCY_MS;
            break;
        case EXTREMELY_HIGH_LATENCY:
            this->latencyLevel = EXTREMELY_HIGH_LATENCY;
            newLatency = EXTREMELY_HIGH_LATENCY_MS;
            break;
        default:
            break;
    }

    this->latency->write(newLatency);
    this->latency->restart();

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "\n\rNew latency set: %llu ms\n\r", newLatency);
    uartUSB.write(buffer, strlen(buffer));
}



void Gateway::actualizeKeepAliveLatency () {
tick_t newKeepAliveLatency = EXTREMELY_LOW_LATENCY_MS;

    switch (this->latencyLevel) {
        case EXTREMELY_LOW_LATENCY:
            newKeepAliveLatency = EXTREMELY_LOW_LATENCY_MS * EXTREMELY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER ;
            break;
        case VERY_LOW_LATENCY:
            newKeepAliveLatency = VERY_LOW_LATENCY_MS * VERY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER ;
            break;
        case LOW_LATENCY:
            newKeepAliveLatency = LOW_LATENCY_MS * VERY_LOW_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case MEDIUM_LATENCY:
            newKeepAliveLatency = MEDIUM_LATENCY_MS * MEDIUM_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case HIGH_LATENCY:
            newKeepAliveLatency = HIGH_LATENCY_MS * HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case VERY_HIGH_LATENCY:
            newKeepAliveLatency = VERY_HIGH_LATENCY_MS * VERY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        case EXTREMELY_HIGH_LATENCY:
            newKeepAliveLatency = EXTREMELY_HIGH_LATENCY_MS * EXTREMELY_HIGH_LATENCY_KEEP_ALIVE_MULTIPLIER;
            break;
        default:
            break;
    }

    this->silentKeepAliveTimer->write(newKeepAliveLatency);
    this->silentKeepAliveTimer->restart();

    char buffer[100];
    snprintf(buffer, sizeof(buffer), "\n\rNew keep alive latency set: %llu ms\n\r", newKeepAliveLatency);
    uartUSB.write(buffer, strlen(buffer));
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


void Gateway::setCurrentRSSI (int RSSI) {
    this->RSSI = RSSI;
}

 
void Gateway::getUrlPathChannel ( char * urlPathChannel) {
    strcpy (urlPathChannel, this->urlPathChannel);
}

 
void Gateway::getDeviceIdentifier ( char * deviceId) {
    strcpy (deviceId, this->deviceIdentifier);
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


void Gateway::getPrevHashChain (char * hashChain) {
    strcpy (hashChain, this->prevChainHash);
}






//=====[Implementations of private methods]==================================