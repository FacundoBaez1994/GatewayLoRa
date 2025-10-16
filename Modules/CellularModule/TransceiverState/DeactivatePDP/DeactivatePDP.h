//=====[#include guards - begin]===============================================

#ifndef _DEACTIVATE_PDP_H_
#define _DEACTIVATE_PDP_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "TransceiverUnavailable.h"


//=====[Declaration of public data types]======================================
class CellularModule;  ///< Forward declaration of CellularModule
struct RemoteServerInformation;   ///< Forward declaration for a struct with infomation over the remote server

//=====[Declaration of public classes]=========================================
/**
 * @class DeactivatePDP
 * @brief Handles the PDP context deactivation in the cellular module.
 * This class is part of the Transceiver state machine and is responsible
 * for sending a PDP deactivation request to the mobile network module.
 * Depending on whether the previous transmission succeeded, 
 * it signals the next state or status to transition to.
 */
class DeactivatePDP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor with reference to the cellular module.
     * @param mobileModule Pointer to the associated CellularModule.
     * @param transmissionWasASuccess True if the transmission was successful.
     */
    DeactivatePDP (CellularModule * mobileModule, bool transmissionWasASuccess);

    /**
     * @brief Destructor.
     */
    virtual ~DeactivatePDP ();

    /**
     * @brief send At command to the module to deactivate PDP context.
     * @param ATHandler Pointer to the AT command interface.
     * @param refreshTime Pointer to non-blocking delay manager.
     * @param message Not used in this state.
     * @param serverTargetted Not used in this state.
     * @param receivedMessage Not used in this state.
     * @param newDataAvailable Not used in this state.
     * @return CellularTransceiverStatus_t Current status after attempting PDP deactivation.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable);

    /**
     * @brief Enables the transceiver. No operation in this state.
     */
    virtual void enableTransceiver ();
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;       ///< Pointer to the associated cellular module
    bool readyToSend;                           ///< Indicates whether the module is ready to send the command
    bool transmissionWasASuccess;               ///< Indicates whether the previous transmission was successful

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _DEACTIVATE_PDP_H_S