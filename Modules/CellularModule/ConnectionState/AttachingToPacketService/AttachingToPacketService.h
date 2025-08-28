//=====[#include guards - begin]===============================================

#ifndef _ATTACHING_TO_PACKET_SERVICE_H_
#define _ATTACHING_TO_PACKET_SERVICE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "DefinePDPContext.h"
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration for circular dependency.
struct CellInformation; //!< Forward declaration for cell information structure.


//=====[Declaration of public classes]=========================================
/**
 * @class AttachingToPacketService
 * @brief State representing the process of attaching to the packet service domain.
 * Implements the `ConnectionState` interface as part of the State design pattern.
 * This state attempts to attach the cellular module to the packet-switched domain
 * using AT commands. Upon success, it transitions to the next connection state.
 */
class AttachingToPacketService : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor Initializes internal state.
     */
    AttachingToPacketService ();

    /**
     * @brief Constructor with reference to the CellularModule.
     * @param mobileModule Pointer to the owning CellularModule instance.
     */
    AttachingToPacketService (CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~AttachingToPacketService ();

    /**
     * @brief Attempts to attach to the packet-switched service.
     * Sends the AT command to the cellular module and waits for a valid response.
     * If successful, transitions to the `DefinePDPContext` state.
     * @param handler Pointer to ATCommandHandler for sending commands.
     * @param refreshTime Pointer to non-blocking delay for timing retries.
     * @param currentCellInformation Not used in this state.
     * @return Status code representing result of the attachment attempt.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);

    /**
     * @brief Retrieves neighboring cell information (not supported in this state).
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay.
     * @param neighborsCellInformation Vector of structures of DataCell where results will be stored.
     * @param numberOfNeighbors Maximum number of neighbors to retrieve.
     * @return Always returns false.
     */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);

    /**
     * @brief Enables the connection process (no-op in this state).
     */
    virtual void enableConnection ();

private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Reference to the cellular module using this state.
    bool readyToSend; //!< Flag indicating if the module is ready to send the AT command.
    int connectionAttempts; //!< Current number of connection attempts made. Surpassed a certain limit the state will return fail status.
    int maxConnectionAttempts; //!< Maximum allowed attempts before failing.

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _ATTACHING_TO_PACKET_SERVICE_H_