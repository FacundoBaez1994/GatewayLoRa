//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_IMEI_H_
#define _CONSULTING_IMEI_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingSIMCardStatus.h"
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
class ConsultingIMEI : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    ConsultingIMEI();
    ConsultingIMEI(CellularModule * mobileModule);
    virtual ~ConsultingIMEI ();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);
    virtual void enableConnection ();
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    bool ATFirstResponseRead;
    bool IMEIRetrived;
    long long int IMEI;
    int connectionAttempts; 
    int maxConnectionAttempts; 
//=====[Declaration of privates methods]=========================================
    bool RetrivIMEI (char *response, long long int &value);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_IMEI_H_