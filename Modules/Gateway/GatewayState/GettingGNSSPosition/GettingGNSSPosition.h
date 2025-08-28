//=====[#include guards - begin]===============================================
#ifndef _GETTING_GNSS_POSITION_H_
#define _GETTING_GNSS_POSITION_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; ///< Forward declaration to avoid circular dependencies

//=====[Declaration of public classes]=========================================
/**
 * @class GettingGNSSPosition
 * @brief Represents the gateway state where the device attempts to obtain GNSS coordinates.
 * @details This class is part of the State design pattern implementation for the Gateway.
 * In this state, the gateway interacts with the GNSS module to retrieve position
 * data, and then transitions to the next state depending on the GNSS availability
 * and current operation mode.
 */
class GettingGNSSPosition : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructs a new GettingGNSSPosition state.
     * @param gateway Pointer to the Gateway context owning this state.
     */
    GettingGNSSPosition (Gateway * gateway);

    /**
     * @brief Destroys the GettingGNSSPosition state. 
     * Sets the gateway pointer to nullptr to avoid dangling references.
     */
    virtual ~GettingGNSSPosition ();

    /**
     * @brief Updates the power status of the cellular transceiver.
     * @details This method can be called periodically to manage cellular power updates
     * while GNSS position acquisition is in progress.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param currentBatteryStatus Pointer to the current battery status data.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);


    /**
     * @brief Attempts to obtain GNSS position.
     * If GNSS data is successfully obtained, the gateway transitions to the next
     * state for network connection or message formatting. If GNSS is unavailable,
     * it transitions to a fallback state based on the operation mode.
     * @param currentGNSSModule Pointer to the GNSS module interface.
     * @param currentGNSSdata Pointer to the structure where GNSS data will be stored.
     */
    virtual void obtainGNSSPosition (GNSSModule * currentGNSSModule, GNSSData * currentGNSSdata);
private:
    Gateway * gateway;  ///< Pointer to the Gateway context
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GETTING_GNSS_POSITION_H_