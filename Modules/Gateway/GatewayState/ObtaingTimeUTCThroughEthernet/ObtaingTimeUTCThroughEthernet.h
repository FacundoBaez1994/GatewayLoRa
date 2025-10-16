//=====[#include guards - begin]===============================================

#ifndef _OBTAING_TIME_UTR_THROUGH_ETHENET_H_
#define _OBTAING_TIME_UTR_THROUGH_ETHENET_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "GatewayBaseState.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================

class ObtaingTimeUTCThroughEthernet : public GatewayBaseState  {
public:
//=====[Declaration of public methods]=========================================
    ObtaingTimeUTCThroughEthernet (Gateway * gateway, gatewayStatus_t gatewayStatus);
    virtual ~ObtaingTimeUTCThroughEthernet ();
    virtual void queryUTCTimeViaRemoteServer (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    Gateway * gateway;
    int connectionRetries;
    gatewayStatus_t currentStatus;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _OBTAING_TIME_UTR_THROUGH_ETHENET_H_