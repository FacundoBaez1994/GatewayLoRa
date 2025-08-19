//=====[#include guards - begin]===============================================

#ifndef _SENDING_MESSAGE_H_
#define _SENDING_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "RFTransicieverState.h"
//#include "WaitingAcknowledgement.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class SendingAck : public RFTransicieverState {
public:
//=====[Declaration of public methods]=========================================
    SendingAck  (Gateway * gateway, long long int deviceId, int messageNumber);
    virtual ~SendingAck  ();
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    virtual bool waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);
private:
    Gateway * currentGateway;
    long long int IdDevice;
    int messageNumber;
    //int connectionRetries;
    // char payload [50];
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_MESSAGE_H_