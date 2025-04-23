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

#include "LoRa.h"
#include "UipEthernet.h"
#include "WaitingForMessage.h"


#include "MessageHandler.h"
#include "MessageHandlerStatus.h"
#include "AuthenticationGenerator.h"
#include "AuthenticationVerifier.h"
#include "BaseMessageHandler.h"
#include "ChecksumVerifier.h"
#include "ChecksumGenerator.h"
#include "Decrypter.h"
#include "Encrypter.h"





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
    bool prepareMessage (char * messageOutput);
    bool processMessage (char * incomingMessage);
    
private:
    GatewayState * currentState;
    
    CellularModule* cellularTransceiver;
    TcpSocket * socketTargetted;
    CellInformation * currentCellInformation; 

    GNSSModule* currentGNSSModule;
    GNSSData * currentGNSSdata;
    NonBlockingDelay * latency; // timer EC21 
    BatteryData  * batteryStatus;

    CustomJWT * jwt;
    char JWTKey [40] = "a-string-secret-at-least-256-bits-long";

    LoRaClass * LoRaTransciever;
    NonBlockingDelay * timer; // timer LoRa

    UipEthernet * ethernetModule;
    DigitalOut * resetEth;

    // Message Handlers
    MessageHandler * encrypter;
    MessageHandler * authgen;
    MessageHandler * ckgen;
    MessageHandler * checksumVerifier;
    MessageHandler * authVer;
    MessageHandler * decrypter;

};


//=====[#include guards - end]=================================================

#endif // _GATEWAY_H_
