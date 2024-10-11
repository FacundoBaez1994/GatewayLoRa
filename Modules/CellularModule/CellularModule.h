//=====[#include guards - begin]===============================================

#ifndef _CELLULAR_MODULE_H_
#define _CELLULAR_MODULE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "TransceiverState.h"
#include "ActivatePDP.h"
#include "PowerManager.h"
#include "TransceiverUnavailable.h"
#include "ConnectionUnavailableState.h"
#include "PowerOFFState.h"
#include "PowerState.h"

//=====[Declaration of public data types]======================================
class ConnectionState; //debido a declaracion adelantada
struct BatteryData;

 struct TcpSocket {
    char * IpDirection; // IPV4
    int TcpPort;        // 
 };

  struct CellInformation {
    float signalLevel;
    long long int IMEI;
    int accessTechnology; //
    int registrationStatus; // pass as int in order to decrese data lenght (translate in server)
    int channel;
    char *  date;
    char * time;
    char * band;
    int tech; // 2 = 2G, 3 = 3G, 4 = 4G
    // Cell identifiers codes ////
    int lac;
    int cellId; 
    int  mcc; // with mnc identifies operator
    int mnc; // 
    //////////////////////////////
 };


//=====[Declaration of public classes]=========================================
/*
 *  
 * 
 */
class CellularModule {
public:

//=====[Declaration of public methods]=========================================
    CellularModule ( );
    virtual ~CellularModule ();
    void startStopUpdate ();
    void reboot ();
    void awake ();
    bool goToSleep ();
    bool measureBattery (BatteryData * currentBatteryData);
    CellularConnectionStatus_t connectToMobileNetwork (CellInformation * currentCellInformation);
    CellularTransceiverStatus_t exchangeMessages (char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
    void changeConnectionState  (ConnectionState * newConnectionState);
    bool retrivNeighborCellsInformation ( std::vector<CellInformation*> &neighborsCellInformation
    , int numberOfNeighbors);
    void enableTransceiver ();
    void enableConnection ();
    void changeTransceiverState  (TransceiverState * newTransceiverState);
    void switchSIMCARD ();

    PowerManager* getPowerManager ();
    BufferedSerial* getUART ();
    ATCommandHandler* getATHandler ();
private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* refreshTime;
    ATCommandHandler* ATHandler;
    ConnectionState* currentConnectionState;
    TransceiverState* currentTransceiverState;
    PowerManager * modulePowerManager;

    DigitalOut* simCardSwitchOutput; // Switch between Simcards
    bool watingForResponse;
    powerStatus_t currentPowerStatus;


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CELLULAR_MODULE_H_