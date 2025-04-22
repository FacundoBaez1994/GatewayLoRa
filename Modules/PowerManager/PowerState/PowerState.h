//=====[#include guards - begin]===============================================

#ifndef _POWER_STATE_H_
#define _POWER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"

//=====[Declaration of public data types]======================================
typedef enum {
    POWER_OFF,
    MANUAL_POWER_OFF,
    POWER_ON,
    SLEEP,
} powerStatus_t;

 struct BatteryData;

//=====[Declaration of public classes]=========================================
/*
 *  Interface - State desing pattern
 * 
 */
class PowerState {
public:
//=====[Declaration of public methods]=========================================
    virtual powerStatus_t startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    virtual bool reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    virtual bool goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    virtual void awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer);
    virtual bool measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData);
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _POWER_STATE_H_