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
class PowerManager; //debido a declaracion adelantada
 struct BatteryData;

//=====[Declaration of public classes]=========================================
/*
 *  class - State desing pattern
 * 
 */
class PowerONState : public PowerState {
public:
//=====[Declaration of public methods]=========================================
    PowerONState ();
    PowerONState (PowerManager * newManager);
    virtual ~PowerONState ();
    powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    bool reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    bool goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData);
  
private:
    bool retrivBatteryData (char * stringToAnalyse,  BatteryData * currentBatteryData);
    PowerManager * manager;
    powerStatus_t status;
    bool ManualTurningPower;
    bool SignalTurningPowerUp;
    bool TurningDown;
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _POWER_ON_STATE_H_