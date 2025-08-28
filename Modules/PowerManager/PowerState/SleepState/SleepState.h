//=====[#include guards - begin]===============================================
#ifndef _SLEEP_STATE_H_
#define _SLEEP_STATE_H_

//==================[Libraries]===============================================
#include "PowerManager.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "PowerState.h"
#include "Non_Blocking_Delay.h"
#include "PowerManager.h"

//=====[Declaration of public data types]======================================
class PowerManager; ///< Forward declaration of PowerManager
struct BatteryData; ///< Forward declaration of BatteryData

//=====[Declaration of public classes]=========================================
/**
 * @class SleepState
 * @brief Concrete state representing the module in sleep mode.
 * @details Implements the behavior of the power system while the module is in sleep state.
 * Transitions to other states such as PowerON or ManualPowerOFF are handled here.
 */
class SleepState : public PowerState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    SleepState ();

    /**
     * @brief Constructor with PowerManager reference.
     * @param newManager Pointer to the PowerManager handling the state.
     */
    SleepState(PowerManager* newManager);

    /**
     * @brief Destructor.
     */
    virtual ~SleepState ();

    /**
     * @brief Updates internal state logic, usually called periodically.
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     * @return Current power status (always SLEEP for this state).
     */
    powerStatus_t startStopUpdate (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer);

    /**
     * @brief Attempt to reboot the device (not allowed in sleep you must awake first).
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     * @return Always false in this state.
     */
    bool reboot (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer);

    /**
     * @brief Put device to sleep (already in this state).
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     * @return Always true.
     */
    bool goToSleep (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer);

    /**
     * @brief Wake the device from sleep mode.
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     */
    void awake (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer);

    /**
     * @brief Attempt to measure battery (not supported in sleep).
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     * @param currentBatteryData Pointer to a structure to hold battery information.
     * @return Always false.
     */
    bool measureBattery (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer
    ,  BatteryData* currentBatteryData);


    /**
     * @brief Turns on the device, transitions to PowerON.
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     * @return Always true.
     */
    bool turnOn (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer);

    /**
     * @brief Turns off the device, using soft or hard shutdown as fallback.
     * @param AThandler Pointer to the AT command interface.
     * @param powerChangeDurationTimer Timer used to handle non-blocking time-based transitions.
     * @return True if shutdown was successful.
     */
    bool turnOff (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer);
  
private:
//=====[Declaration of privates atributes]=========================================
    PowerManager* manager;             ///< Reference to the context (PowerManager)
    powerStatus_t status;             ///< Current state status
    bool ManualTurningPower;          ///< Internal flag to indicate manual shutdown in progress
    bool SignalTurningPowerUp;        ///< Internal flag for power-up signal
    bool TurningDown;                 ///< Internal flag indicating shutdown in progress

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SLEEP_STATE_H_