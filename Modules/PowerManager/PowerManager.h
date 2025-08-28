//=====[#include guards - begin]===============================================

#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "PowerState.h"
#include "PowerOFFState.h"

//=====[Declaration of public data types]======================================
/**
 * @struct BatteryData
 * @brief Holds battery measurement data.
 */
struct BatteryData {
    unsigned int batteryChargeStatus; ///< 0 = not charging, 1 = charging, 2 = charging complete
    unsigned int chargeLevel;         ///< Battery level percentage (0–100)
    float voltage;                    ///< Battery voltage in mV
};

//=====[Declaration of public classes]=========================================


/**
 * @class PowerManager
 * @brief Manages power control logic for a cellular module.
 * @details This class encapsulates digital I/O control, power transitions (on/off/sleep),
 * battery measurement, and state transitions. It uses the State design pattern 
 * through `PowerState` and its subclasses to delegate behavior.
*/
class PowerManager {
public:

//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs a PowerManager object.
     * @param AThandler Pointer to an existing ATCommandHandler instance.
    */
    PowerManager(ATCommandHandler * AThandler);

    /**
     * @brief Destructor. Frees dynamically allocated resources.
     */
    virtual ~PowerManager ();

    /**
     * @brief Updates the power state (typically used to process transitions).
     * @return A `powerStatus_t` indicating current power status.
     */
    powerStatus_t startStopUpdate ();
    
    /**
     * @brief Turns off the cellular module.
     * @return `true` if the transition succeeded.
     */
    bool turnOff ();

    /**
     * @brief Turns on the cellular module.
     * @return `true` if the transition succeeded.
     */
    bool turnOn ();

    /**
     * @brief Reboots the cellular module.
     * @return `true` if the reboot succeeded.
     */
    bool reboot ();

    /**
     * @brief Sends the module to a low-power sleep state.
     * @return `true` if the module entered sleep mode.
     */
    bool goToSleep ();

    /**
     * @brief Wakes the module up from sleep.
     */
    void awake ();

    /**
     * @brief Changes the current power state.
     * @param newPowerState Pointer to a new `PowerState` object.
     */
    void changePowerState (PowerState * newPowerState);

    /**
     * @brief Reads the digital input from the power control button.
     * @return `true` if the button is pressed (depends on circuit logic).
     */
    bool readInputControlButton ();

    /**
     * @brief Reads the power status signal from the module.
     * @return `true` if power signal indicates the module is on.
     */
    bool readPowerStatus ();

    /**
     * @brief Controls the DTR (sleep) signal line.
     * @param newStatus `true` or `false` to set the output state.
     */
    void changeDTRSignal (bool newStatus);

    /**
     * @brief Controls the hard power-down signal.
     * @param newStatus `true` or `false` to set the output state.
     */
    void changePowerDownSignal (bool newStatus);

    /**
     * @brief Controls the soft power key output.
     * @param newStatus `true` or `false` to set the output state.
     */
    void changeKeyDigitalSignal (bool newStatus);

    /**
     * @brief Returns the UART interface used by the AT command handler.
     * @return Pointer to a `BufferedSerial` instance.
     */
    BufferedSerial* getUART ();


    /**
     * @brief Measures and retrieves battery data.
     * @param currentBatteryData Pointer to structure to be populated.
     * @return `true` if measurement succeeded.
     */
    bool measureBattery (BatteryData * currentBatteryData);

private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* powerChangeDurationTimer; ///< Timer for power transitions
    ATCommandHandler* ATHandler;                ///< Handler for sending AT commands
    PowerState* currentPowerState;              ///< Current power behavior state

    DigitalIn* powerStatusInput;                ///< Reads module power status
    DigitalIn* powerControlButtonInput;         ///< Reads power button input
    DigitalOut* powerKeyOutput;                 ///< Controls soft power key signal
    DigitalOut* powerDownOutput;                ///< Controls hard power-down signal
    DigitalOut* powerKeySleepOutput;            ///< Controls sleep (DTR) signal

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _POWER_MANAGER_H_