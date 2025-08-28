//=====[#include guards - begin]===============================================

#ifndef _GOING_TO_SLEEP_H_
#define _GOING_TO_SLEEP_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; /**< Forward declaration to avoid circular dependencies. */

//=====[Declaration of public classes]=========================================
/**
 * @class GoingToSleep
 * @brief Represents the "Going to Sleep" state in the Gateway's state machine.
 * @details Implements the State design pattern, managing the transition phase 
 *          before the device enters its low-power sleep mode or turn off. 
 *          This state ensures that ongoing operations are finalized, 
 *          the cellular transceiver is turned off, and the appropriate next 
 *          state is set depending on the operational mode.
 */
class GoingToSleep : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================

    /**
     * @brief Constructs a new GoingToSleep state.
     * @param gateway Pointer to the main Gateway instance controlling the state machine.
     */
    GoingToSleep (Gateway * gateway);

    /**
     * @brief Destroys the GoingToSleep state object.
     * @details Sets internal pointers to nullptr to avoid dangling references.
     */
    virtual ~GoingToSleep ();

        /**
     * @brief Updates the current power status of the device.
     * @details In this state, the function may request the cellular transceiver 
     *          to update or stop ongoing processes before sleep.
     * @param cellularTransceiver Pointer to the module handling cellular communication.
     * @param currentBatteryStatus Pointer to the current battery status structure.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @brief Initiates the transition to sleep mode.
     * @details Determines whether the gateway should enter battery sensing mode 
     *          or proceed to the sleeping state based on the current operation mode 
     *          and the successful shutdown of the transceiver.
     * @param cellularTransceiver Pointer to the module handling cellular communication.
     */
    virtual void goToSleep (CellularModule * cellularTransceiver);
private:
    Gateway * gateway; /**< Pointer to the main Gateway instance. */
  

//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GOING_TO_SLEEP_H_