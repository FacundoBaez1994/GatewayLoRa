//=====[#include guards - begin]===============================================

#ifndef _TRANSCEIVER_UNAVAILABLE_H_
#define _TRANSCEIVER_UNAVAILABLE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "ActivatePDP.h"
#include "Non_Blocking_Delay.h"


//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class TransceiverUnavailable : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    TransceiverUnavailable();
    TransceiverUnavailable (CellularModule * mobileModule);
    virtual ~TransceiverUnavailable ();
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
    virtual void enableTransceiver ();
private:
    CellularModule * mobileNetworkModule;
    bool readyToSend;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONNECTED_STATE_H_