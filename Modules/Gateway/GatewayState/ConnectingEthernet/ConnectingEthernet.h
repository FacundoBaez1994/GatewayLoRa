//=====[#include guards - begin]===============================================

#ifndef _CONNECTING_ETHERNET_H_
#define _CONNECTING_ETHERNET_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Non_Blocking_Delay.h"

//=====[Declaration of public data types]======================================

/**
 * @brief Forward declaration of the Gateway class.
 *
 * Used to avoid circular dependencies between header files.
 */
class Gateway;

//=====[Declaration of public classes]=========================================

/**
 * @class ConnectingEthernet
 * @brief Ethernet connection state of the Gateway.
 *
 * This class is part of the **State Design Pattern** implemented by the Gateway.
 * It is responsible for handling the Ethernet connection process to the local
 * network, including retry logic and connection status control.
 */
class ConnectingEthernet : public GatewayBaseState {
public:

    //=====[Declaration of public methods]=====================================

    /**
     * @brief Constructor for the ConnectingEthernet state.
     *
     * @param gateway Pointer to the Gateway instance that owns this state.
     * @param gatewayStatus Initial gateway status associated with this state.
     */
    ConnectingEthernet(Gateway * gateway, gatewayStatus_t gatewayStatus);

    /**
     * @brief Destructor for the ConnectingEthernet class.
     */
    virtual ~ConnectingEthernet();

    /**
     * @brief Attempts to connect the Ethernet module to the local network.
     *
     * This method implements the Ethernet connection logic in a non-blocking
     * manner, using a timer to manage retries and timeouts.
     *
     * @param ethernetModule Pointer to the Ethernet module used for connection.
     * @param delay Pointer to a NonBlockingDelay object for timing control.
     */
    virtual void connectEthernetToLocalNetwork(UipEthernet * ethernetModule,
                                               NonBlockingDelay * delay);

private:

    //=====[Declaration of private attributes]=================================

    /**
     * @brief Pointer to the Gateway that owns this state.
     */
    Gateway * gateway;

    /**
     * @brief Ethernet connection retry counter.
     */
    int connectionRetries;

    /**
     * @brief Buffer used to store communication payloads.
     */
    char payload[250];

    /**
     * @brief Current gateway status while in this state.
     */
    gatewayStatus_t currentStatus;

    //=====[Declaration of private methods]====================================
};


//=====[#include guards - end]=================================================

#endif // _CONNECTING_ETHERNET_H_
