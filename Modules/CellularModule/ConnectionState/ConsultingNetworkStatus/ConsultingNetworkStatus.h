//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_NETWORK_STATUS_H_
#define _CONSULTING_NETWORK_STATUS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingAvailableOperators.h"
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
class ConsultingNetworkStatus : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingNetworkStatus();
    ConsultingNetworkStatus(CellularModule * mobileModule);
    virtual ~ConsultingNetworkStatus ();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, 
    NonBlockingDelay * refreshTime,
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
    bool cellDataRetrived;
   // char lac[10];
   // char cellId[20];

    int lac;
    int cellId;

    int accessTechnology;
    int registrationStatus;
    int connectionAttempts; 
    int maxConnectionAttempts; 
//=====[Declaration of privates methods]=========================================
    bool retrivIdCellData (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_NETWORK_STATUS_H_