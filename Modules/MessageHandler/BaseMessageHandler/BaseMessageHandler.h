//=====[#include guards - begin]===============================================

#ifndef _BASE_MESSAGE_HANDLER_H_
#define _BASE_MESSAGE_HANDLER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "MessageHandler.h"
 
//=====[Declaration of public data types]======================================
//class CellularModule; //debido a declaracion adelantada
//struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  Abstract class 
 * 
 */
class BaseMessageHandler : public MessageHandler {
public:
//=====[Declaration of public methods]=========================================
    BaseMessageHandler();
    virtual ~BaseMessageHandler ();
    virtual MessageHandlerStatus_t handleMessage (char * incommingMessage);
    virtual MessageHandler  * setNextHandler (MessageHandler * nextHandler);
protected:
    MessageHandler * nextHandler;
private:
    
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _BASE_MESSAGE_HANDLER_H_