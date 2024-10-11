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
 struct BatteryData {
    unsigned int batteryChargeStatus; // 0 not charging, 1 charging and 2 charging has finished
    unsigned int chargeLevel;        // from 0 to 100
    float voltage;       // in mV measured
 };


//=====[Declaration of public classes]=========================================

/*
 *  
 * 
 */
class PowerManager {
public:

//=====[Declaration of public methods]=========================================
    PowerManager(ATCommandHandler * AThandler);
    virtual ~PowerManager ();
    powerStatus_t startStopUpdate ();
    void reboot ();
    bool goToSleep ();
    void awake ();
    void changePowerState (PowerState * newPowerState);
    bool readInputControlButton ();
    bool readPowerStatus ();
    void changeDTRSignal (bool newStatus);
    void changeKeyDigitalSignal (bool newStatus);
    BufferedSerial* getUART ();
    bool measureBattery (BatteryData * currentBatteryData);

private:
//=====[Declaration of privates atributes]=========================================
    NonBlockingDelay* powerChangeDurationTimer;
    ATCommandHandler* ATHandler;
    PowerState* currentPowerState;

    DigitalIn* powerStatusInput; // negative value if power on
    DigitalIn* powerControlButtonInput;
    DigitalOut* powerKeyOutput; // soft power control
    DigitalOut* powerDownOutput; // Power source enable - hard power control
    DigitalOut* powerKeySleepOutput;

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _POWER_MANAGER_H_