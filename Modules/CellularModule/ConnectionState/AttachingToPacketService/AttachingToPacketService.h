//=====[#include guards - begin]===============================================

#ifndef _ATTACHING_TO_PACKET_SERVICE_H_
#define _ATTACHING_TO_PACKET_SERVICE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "DefinePDPContext.h"
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct CellInformation;


//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class AttachingToPacketService : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    AttachingToPacketService ();
    AttachingToPacketService (CellularModule * mobileModule);
    virtual ~AttachingToPacketService ();
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
    virtual void enableConnection ();

    
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

#endif // _ATTACHING_TO_PACKET_SERVICE_H_