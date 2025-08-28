//=====[#include guards - begin]===============================================

#ifndef _WAITING_FOR_MESSAGE_H_
#define _WAITING_FOR_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Gateway.h"
#include "Non_Blocking_Delay.h"
#include "GatewayBaseState.h"
#include "SendingAck.h"
#include <algorithm> // Para std::find
#include <string>
#include <vector>
#include <cstring> 

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class WaitingForMessage : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    WaitingForMessage  (Gateway * gateway);
    virtual ~WaitingForMessage  ();
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    virtual bool waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);
private:
    Gateway * currentGateway;

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _WAITING_ACKNOWLEDGEMENT_H_