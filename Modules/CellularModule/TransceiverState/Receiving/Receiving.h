//=====[#include guards - begin]===============================================

#ifndef _RECEIVING_H_
#define _RECEIVING_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "Sending.h"
#include "CloseSocket.h"

//=====[Declaration of public data types]======================================
class CellularModule; //debido a declaracion adelantada
struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class Receiving : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    Receiving();
    Receiving (CellularModule * mobileModule);
    virtual ~Receiving ();
    virtual void enableTransceiver ();
    virtual  CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
    bool checkResponse (char * response, char * retrivMessage);
    CellularModule * mobileNetworkModule;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _RECEIVING_H_