//=====[#include guards - begin]===============================================

#ifndef _IDLE_STATE_H_
#define _IDLE_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "CheckingSignalStrength.h"
#include "CellularModule.h"


//=====[Declaration of public data types]======================================
class CellularModule;         //!< Forward declaration of the owning module.
struct CellInformation;       //!< Structure holding cellular parameters.


//=====[Declaration of public classes]=========================================
/**
 * @class IdleState
 * @brief Initial state where the module is expected to be responsive.
 * This state checks whether the cellular module is properly connected and ready to receive commands.
 * On successful response, it transitions to the next state.
 */
class IdleState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    IdleState ();

    /**
     * @brief Constructor with reference to the owning CellularModule.
     * @param mobileModule Pointer to the CellularModule.
    */
    IdleState (CellularModule * mobileModule);

    /**
     * @brief Destructor.
    */
    virtual ~IdleState ();

    /**
     * @brief Verifies if the module is responsive. If so, transitions to the signal check state.
     * @param handler Pointer to the AT command handler.
     * @param refreshTime Pointer to the non-blocking delay handler.
     * @param currentCellInformation Pointer to the structure holding current cell data.
     * @return CellularConnectionStatus_t Current status of the connection process.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);

    /**
     * @brief No action required in this state.
     */
    virtual void enableConnection ();

    /**
     * @brief Not applicable for this state.
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay.
     * @param neighborsCellInformation Vector of structures of DataCell where results will be stored.
     * @param numberOfNeighbors Maximum number of neighbors to retrieve
     * @return Always returns false.
     */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
private:
    CellularModule * mobileNetworkModule;       //!< Pointer to the owning CellularModule.
    bool readyToSend;                           //!< Flag indicating if the command can be sent.
    int connectionAttempts;                     //!< Current number of connection attempts.
    int maxConnectionAttempts;                  //!< Maximum allowed number of attempts.
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_