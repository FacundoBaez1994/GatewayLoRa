//=====[#include guards - begin]===============================================

#ifndef _CHECKING_SIGNAL_STRENGTH_H_
#define _CHECKING_SIGNAL_STRENGTH_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingIMEI.h"
#include "Non_Blocking_Delay.h"
#include <string>
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class CheckingSignalStrength : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    CheckingSignalStrength ();
    CheckingSignalStrength(CellularModule * mobileModule);
    virtual ~CheckingSignalStrength ();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
    virtual void enableConnection ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool ATFirstResponseRead;
    bool signalLevelRetrived;
    float signalLevel;
    int connectionAttemptsATResponse; 
    int maxConnectionAttemptsATResponse; 
    int connectionAttemptsSignal; 
    int maxConnectionAttemptsSignal; 
//=====[Declaration of privates methods]=========================================
    bool checkExpectedResponse (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CHECKING_SIGNAL_STRENGTH_H_