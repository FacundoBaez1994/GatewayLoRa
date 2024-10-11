//=====[#include guards - begin]===============================================

#ifndef _RETRIEVING_TIME_AND_DATE_H_
#define _RETRIEVING_TIME_AND_DATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "AttachingToPacketService.h"
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
class RetrievingTimeAndDate : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    RetrievingTimeAndDate();
    RetrievingTimeAndDate (CellularModule * mobileModule);
    virtual ~RetrievingTimeAndDate();
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
    bool timeAndDateRetrived;
    char time [20];
    char date [20]; 
    int connectionAttempts; 
    int maxConnectionAttempts; 
//=====[Declaration of privates methods]=========================================
    bool retrieveNetworkTime (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _RETRIEVING_TIME_AND_DATE_H_