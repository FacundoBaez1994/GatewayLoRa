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
/**
 * @class MessageHandler
 * @brief Abstract interface for message processing in a chain of responsibility.
 * @details This class defines the interface for handling messages in a modular,
 * extendable way. It is intended to be subclassed by concrete message handlers.
 * Implements the Chain of Responsibility design pattern.
 */
class MessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Attempts to handle the given message.
     * @param message Pointer to the message to be processed.
     * @param sizeOfMessage Length of the message in bytes.
     * @return A status code indicating the result of the handling attempt.
     */
    virtual MessageHandlerStatus_t handleMessage (char * message, unsigned int sizeOfMessage);

    /**
     * @brief Sets the next handler in the chain.
     * @param nextHandler Pointer to the next MessageHandler in the chain.
     * @return A pointer to the same nextHandler, enabling fluent-style chaining.
     */
    virtual MessageHandler * setNextHandler (MessageHandler * nextHandler);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _MESSAGE_HANDLER_H_