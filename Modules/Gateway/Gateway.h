//=====[#include guards - begin]===============================================

#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include "mbed.h"
#include "GatewayState.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "LoRa.h"
#include "UipEthernet.h"
#include "WaitingForMessage.h"



//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS tracker
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class Gateway {
public:
    Gateway ();
    virtual ~Gateway ();
    void update();
    void changeState  (GatewayState * newState);

private:

    void LoRa_rxMode();
    void LoRa_txMode();

    // CellularModule* cellularTransceiver;
    LoRaClass * LoRaTransciever;
    UipEthernet * ethernetModule;
    DigitalOut * resetEth;
    GatewayState * currentState;

    NonBlockingDelay * timer;
   // BatteryData  * batteryStatus;

};


//=====[#include guards - end]=================================================

#endif // _GATEWAY_H_
