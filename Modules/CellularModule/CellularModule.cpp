//=====[Libraries]=============================================================
#include "CellularModule.h"
#include "Debugger.h"


//=====[Declaration of private defines]========================================
#define REFRESHTIME  1000
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
/** 
* @brief r
*/


//=====[Implementations of public methods]===================================
/** 
* 
* 
* 
*/
CellularModule::CellularModule () {
    this->refreshTime = new NonBlockingDelay (REFRESHTIME);
    this->ATHandler = new ATCommandHandler (new BufferedSerial  (CELLULAR_MODULE_TX_UART, 
     CELLULAR_MODULE_RX_UART, CELLULAR_MODULE_BAUD_RATE));
    this->currentConnectionState = new ConnectionUnavailableState (this);
    this->currentTransceiverState = new TransceiverUnavailable (this);
    this->modulePowerManager = new PowerManager (this->ATHandler);

    this->simCardSwitchOutput =  new DigitalOut (CELLULAR_MODULE_SIMCARD_SWITCH_OUTPUT);
    this->simCardSwitchOutput->write(ON);
    this->currentPowerStatus = POWER_OFF;
}


/** 
* 
* 
* 
*/
CellularModule::~CellularModule () {
}


/** 
* 
* 
* 
*/
void CellularModule::startStopUpdate () {
    powerStatus_t newPowerStatus;
    newPowerStatus = this->modulePowerManager->startStopUpdate();
    if (this->currentPowerStatus != newPowerStatus) {
       this->currentPowerStatus = newPowerStatus;
       if (this->currentPowerStatus != POWER_ON) {
            this->changeConnectionState (new ConnectionUnavailableState (this));
            this->changeTransceiverState  (new TransceiverUnavailable (this));
       }
    }
}

/** 
* 
* 
* 
*/
void CellularModule::awake () { 
    this->modulePowerManager->awake();
}

/** 
* 
* 
* 
*/
bool CellularModule::goToSleep () {
    return this->modulePowerManager->goToSleep();
}

/** 
* 
* 
*
*/
bool CellularModule::reboot () {
    return this->modulePowerManager->reboot ();
}

/** 
* 
* 
* 
*/
CellularConnectionStatus_t CellularModule::connectToMobileNetwork 
(CellInformation * currentCellInformation) {
    return this->currentConnectionState->connect(this->ATHandler, this->refreshTime, currentCellInformation);
}


/** 
* @brief 
* 
* @returns 
*/
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


/** 
* @brief 
* 
* 
* @returns 
*/
bool CellularModule::measureBattery (BatteryData * currentBatteryData) {
    return this->modulePowerManager->measureBattery(currentBatteryData);
}

/** 
* @brief 
* 
* @returns 
*/
void CellularModule::changeConnectionState  (ConnectionState * newConnectionState) {
    delete this->currentConnectionState;
    this->currentConnectionState = newConnectionState;
}
/** 
* @brief 
* 
* @returns 
*/
void CellularModule::changeTransceiverState  (TransceiverState * newTransceiverState) {
    delete this->currentTransceiverState;
    this->currentTransceiverState = newTransceiverState;
}

/** 
* @brief 
* 
* @returns 
*/
void CellularModule:: enableTransceiver () {
    this->currentTransceiverState->enableTransceiver();  
}

/** 
* @brief 
* 
* @returns 
*/
void CellularModule:: enableConnection () {
    this->currentConnectionState->enableConnection();
}



/** 
* @brief 
* 
* @returns 
*/
 BufferedSerial* CellularModule::getUART () {
     return this->ATHandler->getUART();
}

/** 
* @brief
* @note 
*/
void CellularModule::switchSIMCARD () {
    *this->simCardSwitchOutput = ! *this->simCardSwitchOutput;
}


bool CellularModule::retrivNeighborCellsInformation ( std::vector<CellInformation*> &neighborsCellInformation,
 int numberOfNeighbors){
        return this->currentConnectionState->retrivNeighborCellsInformation 
        (this->ATHandler, this->refreshTime, neighborsCellInformation, numberOfNeighbors);
    }

 PowerManager* CellularModule::getPowerManager (){
    return this->modulePowerManager;
}

ATCommandHandler* CellularModule::getATHandler (){
    return this->ATHandler;
}


//=====[Implementations of private functions]==================================

