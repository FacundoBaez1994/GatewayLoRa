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
#include "SendingAck.h"


#include "MessageHandler.h"
#include "MessageHandlerStatus.h"
#include "AuthenticationGenerator.h"
#include "AuthenticationVerifier.h"
#include "BaseMessageHandler.h"
#include "ChecksumVerifier.h"
#include "ChecksumGenerator.h"
#include "Decrypter.h"
#include "Encrypter.h"
#include "DecrypterBase64.h"
#include "EncrypterBase64.h"



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
    bool prepareMessage (char * messageOutput, unsigned int messageSize);
    bool processMessage (char * incomingMessage, unsigned int messageSize);

private:

    // CellularModule* cellularTransceiver;
    LoRaClass * LoRaTransciever;
    UipEthernet * ethernetModule;
    DigitalOut * resetEth;
    GatewayState * currentState;

    NonBlockingDelay * timer;
   // BatteryData  * batteryStatus;

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
