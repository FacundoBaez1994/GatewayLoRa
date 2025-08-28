//=====[#include guards - begin]===============================================
#ifndef _MANUAL_POWER_OFF_STATE_H_
#define _MANUAL_POWER_OFF_STATE_H_

//==================[Libraries]===============================================
#include "PowerManager.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "PowerState.h"
#include "PowerONState.h"
#include "Non_Blocking_Delay.h"
#include "PowerManager.h"

//=====[Declaration of public data types]======================================
class PowerManager; //debido a declaracion adelantada
struct BatteryData;

//=====[Declaration of public classes]=========================================
/**
 * @class ManualPowerOFFState
 * @brief Concrete State representing a manually powered-off module.
 * @details Handles manual power-on transitions and debounced button control.
 * From this state, the only way to return to PowerONState is by pressing the
 * manual button or by explicitly calling the turnOn() method.
 */
class ManualPowerOFFState : public PowerState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    ManualPowerOFFState ();

    /**
     * @brief Constructor with manager reference.
     * @param newManager Pointer to the PowerManager context object.
     */
    ManualPowerOFFState (PowerManager * newManager);

    /**
     * @brief Destructor. Nullifies internal pointer to manager.
     */
    virtual ~ManualPowerOFFState ();

    /**
     * @brief Updates the internal power state depending on the power signals.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     * @return Current power status after update.
     */
    powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Reboot is not supported in this state.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     * @return Always returns false.
     */
    bool reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    
    /**
     * @brief Sleep is not supported in this state.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     * @return Always returns false.
     */
    bool goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Awake operation is not defined in this state.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     */
    void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    
    /**
     * @brief Measures battery. Not supported in this state.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     * @param currentBatteryData Pointer to store battery data.
     * @return Always returns false.
     */
    bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData);

    /**
     * @brief Attempts to power on the module manually.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     * @return True if power-on transition succeeds, otherwise false.
     */
    bool turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Power-off is already the current state.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Pointer to non-blocking timer.
     * @return Always returns true.
     */
    bool turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
private:
//=====[Declaration of privates atributes]=========================================
    PowerManager * manager;       ///< Pointer to context manager object
    powerStatus_t status;         ///< Current power status
    bool ManualTurningPower;      ///< Internal flag indicating manual power toggle is in progress
    bool SignalTurningPowerUp;    ///< Indicates KEY signal is HIGH (manual power on trigger)
    bool TurningUP;               ///< Flag to coordinate HIGH and LOW toggle of the KEY signal
    int turnONCounter = 0;        ///< Debounce counter to confirm power-on intent
    bool buttonPushed = false;    ///< Flag indicating button was pressed to power on
    
//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _MANUAL_POWER_OFF_STATE_H_