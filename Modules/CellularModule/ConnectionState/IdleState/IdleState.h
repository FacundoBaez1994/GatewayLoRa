//=====[#include guards - begin]===============================================

#ifndef _IDLE_STATE_H_
#define _IDLE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "CheckingSignalStrength.h"
#include "CellularModule.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class IdleState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    IdleState ();
    IdleState (CellularModule * mobileModule);
    virtual ~IdleState ();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);
    virtual void enableConnection ();
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    int connectionAttempts; 
    int maxConnectionAttempts; 
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_