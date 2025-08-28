//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_AVAILABLE_OPERATORS_H_
#define _CONSULTING_AVAILABLE_OPERATORS_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "RetrievingTimeAndDate.h"
#include "Non_Blocking_Delay.h"
#include <string>
#include "CellularModule.h"

//=====[Declaration of defines]========================================
#define FREQ_BAND_STRING_LEN 20

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration to resolve circular dependency.
struct CellInformation; //!< Forward declaration for cell information structure.

//=====[Declaration of public classes]=========================================
/**
 * @class ConsultingAvailableOperators
 * @brief State responsible for consulting operator and network information.
 * Implements the `ConnectionState` interface as part of the State design pattern.
 * In this state, the module queries the currently available network operator and
 * extracts parameters such as MCC, MNC, frequency band and channel. If successful,
 * it transitions to the next state in the connection process.
 */
class ConsultingAvailableOperators : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
    * @brief Default constructor.
    */
    ConsultingAvailableOperators();

    /**
     * @brief Constructor with reference to the CellularModule.
     * @param mobileModule Pointer to the owning CellularModule.
    */
    ConsultingAvailableOperators (CellularModule * mobileModule);

    /**
     * @brief Destructor.
    */
    virtual ~ConsultingAvailableOperators ();

    /**
     * @brief Collects information over the operator and the associated BTS.
     * Gets the mnc, mcc codes, Access Technology, the channel and band asignated by the BTS
     * @param handler Pointer to command handler.
     * @param refreshTime Pointer to timing delay.
     * @param currentCellInformation Pointer to current cell information structure.
     * @return the status of the connection attempt.
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
     * @brief No-op. Enabling connection is not applicable in this state.
     */
    virtual void enableConnection ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Reference to owning CellularModule.
    bool readyToSend; //!< Flag used to indicate if the AT command is ready to send.
    bool operatorsInformationRetrived; //!< Flag used to indicate if the operators info was parsed with success.
    int mcc; //!< Mobile Country Codes 
    int mnc; //!< Mobile Network Code
    char * currentOperator; //!< Mobile Network Operator
    int channel; //!< Channel Asignated
    char band [FREQ_BAND_STRING_LEN]; //!< frecuency band asignated
    int connectionAttempts;   //!< number of tries
    int maxConnectionAttempts;  //!< max number of attempts
//=====[Declaration of privates methods]=========================================
    /**
     * @brief parses the AT command response and retrieves the useful data (mcc, mnc codes).
     * channel, band asignated, e.g
     * @param response raw string provided by the AT command response.
     * @return returns true if the string was successfuly parsed.
     */
    bool retrivOperatorsInformation (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_AVAILABLE_OPERATORS_H_