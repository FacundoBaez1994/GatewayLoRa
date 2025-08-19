//=====[#include guards - begin]===============================================

#ifndef _RF_TRANSCIEVER_STATE_H_
#define _RF_TRANSCIEVER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "LoRa.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================
struct RFPacketMessage;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class RFTransicieverState {
public:
//=====[Declaration of public methods]=========================================
   // virtual void addRFFormatToMessage (long long int deviceId, int messageNumber, char * messageToBeSend);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, char * messageToBeSend, NonBlockingDelay * backoffTime);
    virtual bool waitForMessage (LoRaClass * LoRaModule, char * messageRecieved, NonBlockingDelay * timeOut);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _RF_TRANSCIEVER_STATE_H_