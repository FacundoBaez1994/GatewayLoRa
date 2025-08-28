//=====[#include guards - begin]===============================================

#ifndef _CONNECTED_STATE_H_
#define _CONNECTED_STATE_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "CellularModule.h"
#include <string>
#include <vector>
#include <cstring>  

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration to avoid circular dependency.
struct CellInformation; //!< Forward declaration for cell data.

//=====[Declaration of public classes]=========================================
/**
 * @class ConnectedState
 * @brief Represents the state in which the module is connected to the network.
 * Implements the `ConnectionState` interface as part of the State design pattern.
 * This state assumes the cellular module is connected to the mobile network and
 * allows retrieval of neighbor cell information. It in this operational state
 * the device is ready to start the data transmission sequence.
 */
class ConnectedState : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    ConnectedState();

    /**
     * @brief Constructor with reference to the cellular module.
     * @param mobileModule Pointer to the owning CellularModule.
     */
    ConnectedState (CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~ConnectedState ();

    /**
     * @brief Returns connection status without initiating a new connection.
     * Since the module is already connected, this method only reports status.
     * @param handler Pointer to command handler.
     * @param refreshTime Pointer to timing delay.
     * @param currentCellInformation Pointer to current cell information structure.
     * @return Always returns `CELLULAR_CONNECTION_STATUS_CONNECTED_TO_NETWORK` because the
     * its already connect to the mobile network.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler,
     NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);

    /**
     * @brief Retrieves information about nearby cells.
     * Parses responses to build a list of neighboring cells and appends them
     * to the given vector.
     * @param handler Pointer to ATCommandHandler.
     * @param refreshTime Pointer to timing object.
     * @param neighborsCellInformation Output vector of CellInformation pointers.
     * @param numberOfNeighbors number of neighbors retrieved.
     * @return true if the retrieval process completes or times out, false otherwise.
     * @note if the device couldn't retrieved the neighbor BTS data it would let the vector empty
     */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);

    /**
     * @brief No-op in this state. Connection is already enabled and stabished.
     */
    virtual void enableConnection ();

private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Reference to the cellular module using this state.
    bool readyToSend; //!< Internal flag to manage message sending intervals.
    bool enableTransceiver; //!< Unused in this state; reserved for future use.
    //=====[Declaration of privates methods]=========================================
    /**
     * @brief Parses a response to extract operator codes (MCC and MNC).
     * @param response Raw response string to parse.
     * @param mcc Pointer to store Mobile Country Code.
     * @param mnc Pointer to store Mobile Network Code.
     * @return true if both codes were extracted successfully.
     */
    bool retrivOperatorsCodes (const char *response, int * mcc, int * mnc);

    /**
     * @brief Parses a response to extract cell parameters.
     * Extracts technology generation, cell ID, location area code, and signal level.
     * @param response Raw response string to parse.
     * @param tech Pointer to technology type (2G/3G/4G).
     * @param idCell Pointer to cell identifier.
     * @param lac Pointer to location area code.
     * @param prx Pointer to signal level (in dBm).
     * @return true if parsing was successful.
     */
    bool retrivCellData (const char *response, int * tech, int * idCell, int * lac, float * prx);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONNECTED_STATE_H_