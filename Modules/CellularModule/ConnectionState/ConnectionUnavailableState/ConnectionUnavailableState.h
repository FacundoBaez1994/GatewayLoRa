//=====[#include guards - begin]===============================================

#ifndef _CONNECTION_UNAVAILABLE_STATE_H_
#define _CONNECTION_UNAVAILABLE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "IdleState.h"
#include "Non_Blocking_Delay.h"
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConnectionUnavailableState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConnectionUnavailableState();
    ConnectionUnavailableState (CellularModule * mobileModule);
    virtual ~ConnectionUnavailableState();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler,
     NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
    virtual void enableConnection ();
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool enableTransceiver;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CONNECTION_UNAVAILABLE_STATE_H_