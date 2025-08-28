//=====[#include guards - begin]===============================================

#ifndef _CONSULTING_IMEI_H_
#define _CONSULTING_IMEI_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "ConsultingSIMCardStatus.h"
#include "Non_Blocking_Delay.h"
#include <string>
#include "CellularModule.h"

//=====[Declaration of public data types]======================================
class CellularModule; //!< Forward declaration to resolve circular dependency.
struct CellInformation; //!< Forward declaration for cell information structure.

//=====[Declaration of public classes]=========================================

/**
 * @class ConsultingIMEI
 * @brief State responsible for retrieving the IMEI of the cellular module.
 * 
 * This class is part of the State design pattern and represents a step in the
 * connection process where the device identifier (IMEI) is queried and stored.
 * If successful, the state transitions to `ConsultingSIMCardStatus`.
 */
class ConsultingIMEI : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
    * @brief Default constructor.
    */
    ConsultingIMEI();

    /**
     * @brief Constructor with reference to the CellularModule.
     * @param mobileModule Pointer to the owning CellularModule.
    */
    ConsultingIMEI(CellularModule * mobileModule);

    /**
     * @brief Destructor.
    */
    virtual ~ConsultingIMEI ();
    
    /**
     * @brief Executes logic to retrieve the IMEI from the module.
     * If the IMEI is successfully parsed, it is stored in the given 
     * `CellInformation` structure and the state transitions. If this process
     * is sucessful it will progress into the next state.
     * @param handler Pointer to AT command handler.
     * @param refreshTime Pointer to non-blocking delay for retry control.
     * @param currentCellInformation Output structure to store retrieved IMEI.
     * @return Status representing progress or failure of the retrieval process.
    */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
    CellInformation * currentCellInformation);


    /**
     * @brief No-op in this state.
     */
    virtual void enableConnection ();

    /**
     * @brief Neighbor cell retrieval not supported in this state.
     * @param handler Pointer to ATCommandHandler for communication.
     * @param refreshTime Pointer to non-blocking delay.
     * @param neighborsCellInformation Vector of structures of DataCell where results will be stored.
     * @param numberOfNeighbors Maximum number of neighbors to retrieve.
     * @return Always returns false.
    */
    virtual bool retrivNeighborCellsInformation (ATCommandHandler * handler,
    NonBlockingDelay * refreshTime, std::vector<CellInformation*> &neighborsCellInformation, 
    int numberOfNeighbors);
    
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Pointer to the cellular module using this state.
    bool readyToSend; //!< Flag to indicate readiness for retry the request.
    bool ATFirstResponseRead; //!< Flag to track if a first response was processed.
    bool IMEIRetrived; //!< Flag indicating if IMEI was successfully retrieved.
    long long int IMEI; //!< Stores the retrieved IMEI value.
    int connectionAttempts; //!< Retry counter.
    int maxConnectionAttempts; //!< Maximum allowed retries before failure.

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Parses a response string to extract the IMEI.
     * Validates format and converts the IMEI string into a numeric value.
     * @param response Raw string response from the module.
     * @param value Reference to store parsed IMEI.
     * @return true if the IMEI was successfully parsed and is valid.
     */
    bool RetrivIMEI (char *response, long long int &value);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CONSULTING_IMEI_H_