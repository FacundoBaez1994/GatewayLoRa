//=====[#include guards - begin]===============================================

#ifndef _DEFINE_PDP_CONTEXT_H_
#define _DEFINE_PDP_CONTEXT_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "ConnectedState.h"
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule;     //!< Forward declaration of the owning module.
struct CellInformation;   //!< Structure holding cellular parameters.

//=====[Declaration of public classes]=========================================
/**
 * @class DefinePDPContext
 * @brief State responsible for defining the PDP context before establishing a data connection.
 *  This class is part of the State design pattern for managing cellular connectivity.
 */
class DefinePDPContext : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    DefinePDPContext ();

    /**
     * @brief Constructor with reference to the owning CellularModule.
     * @param mobileModule Pointer to the owning CellularModule.
    */
    DefinePDPContext (CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~DefinePDPContext ();

    /**
     * @brief Attempts to define the PDP context and on success transitions to the next state on success.
     * @param handler Pointer to the AT command handler.
     * @param refreshTime Pointer to the non-blocking delay timer.
     * @param currentCellInformation Pointer to the structure containing the current cell info.
     * @return CellularConnectionStatus_t Result of the connection attempt.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, 
    NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);

    /**
     * @brief Not used in this state.
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay.
     * @param neighborsCellInformation Vector of structures of DataCell where results will be stored.
     * @param numberOfNeighbors Maximum number of neighbors to retrieve
     * @return Always returns false.
     */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);

    /**
     * @brief Not used in this state.
     */
    virtual void enableConnection ();
private:
    CellularModule * mobileNetworkModule;   //!< Pointer to the owning module.
    bool readyToSend;                       //!< Flag indicating readiness to send a command.
    int connectionAttempts;                 //!< Number of connection attempts made.
    int maxConnectionAttempts;              //!< Maximum allowed attempts before timeout.
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _DEFINE_PDP_CONTEXT_H_