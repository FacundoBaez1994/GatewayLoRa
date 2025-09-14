//=====[#include guards - begin]===============================================

#ifndef _CONNECTING_TO_MOBILE_NETWORK_H_
#define _CONNECTING_TO_MOBILE_NETWORK_H_

//==================[Libraries]===============================================

#include "Non_Blocking_Delay.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "GatewayStatus.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; ///< Forward declaration due to circular dependency

//=====[Declaration of public classes]=========================================
/**
 * @brief State class implementing the connection process to the mobile network
 * @details This class represents the "ConnectingToMobileNetwork" state in the
 * Gateway's state machine. It manages attempts to connect the cellular module
 * to the mobile network and transitions the Gateway to appropriate subsequent
 * states based on connection success or failure.
 */
class ConnectingToMobileNetwork : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor
     * @param gateway Pointer to the Gateway instance for state control and data access
     * @param gatewayStatus Current status of the gateway for managing transitions
     */
    ConnectingToMobileNetwork (Gateway * gateway, gatewayStatus_t gatewayStatus);

    /** @brief Constructor
     * @param gateway Pointer to the Gateway instance for state control and data access
     */
    ConnectingToMobileNetwork (Gateway * gateway);

    /**
     * @brief Destructor
    */
    virtual ~ConnectingToMobileNetwork ();

        /**
     * @brief Updates the power status of the cellular module
     * @param cellularTransceiver Pointer to the CellularModule instance
     * @param currentBatteryStatus Pointer to current battery status data
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @brief Attempts to connect the cellular module to the mobile network
     * @param cellularTransceiver Pointer to the CellularModule instance
     * @param currentCellInformation Pointer to the CellInformation struct to store network info
     * @details Transitions the Gateway to next states depending on connection results.
     * If connected successfully, goes to GatheringInertialData or GatheringCellInformation.
     * If connection fails, attempts data sending over LoRa network by switching to
     * GatheringInertialData with appropriate status.
     */
    virtual void connectToMobileNetwork (CellularModule * cellularTransceiver,
    CellInformation * currentCellInformation);
private:
    Gateway * gateway; ///< Pointer to the Gateway instance
    gatewayStatus_t currentStatus; ///< Current operational status of the gateway
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CONNECTING_TO_MOBILE_NETWORK_H_