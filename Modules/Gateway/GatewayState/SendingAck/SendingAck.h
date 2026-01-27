//=====[#include guards - begin]===============================================

#ifndef _SENDING_ACK_H_
#define _SENDING_ACK_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "Non_Blocking_Delay.h"
#include "GatewayBaseState.h"

//=====[Declaration of public data types]======================================

/**
 * @brief Forward declaration of the Gateway class.
 *
 * Used to avoid circular dependencies between header files.
 */
class Gateway;

//=====[Declaration of public classes]=========================================

/**
 * @class SendingAck
 * @brief Gateway state responsible for sending ACK messages via LoRa.
 *
 * This class is part of the **State Design Pattern** implemented by the Gateway.
 * It handles the generation, encoding, fragmentation, and transmission of
 * acknowledgment (ACK) messages over the LoRa interface.
 *
 * The ACK transmission is performed in a non-blocking manner, including
 * backoff timing, message fragmentation, and retry control.
 */
class SendingAck : public GatewayBaseState {
public:

    //=====[Declaration of public methods]=====================================

    /**
     * @brief Constructor for the SendingAck state.
     *
     * @param gateway Pointer to the Gateway instance that owns this state.
     * @param deviceId Unique identifier of the remote device.
     * @param messageNumber Message sequence number being acknowledged.
     */
    SendingAck(Gateway * gateway,
               long long int deviceId,
               int messageNumber);

    /**
     * @brief Constructor for the SendingAck state with gateway status.
     *
     * @param gateway Pointer to the Gateway instance that owns this state.
     * @param deviceId Unique identifier of the remote device.
     * @param messageNumber Message sequence number being acknowledged.
     * @param gatewayStatus Current gateway status.
     */
    SendingAck(Gateway * gateway,
               long long int deviceId,
               int messageNumber,
               gatewayStatus_t gatewayStatus);

    /**
     * @brief Destructor for the SendingAck class.
     */
    virtual ~SendingAck();

    /**
     * @brief Sends an acknowledgment (ACK) message over the LoRa interface.
     *
     * This method formats the ACK message, applies encoding if required,
     * fragments the message into multiple LoRa packets if necessary,
     * and transmits them using a non-blocking backoff mechanism.
     *
     * @param LoRaModule Pointer to the LoRa radio interface.
     * @param messageToBeSend Pointer to the message buffer to be transmitted.
     * @param backoffTime Pointer to a NonBlockingDelay object used for timing control.
     */
    virtual void sendAcknowledgement(LoRaClass * LoRaModule,
                                     char * messageToBeSend,
                                     NonBlockingDelay * backoffTime);

    /**
     * @brief Waits for an incoming message while in the ACK sending state.
     *
     * This method can be used to monitor incoming LoRa messages during
     * or after the ACK transmission process.
     *
     * @param LoRaModule Pointer to the LoRa radio interface.
     * @param messageRecieved Buffer where the received message will be stored.
     * @param timeOut Pointer to a NonBlockingDelay object used as a timeout.
     *
     * @return true if a message was received, false otherwise.
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

    /**
     * @brief Unique identifier of the remote device.
     */
    long long int IdDevice;

    /**
     * @brief Message sequence number being acknowledged.
     */
    int messageNumber;

    /**
     * @brief Current gateway status.
     */
    gatewayStatus_t currentStatus;

    //=====[Declaration of private methods]====================================
};


//=====[#include guards - end]=================================================

#endif // _SENDING_ACK_H_
