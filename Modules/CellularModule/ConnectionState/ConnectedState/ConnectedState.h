//=====[#include guards - begin]===============================================

#ifndef _CONNECTED_STATE_H_
#define _CONNECTED_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "CellularModule.h"
#include <string>
#include <vector>
#include <cstring>  
//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConnectedState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConnectedState();
    ConnectedState (CellularModule * mobileModule);
    virtual ~ConnectedState ();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler,
     NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
    virtual void enableConnection ();
private:
    bool retrivOperatorsCodes (const char *response, int * mcc, int * mnc);
    bool retrivCellData (const char *response, int * tech, int * idCell, int * lac, float * prx);
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool enableTransceiver;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONNECTED_STATE_H_