//=====[#include guards - begin]===============================================

#ifndef _SENDING_MESSAGE_THROUGH_ETHERNET_H_
#define _SENDING_MESSAGE_THROUGH_ETHERNET_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "GatewayBaseState.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class SendingMessageThroughEthernet : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    SendingMessageThroughEthernet (Gateway * gateway, gatewayStatus_t gatewayStatus);
    virtual ~SendingMessageThroughEthernet ();
    virtual void exchangeMessagesThroughEthernet (UipEthernet * ethernetModule, NonBlockingDelay * delay, char * payload);
private:
    void disconnect (UipEthernet * ethernetModule, TcpClient * socket);
    Gateway * gateway;
    gatewayStatus_t currentStatus;
    int connectionRetries;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_MESSAGE_THROUGH_ETHERNET_H_