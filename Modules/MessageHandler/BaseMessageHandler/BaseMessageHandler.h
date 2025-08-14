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
/**
 * @class BaseMessageHandler
 * @brief Abstract base class for implementing message processing handlers.
 * @details This class implements the Chain of Responsibility design pattern,
 * allowing a sequence of handlers to process a message. Subclasses must override
 * the `handleMessage()` method to provide specific behavior.
 */
class BaseMessageHandler : public MessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs a BaseMessageHandler.
     */
    BaseMessageHandler();

    /**
     * @brief Virtual destructor.
     */
    virtual ~BaseMessageHandler ();

    /**
     * @brief Handles an incoming message.
     * @param message Pointer to the message buffer.
     * @param sizeOfMessage Size of the message in bytes.
     * @return A status code indicating the result of message processing.
     */
    virtual MessageHandlerStatus_t handleMessage (char * incommingMessage, unsigned int sizeOfMessage);

    /**
     * @brief Sets the next handler in the chain.
     * @param nextHandler Pointer to the next MessageHandler.
     * @return A pointer to the handler that was set.
     */
    virtual MessageHandler  * setNextHandler (MessageHandler * nextHandler);
protected:
//=====[Declaration of protected atributes]=========================================
    MessageHandler * nextHandler; //< Pointer to the next handler in the chain
private:
//=====[Declaration of privates atributes]=========================================
    


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _BASE_MESSAGE_HANDLER_H_