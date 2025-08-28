//=====[#include guards - begin]===============================================

#ifndef _CONNECTION_UNAVAILABLE_STATE_H_
#define _CONNECTION_UNAVAILABLE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "IdleState.h"
#include "Non_Blocking_Delay.h"
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration to resolve circular dependency.
struct CellInformation; //!< Forward declaration for cell information structure.

//=====[Declaration of public classes]=========================================
/**
 * @class ConnectionUnavailableState
 * @brief Represents the initial or disconnected connection state.
 * Implements the `ConnectionState` interface as part of the State design pattern.
 * This state indicates that no connection is allowed yet. It does not 
 * attempt to connect, but can transition to `IdleState` upon enabling the connection.
 */
class ConnectionUnavailableState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================

    /**
    * @brief Default constructor.
    */
    ConnectionUnavailableState();

    /**
     * @brief Constructor with reference to the CellularModule.
     * @param mobileModule Pointer to the owning CellularModule.
    */
    ConnectionUnavailableState (CellularModule * mobileModule);

    /**
     * @brief Destructor.
    */
    virtual ~ConnectionUnavailableState();

    /**
     * @brief Returns status indicating that no connection is available.
     * @return `CELLULAR_CONNECTION_STATUS_UNAVAIBLE`
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler,
     NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);

    /**
     * @brief Neighbor information will not be retrieved in this state.
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay.
     * @param neighborsCellInformation Vector of structures of DataCell where results will be stored.
     * @param numberOfNeighbors Maximum number of neighbors to retrieve.
     * @return Always returns true to indicate the step is skipped.
     */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);

    /**
     * @brief Transitions the connection state to IdleState.
     * This enables the connection logic by progressing to the next state.
     */
    virtual void enableConnection ();
private:
    CellularModule * mobileNetworkModule; //!< Reference to the owning CellularModule.
    bool readyToSend; //!< Reserved flag; not used in this state.
    bool enableTransceiver; //!< Reserved flag; not used in this state.
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CONNECTION_UNAVAILABLE_STATE_H_