//=====[#include guards - begin]===============================================

#ifndef _POWER_OFF_STATE_H_
#define _POWER_OFF_STATE_H_

//==================[Libraries]===============================================

#include "PowerManager.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "PowerState.h"
#include "Non_Blocking_Delay.h"
#include "PowerONState.h"
#include "PowerManager.h"

//=====[Declaration of public data types]======================================
class PowerManager; ///< Forward declaration of PowerManager
struct BatteryData; ///< Forward declaration of BatteryData

//=====[Declaration of public classes]=========================================
/**
 * @class PowerOFFState
 * @brief Concrete State representing the module in a powered off state.
 * @details Implements the behavior of the system when the module is turned off, for any reason
 * it may initiate a transition back to the ON state depending on the conditions.
 */
class PowerOFFState : public PowerState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    PowerOFFState ();

    /**
     * @brief Constructs a PowerOFFState associated with a PowerManager.
     * @param newManager Pointer to the PowerManager context object.
     */
    PowerOFFState (PowerManager * newManager);

    /**
     * @brief Destructor. Clears internal manager reference.
     */
    virtual ~PowerOFFState ();

   /**
     * @brief Evaluates current power signals and may trigger power-on sequence.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer controlling transitions.
     * @return Current power status after evaluation.
     */
    powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Reboot is not supported from the OFF state.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer (unused).
     * @return Always returns false.
     */
    bool reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Sleep is not supported from the OFF state, the module is turn off.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer (unused).
     * @return Always returns false.
     */
    bool goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Awake operation is not relevant in the OFF state.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer (unused).
     */
    void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    
    /**
     * @brief Battery measurement is not possible in the OFF state.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer (unused).
     * @param currentBatteryData Pointer to where battery info would be stored.
     * @return Always returns false.
    */
    bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,BatteryData * currentBatteryData);


    /**
     * @brief Triggers power-on transition logic.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer for delays.
     * @return True if transition to ON was successful.
     */
    bool turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Transitions to ManualPowerOFFState regardless of current state.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to timer (unused).
     * @return Always returns true.
     */
    bool turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

private:
    PowerManager * manager;
    powerStatus_t status;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _POWER_OFF_STATE_H_