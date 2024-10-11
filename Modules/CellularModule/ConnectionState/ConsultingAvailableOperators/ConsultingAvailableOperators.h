//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_AVAILABLE_OPERATORS_H_
#define _CONSULTING_AVAILABLE_OPERATORS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "RetrievingTimeAndDate.h"
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
class ConsultingAvailableOperators : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingAvailableOperators();
    ConsultingAvailableOperators (CellularModule * mobileModule);
    virtual ~ConsultingAvailableOperators ();
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
    bool operatorsInformationRetrived;
    int mcc;
    int mnc;
    char * currentOperator;
    int channel;
    char band [20];
    int connectionAttempts; 
    int maxConnectionAttempts; 
//=====[Declaration of privates methods]=========================================
    bool retrivOperatorsInformation (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_AVAILABLE_OPERATORS_H_