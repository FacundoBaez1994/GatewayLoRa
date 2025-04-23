//=====[#include guards - begin]===============================================

#ifndef _MESSAGE_HANDLER_H_
#define _MESSAGE_HANDLER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "MessageHandlerStatus.h"

//=====[Declaration of public data types]======================================

//=====[Declaration of public classes]=========================================
/*
 *  Interface - Chaing Of Responsability desing pattern
 * 
 */
class MessageHandler {
public:
//=====[Declaration of public methods]=========================================
    virtual MessageHandlerStatus_t handleMessage (char * message);
    virtual MessageHandler * setNextHandler (MessageHandler * nextHandler);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _MESSAGE_HANDLER_H_