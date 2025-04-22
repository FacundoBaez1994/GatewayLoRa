//=====[#include guards - begin]===============================================

#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "CellularModule.h"
#include "GNSSModule.h"
#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "string.h"
#include "GatewayState.h"
#include "GatewayStatus.h"
#include <CustomJWT.h>





//=====[Declaration of public defines]=========================================


//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================
/*
 * Class implementation for a GPS gateway
 * High hierarchy class
 * it will be instantiated and used from the main function
 */
class Gateway {
public:
    Gateway ();
    virtual ~Gateway ();
    void update();
    void changeState  (GatewayState * newGatewayState);
    void encodeJWT(char * payloadToJWT, char * jwtEncoded);
    void decodeJWT (char * jwtToDecode, char * payloadRetrived);
    
private:
    GatewayState * currentState;
    
    CellularModule* cellularTransceiver;
    TcpSocket * socketTargetted;
    CellInformation * currentCellInformation; 

    GNSSModule* currentGNSSModule;
    GNSSData * currentGNSSdata;
    NonBlockingDelay * latency;
    BatteryData  * batteryStatus;

    CustomJWT * jwt;
    char JWTKey [40] = "a-string-secret-at-least-256-bits-long";

};


//=====[#include guards - end]=================================================

#endif // _GATEWAY_H_
