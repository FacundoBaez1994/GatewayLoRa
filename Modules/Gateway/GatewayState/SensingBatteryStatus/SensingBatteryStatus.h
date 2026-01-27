//=====[#include guards - begin]===============================================

#ifndef _SENSING_BATTERY_STATUS_H_
#define _SENSING_BATTERY_STATUS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; //debido a declaracion adelantada

//=====[Declaration of public classes]=========================================
/**
 * @class SensingBatteryStatus
 * @brief State in the Gateway's state machine responsible for reading the battery status.
 * 
 * This class is part of the State design pattern implementation for the Gateway system.
 * It focuses on measuring the battery status via the cellular module and deciding the 
 * next operational state based on the result.
 */
class SensingBatteryStatus : public GatewayBaseState{
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs the SensingBatteryStatus state.
     * 
     * @param gateway Pointer to the Gateway context that owns this state.
     */
    SensingBatteryStatus (Gateway * gateway);

    /**
     * @brief Destroys the SensingBatteryStatus state.
     * 
     * Sets the gateway pointer to nullptr.
     */
    virtual ~SensingBatteryStatus ();

    /**
     * @brief Updates the current power status of the system.
     * This method queries the cellular transceiver to measure the battery 
     * status and, if successful, transitions to the next state in the gateway.
     * @param cellularTransceiver Pointer to the CellularModule used for measurement.
     * @param currentBatteryStatus Pointer to a BatteryData structure to store the measured status.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);
private:
//=====[Declaration of privates atributes]=========================================
    //bool checkResponse (char * response, char * retrivMessage);
    Gateway * gateway; /**< Pointer to the Gateway context using this state. */


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _SENSING_BATTERY_STATUS_H_