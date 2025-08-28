//=====[#include guards - begin]===============================================

#ifndef _CONNECTION_STATE_H_
#define _CONNECTION_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "CellularConnectionStatus.h"

//=====[Declaration of public data types]======================================
struct CellInformation;

//=====[Declaration of public classes]=========================================
/**
 * @class ConnectionState
 * @brief Interface for connection behavior in the State design pattern.
 * This abstract base class defines the interface for managing the connection
 * logic of the cellular module. Concrete implementations (states) such as
 * will implement these methods to provide behavior specific to that connection state.
 * 
 */
class ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Attempts to connect to the mobile network.
     * 
     * This method must be implemented by concrete states to define how the
     * connection attempt is performed.
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay for timing logic.
     * @param currentCellInformation Pointer to structure where cell info will be stored.
     * @return Status of the connection attempt.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, CellInformation * currentCellInformation);

    /**
     * @brief Retrieves information about neighboring cells. 
     * Should be implemented by concrete states if neighbor retrieval is supported.
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay.
     * @param neighborsCellInformation Vector of structures of DataCell where results will be stored.
     * @param numberOfNeighbors Maximum number of neighbors to retrieve.
     * @return true if the process has finished, false otherwise.
     */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
    
    /**
     * @brief Enables the connection logic in the current state.
     */
    virtual void enableConnection ();
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_