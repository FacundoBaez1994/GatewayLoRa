//=====[#include guards - begin]===============================================

#ifndef _WAITING_FOR_MESSAGE_H_
#define _WAITING_FOR_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayState.h"
#include "Non_Blocking_Delay.h"
#include "SendingAck.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class WaitingForMessage : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    WaitingForMessage (Gateway * gateway);
    virtual ~WaitingForMessage ();
    virtual void receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    //bool checkResponse (char * response, char * retrivMessage);
    Gateway * gateway;
    char payload [50];
    int IdDeviceReceived;
    int messageNumberReceived;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _WAITING_FOR_MESSAGE_H_