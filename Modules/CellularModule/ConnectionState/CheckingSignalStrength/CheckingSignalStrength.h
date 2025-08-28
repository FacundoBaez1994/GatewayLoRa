//=====[#include guards - begin]===============================================

#ifndef _CHECKING_SIGNAL_STRENGTH_H_
#define _CHECKING_SIGNAL_STRENGTH_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingIMEI.h"
#include "Non_Blocking_Delay.h"
#include <string>
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration to resolve circular dependency.
struct CellInformation; //!< Forward declaration for cell information structure.

//=====[Declaration of public classes]=========================================
/**
 * @class CheckingSignalStrength
 * @brief State responsible for checking signal strength via AT command.
 * Implements the `ConnectionState` interface as part of the State design pattern.
 * This state queries the signal quality of the cellular network and determines
 * whether it is suitable for continuing the connection sequence. If successful,
 * it transitions to `ConsultingIMEI`.
 */
class CheckingSignalStrength : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    CheckingSignalStrength ();

    /**
     * @brief Constructor with a pointer to the cellular module.
     * @param mobileModule Pointer to the CellularModule instance using this state.
     */
    CheckingSignalStrength(CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~CheckingSignalStrength ();

    /**
     * @brief Attempts to retrieve signal strength and evaluates if it's above the acceptable minimum .
     * Sends the corresponding command and parses the response to extract the signal level.
     * If the level is above a configured threshold, the state transitions to the next.
     * @param handler Pointer to the AT command handler.
     * @param refreshTime Non-blocking delay used for timing between retries.
     * @param currentCellInformation Pointer where the current signal level will be stored.
     * @return CellularConnectionStatus_t representing the current connection status.
     */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);

    /**
     * @brief Neighbor retrieval not supported in this state.
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
     * @brief No-op. Enabling connection is not applicable in this state.
     */
    virtual void enableConnection ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Reference to owning CellularModule.
    bool readyToSend; //!< Flag that Indicates readiness to send the next AT command.
    bool ATFirstResponseRead; //!< Flag that Indicates whether the first response from AT was read.
    bool signalLevelRetrived;  //!< Flag indicating if signal level was successfully parsed.
    float signalLevel; //!< Last measured signal level in dBm.
    int connectionAttemptsATResponse; //!< Retry counter for AT command response timeout.
    int maxConnectionAttemptsATResponse; //!< Maximum allowed retries for AT response.
    int connectionAttemptsSignal; //!< Retry counter for low signal condition.
    int maxConnectionAttemptsSignal; //!< Maximum retries allowed for weak signal.
    
//=====[Declaration of privates methods]=========================================
    /**
     * @brief Checks if the AT response matches the expected format and extracts signal level.
     * @param response The response string from the module.
     * @return true if signal level was successfully parsed and stored.
     */
    bool checkExpectedResponse (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _CHECKING_SIGNAL_STRENGTH_H_