//=====[#include guards - begin]===============================================
#ifndef _RETRIEVING_TIME_AND_DATE_H_
#define _RETRIEVING_TIME_AND_DATE_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "ConnectionState.h"
#include "Non_Blocking_Delay.h"
#include "AttachingToPacketService.h"
#include <string>
#include "CellularModule.h"
#include "timeStampTools.h"

//=====[Declaration of defines]========================================
#define BUFFER_DATE_LEN 20
#define BUFFER_TIME_LEN 20

//=====[Declaration of public data types]======================================
class CellularModule;         //!< Forward declaration of the owning module.
struct CellInformation;       //!< Structure holding cellular parameters.

//=====[Declaration of public classes]=========================================
/**
 * @class RetrievingTimeAndDate
 * @brief State responsible for retrieving the current network time and date.
 * This state queries the network to obtain the current timestamp, updates the local RTC,
 * and transitions to the packet service attachment state.
 */
class RetrievingTimeAndDate : public ConnectionState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    RetrievingTimeAndDate();

    /**
     * @brief Constructor with reference to the CellularModule.
     * @param mobileModule Pointer to the owning CellularModule.
    */
    RetrievingTimeAndDate (CellularModule * mobileModule);

    /**
     * @brief Destructor.
    */
    virtual ~RetrievingTimeAndDate();

    /**
     * @brief Attempts to retrieve the current network-provided timestamp.
     * On success, it stores the value and updates the system RTC.
     * @param handler Pointer to the AT command handler.
     * @param refreshTime Pointer to the non-blocking delay used for retry control.
     * @param currentCellInformation Structure to update with the retrieved timestamp.
     * @return CellularConnectionStatus_t Status of the connection process.
    */
    virtual CellularConnectionStatus_t connect (ATCommandHandler * handler, NonBlockingDelay * refreshTime,
     CellInformation * currentCellInformation);

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

    /**
     * @brief No action required in this state.
     */
    virtual void enableConnection ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;   //!< Pointer to the owning CellularModule.
    bool readyToSend;                       //!< Indicates if the command is ready to be sent.
    bool timeAndDateRetrived;               //!< True if timestamp has been successfully retrieved.
    char time [BUFFER_TIME_LEN];            //!< Buffer for time in HHMMSS format.
    char date [BUFFER_DATE_LEN];            //!< Buffer for date in DDMMYY format.
    int connectionAttempts;                 //!< Number of attempts made.
    int maxConnectionAttempts;              //!< Maximum number of attempts before failure.
//=====[Declaration of privates methods]=========================================
    /**
     * @brief Extracts and parses the date and time from the network response.
     * @param response Raw response string to parse.
     * @return true if the response was successfully parsed, false otherwise.
     */
    bool retrieveNetworkTime (char *response);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _RETRIEVING_TIME_AND_DATE_H_