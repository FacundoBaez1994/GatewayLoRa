//=====[#include guards - begin]===============================================

#ifndef _TRANSCEIVER_STATE_H_
#define _TRANSCEIVER_STATE_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "Non_Blocking_Delay.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverStatus.h"

//=====[Declaration of public data types]======================================
struct TcpSocket; //!< Forward declaration for TCP socket structure used in communication.

//=====[Declaration of public classes]=========================================
/**
 * @class TransceiverState
 * @brief Interface for implementing different states in the transceiver behavior.
 * This abstract class defines the methods that all transceiver states must implement,
 * allowing the system to switch dynamically between behaviors using the State Design Pattern.
 */
class TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Handles message exchange in the current transceiver state.
     * @param ATHandler Pointer to the AT command handler.
     * @param refreshTime Pointer to the non-blocking delay used for timing control.
     * @param message Message to be sent.
     * @param socketTargetted Target TCP socket to be used for sending/receiving data.
     * @param receivedMessage Buffer to store received message.
     * @param newDataAvailable Pointer to a flag set to true when new data is received.
     * @returns Status of the transceiver after the attempt to exchange messages.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);

    /**
     * @brief Enables the transceiver to begin or resume communication.
     */
    virtual void enableTransceiver ();
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CELLULAR_STATE_H_