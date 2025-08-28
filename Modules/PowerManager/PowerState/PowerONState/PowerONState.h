//=====[#include guards - begin]===============================================
#ifndef _POWER_ON_STATE_H_
#define _POWER_ON_STATE_H_

//==================[Libraries]===============================================
#include "PowerManager.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "PowerState.h"
#include "Non_Blocking_Delay.h"
#include "ManualPowerOFFState.h"
#include "PowerManager.h"
#include "SleepState.h"

//=====[Declaration of public data types]======================================
class PowerManager; ///< Forward declaration of PowerManager
struct BatteryData; ///< Forward declaration of BatteryData

//=====[Declaration of public classes]=========================================
/**
 * @class PowerONState
 * @brief Concrete State representing the module in an active (powered on) state.
 * @details This class implements the behavior of the power management system when the module is fully powered on.
 * It handles transitions to other states like Sleep, PowerOFF, and ManualPowerOFF.
 */
class PowerONState : public PowerState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    PowerONState ();

    /**
     * @brief Constructs a PowerONState associated with a PowerManager.
     * @param newManager Pointer to the PowerManager context object.
     */
    PowerONState (PowerManager * newManager);
    
    /**
     * @brief Destructor. Clears internal manager reference.
     */
    virtual ~PowerONState ();

    /**
     * @brief Monitors power status and triggers transitions to other states if necessary.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer controlling transition delays.
     * @return Current power status after evaluation.
     */
    powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);


    /**
     * @brief Attempts to reboot the module using standard power-down.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer controlling transition delays.
     * @return True if reboot initiated successfully.
     */
    bool reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Attempts to put the module into sleep mode.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer controlling transition delays.
     * @return True if sleep command was accepted.
     */
    bool goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief No operation in this state. Included for interface compliance.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer controlling transition delays.
    */
    void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Measures the battery using an AT command and parses the result.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer controlling retry timing.
     * @param currentBatteryData Pointer to structure where the result will be stored.
     * @return True if measurement succeeded and data was parsed correctly.
    */
    bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData);


    /**
     * @brief No-op; the system is already powered on.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer (unused).
     * @return Always returns true.
     */
    bool turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);


    /**
     * @brief Attempts to power off the system, using soft or hard fallback.
     * @param AThandler Pointer to the AT command handler.
     * @param powerChangeDurationTimer Pointer to the timer controlling retry timing.
     * @return True if the system transitions to off state.
     */
    bool turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
  
private:
//=====[Declaration of privates atributes]=========================================
    PowerManager * manager; ///< Pointer to the context object (PowerManager).
    powerStatus_t status;   ///< Current power status.
    bool ManualTurningPower; ///< Indicates manual power toggle is in progress.
    bool SignalTurningPowerUp; ///< Indicates signal transition is active.
    bool TurningDown; ///< Indicates shutdown sequence is active.
    bool turnOfWasCall; ///< Indicates if the method turnOf() was call 

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Reports battery data.
     * @param stringToAnalyse Pointer to the buffer with the raw response.
     * @param currentBatteryData Pointer to the structure to populate.
     * @return True if parsing succeeded and values were valid.
     */
    bool retrivBatteryData (char * stringToAnalyse,  BatteryData * currentBatteryData);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _POWER_ON_STATE_H_