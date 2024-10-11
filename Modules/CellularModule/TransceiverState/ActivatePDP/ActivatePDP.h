//=====[#include guards - begin]===============================================

#ifndef _ACTIVATE_PDP_H_
#define _ACTIVATE_PDP_H_

//==================[Libraries]===============================================

#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "CreateSocket.h"
#include "TransceiverUnavailable.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ActivatePDP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    ActivatePDP();
    ActivatePDP (CellularModule * mobileModule);
    virtual ~ActivatePDP ();
    virtual void enableTransceiver ();
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
    int Attempts; 
    int maxAttempts; 
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ACTIVATE_PDP_H_