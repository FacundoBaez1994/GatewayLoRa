//=====[#include guards - begin]===============================================

#ifndef _SLEEPING_H_
#define _SLEEPING_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway;  /**< Forward declaration to avoid circular dependency with Gateway.h */

//=====[Declaration of public classes]=========================================
/**
 * @class Slepping
 * @brief Represents the "Sleeping" state in the Gateway's State design pattern.
 * @details In this state, the device reduces its activity to save power. 
 * It monitors specific events (e.g., movement detection, timers, and silent mode expiration) 
 * to decide when to transition back to an active state. 
 * The state is responsible for:
 * - Periodic power status checks.
 * - Waking up the device under certain movement or timer conditions.
 * - Coordinating transitions to other states such as SensingBatteryStatus.
 */
class Slepping : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs the Sleeping state.
     * @param gateway Pointer to the main Gateway context to allow state transitions and data access.
     */
    Slepping (Gateway * gateway);

    /**
     * @brief Destructor for the Sleeping state.
     * @details Cleans up internal references to avoid dangling pointers.
     */
    virtual ~Slepping ();

    /**
     * @brief Updates the device's power status during the sleeping phase.
     * @details Typically used to trigger background tasks related to the cellular transceiver's 
     * power management.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param currentBatteryStatus Pointer to the current battery status data structure.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    
    /**
     * @brief Attempts to wake up the device from the sleeping state.
     * @details This method evaluates multiple conditions, such as movement events, 
     * latency timers, and silent mode expiration, to decide if the Gateway should 
     * transition to an active state.
     * Typical wake-up triggers:
     * - Expiration of silent mode.
     * - Operation mode set to pursuit.
     * - Movement detected (restarted or active after parking).
     * - Latency timeout when moving.
     * Upon successful wake-up, the Gateway usually transitions to SensingBatteryStatus state.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param latency Pointer to a non-blocking delay timer controlling wake-up latency.
     * @param silentKeepAliveTimer Pointer to a non-blocking delay timer controlling silent mode
     * and keepalive duration.
     */
    virtual void awake (CellularModule * cellularTransceiver, NonBlockingDelay * latency,
    NonBlockingDelay * silentKeepAliveTimer);
private:
    Gateway * gateway;
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SLEEPING_H_