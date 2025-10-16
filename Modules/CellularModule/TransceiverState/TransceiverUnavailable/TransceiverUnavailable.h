//=====[#include guards - begin]===============================================

#ifndef _TRANSCEIVER_UNAVAILABLE_H_
#define _TRANSCEIVER_UNAVAILABLE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "ActivatePDP.h"
#include "ConfigurePDPForHTTP.h"
#include "Non_Blocking_Delay.h"


//=====[Declaration of public data types]======================================
class CellularModule; ///< Forward declaration of the cellular module class.
struct RemoteServerInformation; ///< Forward declaration for a struct with infomation over the remote server

//=====[Declaration of public classes]=========================================
/**
* @class TransceiverUnavailable
* @brief State representing a transceiver that is currently not ready for communication.
* This state is typically used when the transceiver needs to be configured
* before message exchange can begin. It implements the State design pattern.
*/
class TransceiverUnavailable : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================

    /**
     * @brief Default constructor.
     */
    TransceiverUnavailable();

    /**
     * @brief Constructor that links the state to the cellular module.
     * @param mobileModule Pointer to the cellular module instance.
     */
    TransceiverUnavailable (CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~TransceiverUnavailable ();


    /**
     * @brief Handles message exchange. In this state, message exchange is not possible.
     * @param refreshTime Not used in this state.
     * @param message Not used in this state.
     * @param serverTargetted Not used in this state.
     * @param receivedMessage Not used in this state.
     * @param newDataAvailable Not used in this state
     * @returns Always returns a status indicating the transceiver is unavailable.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable);

    /**
     * @brief Enables the transceiver, triggering a state change to prepare it for communication.
     */
    virtual void enableTransceiver ();
private:
    CellularModule * mobileNetworkModule;  //!< Pointer to the owning cellular module.
    bool readyToSend;                       //!< Flag indicating if the transceiver is ready to send data.
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONNECTED_STATE_H_