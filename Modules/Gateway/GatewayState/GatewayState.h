//=====[#include guards - begin]===============================================

#ifndef _GATEWAY_STATE_H_
#define _GATEWAY_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "LoRa.h"
#include "UipEthernet.h"

//=====[Declaration of public data types]======================================
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class GatewayState {
public:
//=====[Declaration of public methods]=========================================
    virtual void receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATEWAY_STATE_H_