//=====[#include guards - begin]===============================================

#ifndef _WAITING_FOR_MESSAGE_H_
#define _WAITING_FOR_MESSAGE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Gateway.h"
#include "Non_Blocking_Delay.h"
#include "GatewayBaseState.h"
#include "SendingAck.h"
#include <algorithm>
#include <string>
#include <vector>
#include <cstring>

//=====[Declaration of public data types]======================================

/**
 * @brief Forward declaration of the Gateway class.
 *
 * Used to avoid circular dependencies between header files.
 */
class Gateway;

//=====[Declaration of public classes]=========================================

/**
 * @class WaitingForMessage
 * @brief Gateway state responsible for receiving LoRa messages.
 *
 * This class is part of the **State Design Pattern** implemented by the Gateway.
 * It waits for incoming LoRa packets, handles message fragmentation,
 * reconstructs complete messages using delimiters, and validates the
 * received payload.
 *
 * Once a valid message is received and processed, the state transitions
 * to the ACK sending state.
 */
class WaitingForMessage : public GatewayBaseState {
public:

    //=====[Declaration of public methods]=====================================

    /**
     * @brief Constructor for the WaitingForMessage state.
     *
     * @param gateway Pointer to the Gateway instance that owns this state.
     */
    WaitingForMessage(Gateway * gateway);

    /**
     * @brief Destructor for the WaitingForMessage class.
     */
    virtual ~WaitingForMessage();

    /**
     * @brief Waits for an incoming LoRa message.
     *
     * This method listens for LoRa packets, accumulates message fragments,
     * detects message delimiters, reconstructs the complete message,
     * and triggers message parsing and validation.
     *
     * The operation is designed to be non-blocking and compatible with
     * a finite state machine execution flow.
     *
     * @param LoRaModule Pointer to the LoRa radio interface.
     * @param messageRecieved Buffer where the reconstructed message will be stored.
     * @param timeOut Pointer to a NonBlockingDelay object used as a timeout mechanism.
     *
     * @return true if a complete and valid message was received, false otherwise.
     */
    virtual bool waitForMessage(LoRaClass * LoRaModule,
                                char * messageRecieved,
                                NonBlockingDelay * timeOut);

private:

    //=====[Declaration of private attributes]=================================

    /**
     * @brief Pointer to the Gateway that owns this state.
     */
    Gateway * currentGateway;

    //=====[Declaration of private methods]====================================
};


//=====[#include guards - end]=================================================

#endif // _WAITING_FOR_MESSAGE_H_
