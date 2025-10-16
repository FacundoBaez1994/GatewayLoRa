//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_NETWORK_STATUS_H_
#define _CONSULTING_NETWORK_STATUS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingAvailableOperators.h"
#include "Non_Blocking_Delay.h"
#include <string>
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration for circular dependency.
struct CellInformation; //!< Forward declaration for cell data structure.

//=====[Declaration of public classes]=========================================
/**
 * @class ConsultingNetworkStatus
 * @brief State responsible for retrieving current network registration status and cell information.
 * Implements the `ConnectionState` interface as part of the State design pattern.
 * In this state, the module checks whether it is registered to the network and extracts
 * details such as LAC, Cell ID, access technology and registration status. If registration
 * is successful, the state transitions to `ConsultingAvailableOperators`.
 */
class ConsultingNetworkStatus : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    ConsultingNetworkStatus();

    
    /**
     * @brief Constructor with reference to the owning CellularModule.
     * @param mobileModule Pointer to the CellularModule instance.
     */
    ConsultingNetworkStatus(CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~ConsultingNetworkStatus ();

    /**
     * @brief Executes logic to verify network registration and retrieve cell data.
     * If registration is confirmed, information is saved and the state progresses.
     * @param handler Pointer to AT command handler.
     * @param refreshTime Pointer to a non-blocking timer for managing retries.
     * @param currentCellInformation Pointer where extracted data is stored.
     * @return Connection status after attempting to retrieve registration info.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, 
    NonBlockingDelay * refreshTime,
     CellInformation * currentCellInformation);

    /**
     * @brief Not supported in this state.
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
     * @brief No-op. Enabling connection is not required in this state.
     */
    virtual void enableConnection ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Pointer to owning module instance.
    bool readyToSend; //!< Indicates whether the first AT response was processed.
    bool ATFirstResponseRead; //!< Indicates whether cell data has been successfully retrieved.
    bool cellDataRetrived; //!< Indicates whether cell data has been successfully retrieved.
    int lac;  //!< Location Area Code.
    int cellId; //!< Cell Identifier.
    int accessTechnology; //!< Access technology used by the cell (2G, 3G, 4G).
    int registrationStatus; //!< Network registration status code. 
    int connectionAttempts; //!< Number of connection attempts made.
    int maxConnectionAttempts;  //!< Maximum number of retries before failure.
    
//=====[Declaration of privates methods]=========================================
    /**
     * @brief Parses a response to extract registration and cell-related data.
     * Extracts LAC, Cell ID, access technology, and registration status.
     * @param response Raw string response to parse.
     * @return true if data was successfully extracted.
     */
    bool retrivIdCellData (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_NETWORK_STATUS_H_