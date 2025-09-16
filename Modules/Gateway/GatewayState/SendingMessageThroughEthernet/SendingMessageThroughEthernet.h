//=====[#include guards - begin]===============================================

#ifndef _SENDING_MESSAGE_THROUGH_ETHERNET_H_
#define _SENDING_MESSAGE_THROUGH_ETHERNET_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayState.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class SendingMessageThroughEthernet : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    SendingMessageThroughEthernet (Gateway * gateway);
    SendingMessageThroughEthernet (Gateway * gateway, int IdDevice, int messageNumber, char * payload);
    virtual ~SendingMessageThroughEthernet ();
    virtual void connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay);
    virtual void queryUTCTimeViaRemoteServer (UipEthernet * ethernetModule, NonBlockingDelay * delay);
    virtual void receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    void disconnect (UipEthernet * ethernetModule, TcpClient * socket);
    Gateway * gateway;
    int IdDevice;
    int messageNumber;
    int connectionRetries;
    char payload [2248];
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_MESSAGE_THROUGH_ETHERNET_H_