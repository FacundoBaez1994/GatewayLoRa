//=====[#include guards - begin]===============================================
#ifndef _ACTIVATE_PDP_H_
#define _ACTIVATE_PDP_H_

//==================[Libraries]===============================================
#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "CreateSocket.h"
#include "TransceiverUnavailable.h"

//=====[Declaration of public data types]======================================
class CellularModule;   ///< Forward declaration for CellularModule
struct TcpSocket;       ///< Forward declaration for TcpSocket

//=====[Declaration of public classes]=========================================
/**
 * @class ActivatePDP
 * @brief Transceiver state responsible for activating the PDP context.
 * This state sends the AT command to activate the Packet Data Protocol (PDP) context 
 * and transitions to the socket creation state upon success.
 */
class ActivatePDP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor. Initializes attributes with safe defaults.
     */
    ActivatePDP();
    /**
     * @brief Constructor that receives a reference to the cellular module.
     * 
     * @param mobileModule Pointer to the CellularModule instance.
     */
    ActivatePDP (CellularModule * mobileModule);

    /**
     * @brief Destructor. Resets internal references.
     */
    virtual ~ActivatePDP ();

    /**
     * @brief No operation in this state. Included to satisfy interface.
     */
    virtual void enableTransceiver ();

    /**
     * @brief Exchanges messages with the module to activate PDP.
     * Sends the appropriate AT command and waits for the expected response. On success,
     * transitions to the `CreateSocket` state.
     * @param ATHandler Pointer to the ATCommandHandler object used to send and read AT commands.
     * @param refreshTime Pointer to the NonBlockingDelay object to manage delays between attempts.
     * @param message Unused parameter in this state.
     * @param socketTargetted Unused parameter in this state.
     * @param receivedMessage Unused parameter in this state.
     * @param newDataAvailable Unused parameter in this state.
     * @return CellularTransceiverStatus_t Current status of the transceiver operation.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);

//=====[Declaration of privates atributes]=========================================
private:
    CellularModule * mobileNetworkModule; ///< Pointer to main cellular module
    bool readyToSend;                     ///< Flag indicating if the command is ready to be sent
    int Attempts;                         ///< Number of current attempts
    int maxAttempts;                      ///< Maximum allowed attempts before aborting

//=====[Declaration of privates methods]=========================================
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ACTIVATE_PDP_H_