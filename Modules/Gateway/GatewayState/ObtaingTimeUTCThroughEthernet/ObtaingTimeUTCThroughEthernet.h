//=====[#include guards - begin]===============================================

#ifndef _OBTAING_TIME_UTR_THROUGH_ETHENET_H_
#define _OBTAING_TIME_UTR_THROUGH_ETHENET_H_

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
 * @class ObtaingTimeUTCThroughEthernet
 * @brief Gateway state responsible for obtaining UTC time via Ethernet.
 *
 * This class is part of the **State Design Pattern** used by the Gateway.
 * It queries a remote NTP/Time server over Ethernet to obtain the current
 * UTC time and updates the system clock accordingly.
 *
 * Once the time is successfully retrieved (or if an error occurs),
 * the state transitions to the message formatting state.
 */
class ObtaingTimeUTCThroughEthernet : public GatewayBaseState {
public:

    //=====[Declaration of public methods]=====================================

    /**
     * @brief Constructor for the ObtaingTimeUTCThroughEthernet state.
     *
     * @param gateway Pointer to the Gateway instance that owns this state.
     * @param gatewayStatus Initial gateway status associated with this state.
     */
    ObtaingTimeUTCThroughEthernet(Gateway * gateway,
                                 gatewayStatus_t gatewayStatus);

    /**
     * @brief Destructor for the ObtaingTimeUTCThroughEthernet class.
     */
    virtual ~ObtaingTimeUTCThroughEthernet();

    /**
     * @brief Queries the current UTC time from a remote time server via Ethernet.
     *
     * This method sends a request to a remote time/NTP server using UDP,
     * receives the current time, converts it to UNIX epoch format,
     * and updates the system RTC.
     *
     * The operation is designed to be used within a non-blocking
     * state machine execution flow.
     *
     * @param ethernetModule Pointer to the Ethernet interface used for communication.
     * @param delay Pointer to a NonBlockingDelay object for timing control.
     */
    virtual void queryUTCTimeViaRemoteServer(UipEthernet * ethernetModule,
                                             NonBlockingDelay * delay);

private:

    //=====[Declaration of private attributes]=================================

    /**
     * @brief Pointer to the Gateway that owns this state.
     */
    Gateway * gateway;

    /**
     * @brief Ethernet connection retry counter.
     *
     * Used to limit the number of attempts when querying the time server.
     */
    int connectionRetries;

    /**
     * @brief Current gateway status while in this state.
     */
    gatewayStatus_t currentStatus;

    //=====[Declaration of private methods]====================================
};


//=====[#include guards - end]=================================================

#endif // _OBTAING_TIME_UTR_THROUGH_ETHENET_H_
