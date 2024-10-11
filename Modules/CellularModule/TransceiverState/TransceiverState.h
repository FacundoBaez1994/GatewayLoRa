//=====[#include guards - begin]===============================================

#ifndef _TRANSCEIVER_STATE_H_
#define _TRANSCEIVER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverStatus.h"

//=====[Declaration of public data types]======================================
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
    virtual void enableTransceiver ();
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_