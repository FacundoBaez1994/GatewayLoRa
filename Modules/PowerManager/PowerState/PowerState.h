//=====[#include guards - begin]===============================================

#ifndef _POWER_STATE_H_
#define _POWER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"

//=====[Declaration of public data types]======================================
/**
 * @enum powerStatus_t
 * @brief Represents the current power state of the device.
 */
typedef enum {
    POWER_OFF,         ///< Module is powered off
    MANUAL_POWER_OFF,  ///< Manual shutdown (e.g., user-requested)
    POWER_ON,          ///< Module is powered on
    SLEEP,             ///< Module is in low-power sleep mode
} powerStatus_t;

struct BatteryData; // Forward declaration


//=====[Declaration of public classes]=========================================
/**
 * @class PowerState
 * @brief Interface for defining power state behavior.
 * @details This abstract class defines the interface for all concrete states in the
 * power management system. Subclasses implement specific behavior for
 * powering on/off, rebooting, sleeping, and measuring battery status.
 * Used as part of the State design pattern by `PowerManager`.
 */
class PowerState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Evaluates or updates the current state (non-blocking).
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays in transitions.
     * @return Current power status.
     */
    virtual powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Attempts to power on the device.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays in transitions.
     * @return `true` if the transition process has finished correctly.
     */
    virtual bool turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Attempts to power off the device.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays in transitions.
     * @return `true` if the transition process has finished correctly.
     */
    virtual bool turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Reboots the device.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays in transitions.
     * @return `true` if the reboot was done successfully.
    */
    virtual bool reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);


    /**
     * @brief Sends the device to low-power sleep mode.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays in transitions.
     * @return `true` if the transition process has finished correctly.
     */
    virtual bool goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Wakes the device from sleep mode.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays in transitions.
     */
    virtual void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);

    /**
     * @brief Measures the battery and stores the result in the provided struct.
     * @param AThandler Pointer to AT command handler.
     * @param powerChangeDurationTimer Timer used for delays.
     * @param currentBatteryData Pointer to structure to fill with data.
     * @return `true` if measurement was successful.
     */
    virtual bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _POWER_STATE_H_