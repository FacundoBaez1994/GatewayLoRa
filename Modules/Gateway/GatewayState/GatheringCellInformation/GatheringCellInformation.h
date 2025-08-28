//=====[#include guards - begin]===============================================

#ifndef _GATHERING_CELL_INFORMATION_H_
#define _GATHERING_CELL_INFORMATION_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "GatewayBaseState.h"
#include "Gateway.h"

//=====[Declaration of public data types]======================================
class Gateway; 

//=====[Declaration of public classes]=========================================
/**
 * @class GatheringCellInformation
 * @brief State responsible for acquiring neighboring cell data.
 * @details This state interacts with the cellular transceiver to retrieve 
 * information about nearby cells and stores it in the provided 
 * container. If the acquisition is successful, it transitions 
 * the gateway to the next state in the sequence (e.g., 
 * GatheringInertialData). 
 * It also manages periodic power status updates for the cellular module 
 * and can perform motion stability checks through the IMUManager.
 * @note Part of the State design pattern for gateway device operation.
 */
class GatheringCellInformation : public GatewayBaseState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor.
     * @param gateway Pointer to the gateway context that owns this state.
     */
    GatheringCellInformation (Gateway * gateway);

    /**
     * @brief Destructor.
     */
    virtual ~GatheringCellInformation ();

    /**
     * @brief Update the cellular module's power status.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param currentBatteryStatus Pointer to the current battery status structure.
     */
    virtual void updatePowerStatus (CellularModule * cellularTransceiver, BatteryData * currentBatteryStatus);

    /**
     * @brief Retrieve information about neighboring cells.
     * @details Attempts to obtain cell information for the given number of neighbors.
     * If successful, triggers a state change to gather inertial data.
     * @param cellularTransceiver Pointer to the cellular module interface.
     * @param neighborsCellInformation Reference to the vector that will store the retrieved cell information.
     * @param numberOfNeighbors Number of neighboring cells to retrieve.
    */
    virtual void obtainNeighborCellsInformation (CellularModule* cellularTransceiver, 
    std::vector<CellInformation*> &neighborsCellInformation, int numberOfNeighbors );

private:
    Gateway * gateway;  /**< Pointer to the gateway context that owns this state. */
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _GATHERING_CELL_INFORMATION_H_