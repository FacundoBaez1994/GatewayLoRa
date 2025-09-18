//=====[#include guards - begin]===============================================

#ifndef _SENDING_ACK_H_
#define _SENDING_ACK_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "GatewayBaseState.h"
//#include "WaitingAcknowledgement.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class SendingAck : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    SendingAck  (Gateway * gateway, long long int deviceId, int messageNumber);
    SendingAck (Gateway * gateway, long long int deviceId, int messageNumber,  gatewayStatus_t gatewayStatus);
    virtual ~SendingAck  ();
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    virtual bool waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);
private:
    Gateway * currentGateway;
    long long int IdDevice;
    int messageNumber;
    gatewayStatus_t currentStatus; ///< Current gateway status

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_ACK_H_