//=====[Libraries]=============================================================
#include "CellularModule.h"
#include "Debugger.h"

//=====[Declaration of private defines]========================================
#define REFRESH_TIME  1000
#define CELLULAR_MODULE_TX_UART PA_9 // tx nucleo
#define CELLULAR_MODULE_RX_UART PA_10 // rx nucleo


#define CELLULAR_MODULE_BAUD_RATE 115200
#define CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT D7


//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================s

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================



//=====[Implementations of public methods]===================================

CellularModule::CellularModule () {
    this->refreshTime = new NonBlockingDelay (REFRESH_TIME);
    this->ATHandler = new ATCommandHandler (new BufferedSerial  (CELLULAR_MODULE_TX_UART, 
     CELLULAR_MODULE_RX_UART, CELLULAR_MODULE_BAUD_RATE));
    this->currentConnectionState = new ConnectionUnavailableState (this);
    this->currentTransceiverState = new TransceiverUnavailable (this);
    this->modulePowerManager = new PowerManager (this->ATHandler);

    this->simCardSwitchOutput =  new DigitalOut (CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT);
    this->currentPowerStatus = POWER_OFF;
}

CellularModule::~CellularModule () {
    delete this->refreshTime;
    this->refreshTime = nullptr;
    delete this->ATHandler;
    this->ATHandler = nullptr;
    delete this->currentConnectionState;
    this->currentConnectionState = nullptr;
    delete this->currentTransceiverState;
    this->currentTransceiverState = nullptr;
    delete this->modulePowerManager;
    this->modulePowerManager = nullptr;
    delete this->simCardSwitchOutput;
    this->simCardSwitchOutput = nullptr;
}

void CellularModule::startStopUpdate () {
    powerStatus_t newPowerStatus;
    newPowerStatus = this->modulePowerManager->startStopUpdate();
    if (this->currentPowerStatus != newPowerStatus) {
       this->currentPowerStatus = newPowerStatus;
       if (this->currentPowerStatus != POWER_ON) {
            //this->changeConnectionState (new ConnectionUnavailableState (this));
            //this->changeTransceiverState  (new TransceiverUnavailable (this));
       }
    }
}

bool CellularModule::turnOn () { 
    return this->modulePowerManager->turnOn();
}

bool CellularModule::turnOff () { 
    return this->modulePowerManager->turnOff();
}

void CellularModule::awake () { 
    this->modulePowerManager->awake();
}

bool CellularModule::goToSleep () {
    return this->modulePowerManager->goToSleep();
}

bool CellularModule::reboot () {
    return this->modulePowerManager->reboot ();
}

CellularConnectionStatus_t CellularModule::connectToMobileNetwork 
(CellInformation * currentCellInformation) {
    return this->currentConnectionState->connect(this->ATHandler, this->refreshTime, currentCellInformation);
}

CellularTransceiverStatus_t CellularModule::exchangeMessages (char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable) {
    CellularTransceiverStatus_t currentStatus = this->currentTransceiverState->exchangeMessages (this->ATHandler,
    this->refreshTime, message, socketTargetted, receivedMessage, newDataAvailable);
    if (currentStatus != CELLULAR_TRANSCEIVER_STATUS_TRYNING_TO_SEND &&
    currentStatus  != CELLULAR_TRANSCEIVER_STATUS_UNAVAIBLE) {
        this->changeConnectionState(new ConnectionUnavailableState (this));
        this->changeTransceiverState(new TransceiverUnavailable (this));
    }
    return currentStatus;
}

bool CellularModule::measureBattery (BatteryData * currentBatteryData) {
    return this->modulePowerManager->measureBattery(currentBatteryData);
}

void CellularModule::changeConnectionState  (ConnectionState * newConnectionState) {
    delete this->currentConnectionState;
    this->currentConnectionState = newConnectionState;
}

void CellularModule::changeTransceiverState  (TransceiverState * newTransceiverState) {
    delete this->currentTransceiverState;
    this->currentTransceiverState = newTransceiverState;
}

void CellularModule:: enableTransceiver () {
    this->currentTransceiverState->enableTransceiver();  
}

void CellularModule:: enableConnection () {
    this->currentConnectionState->enableConnection();
}

 BufferedSerial* CellularModule::getUART () {
     return this->ATHandler->getUART();
}

void CellularModule::switchSIMCARD () {
    *this->simCardSwitchOutput = ! *this->simCardSwitchOutput;
}


bool CellularModule::retrivNeighborCellsInformation ( std::vector<CellInformation*> &neighborsCellInformation,
 int numberOfNeighbors){
     bool result = this->currentConnectionState->retrivNeighborCellsInformation 
        (this->ATHandler, this->refreshTime, neighborsCellInformation, numberOfNeighbors);
    if (result == true) {
        refreshTime->write(REFRESH_TIME);
    }
    return result;
}

 PowerManager* CellularModule::getPowerManager (){
    return this->modulePowerManager;
}

ATCommandHandler* CellularModule::getATHandler (){
    return this->ATHandler;
}


//=====[Implementations of private functions]==================================