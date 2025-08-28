//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_SIMCARD_STATUS_H_
#define _CONSULTING_SIMCARD_STATUS_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingNetworkStatus.h"
#include "Non_Blocking_Delay.h"
#include <string>
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule;     //!< Forward declaration of the owning module.
struct CellInformation;   //!< Structure holding cellular parameters.

//=====[Declaration of public classes]=========================================
/**
 * @class ConsultingSIMCardStatus
 * @brief State responsible for checking if a SIM card is present and ready.
 * This class is part of the State design pattern for managing cellular connectivity.
 * If a SIM is detected and ready, the module transitions to the next connectiojn state.
 * If no SIM is detected, it may attempt to switch SIM slots or reboot the module.
 */
class ConsultingSIMCardStatus : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    ConsultingSIMCardStatus();
    
    /**
     * @brief Constructor with reference to the owning CellularModule.
     * @param mobileModule Pointer to the parent module managing connection states.
     */
    ConsultingSIMCardStatus(CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~ConsultingSIMCardStatus ();

    /**
     * @brief Attempts to verify the presence and readiness of a SIM card.
     * This function implements the logic for detecting SIM cards. If a valid SIM is found,
     * the system transitions to the next state. Otherwise, it may switch SIM slots or reboot.
     * @param handler Pointer to the AT command handler.
     * @param refreshTime Pointer to a non-blocking delay manager.
     * @param currentCellInformation Pointer to the structure storing current cell info.
     * @return Current status of the cellular connection.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler,
     NonBlockingDelay * refreshTime,
     CellInformation * currentCellInformation);

    /**
     * @brief This state does not support retrieving neighboring cell information.
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
     * @brief No effect in this state. Required by interface.
     */
    virtual void enableConnection ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;   //!< Reference to the parent module.
    bool readyToSend;                       //!< Indicates readiness to send a command.
    bool ATFirstResponseRead;               //!< Tracks if the first response was handled.
    bool simCardDetected;                   //!< True if a SIM card was detected as ready.
    int connectionAttempts;                 //!< Number of attempts made so far.
    int maxConnectionAttempts;              //!< Maximum allowed connection attempts.
//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONSULTING_SIMCARD_STATUS_H_