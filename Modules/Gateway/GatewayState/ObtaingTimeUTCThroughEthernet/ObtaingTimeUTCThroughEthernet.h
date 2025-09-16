//=====[#include guards - begin]===============================================

#ifndef _OBTAING_TIME_UTR_THROUGH_ETHENET_H_
#define _OBTAING_TIME_UTR_THROUGH_ETHENET_H_

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
class ObtaingTimeUTCThroughEthernet : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    ObtaingTimeUTCThroughEthernet (Gateway * gateway);
    virtual ~ObtaingTimeUTCThroughEthernet ();
    virtual void connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay);
    virtual void queryUTCTimeViaRemoteServer (UipEthernet * ethernetModule, NonBlockingDelay * delay);
    virtual void receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    Gateway * gateway;
    int connectionRetries;
    char payload [250];
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _OBTAING_TIME_UTR_THROUGH_ETHENET_H_