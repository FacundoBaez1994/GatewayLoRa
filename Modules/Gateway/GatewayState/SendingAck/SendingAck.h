//=====[#include guards - begin]===============================================

#ifndef _SENDING_ACK_H_
#define _SENDING_ACK_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayState.h"
#include "Non_Blocking_Delay.h"
#include "AuthenticationGenerator.h"
#include "EncrypterBase64.h"
#include "ChecksumGenerator.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada
//struct TcpSocket;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class SendingAck : public GatewayState {
public:
//=====[Declaration of public methods]=========================================
    SendingAck (Gateway * gateway, int IdDevice, int messageNumber);
    virtual ~SendingAck ();
    virtual void receiveMessage (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendAcknowledgement (LoRaClass * LoRaModule, NonBlockingDelay * delay);
    virtual void sendTCPMessage (UipEthernet * ethernetModule, NonBlockingDelay * delay);
private:
    Gateway * gateway;
    int IdDevice;
    int messageNumber;

    EncrypterBase64 encrypt;
    AuthenticationGenerator authgen;
    ChecksumGenerator ckgen;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_SENDING_H_