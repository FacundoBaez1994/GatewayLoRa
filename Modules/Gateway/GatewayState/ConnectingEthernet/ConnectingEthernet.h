//=====[#include guards - begin]===============================================

#ifndef _CONNECTING_ETHERNET_H_
#define _CONNECTING_ETHERNET_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class ConnectingEthernet : public GatewayBaseState  {
public:
//=====[Declaration of public methods]=========================================
    ConnectingEthernet (Gateway * gateway, gatewayStatus_t gatewayStatus);
    virtual ~ConnectingEthernet ();
    virtual void connectEthernetToLocalNetwork (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    Gateway * gateway;
    int connectionRetries;
    char payload [250];
    gatewayStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONNECTING_ETHERNET_H_