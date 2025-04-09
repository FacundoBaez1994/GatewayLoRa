//=====[#include guards - begin]===============================================

#ifndef _SENDING_TCP_MESSAGE_H_
#define _SENDING_TCP_MESSAGE_H_

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
class SendingTCPMessage : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    SendingTCPMessage (Gateway * gateway, int IdDevice, int messageNumber, char * payload);
    virtual ~SendingTCPMessage ();
    virtual void receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    void disconnect (UipEthernet * ethernetModule, TcpClient * socket);
    Gateway * gateway;
    int IdDevice;
    int messageNumber;
    int connectionRetries;
    char payload [250];
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_TCP_MESSAGE_H_