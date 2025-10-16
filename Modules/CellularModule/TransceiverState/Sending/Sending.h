//=====[#include guards - begin]===============================================

#ifndef _SENDING_H_
#define _SENDING_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "CloseSocket.h"
#include "Receiving.h"


//=====[Declaration of public data types]======================================
class CellularModule;   //!< Forward declaration of the owning module.
struct RemoteServerInformation;   ///< Forward declaration for a struct with infomation over the remote server

//=====[Declaration of public classes]=========================================
/**
 * @class Sending
 * @brief Represents the state responsible for sending data to a TCP Socket on a remote server.
 * This class implements the Sending state in the TransceiverState state machine.
 */
class Sending : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     * Initializes internal flags and counters.
     * Makes the object enable to use
     */
    Sending();

    /**
     * @brief Constructor with CellularModule reference.
     * Makes the object enable to use
     * @param mobileModule Pointer to the owner cellular module.
     */
    Sending (CellularModule * mobileModule);

    /**
     * @brief Destructor.
     * Clears the pointer to the cellular module.
     * it won´t destroy de CellularModule object
     */
    virtual ~Sending ();

    /**
     * @brief Allows enabling the transceiver from an external state.
     */
    virtual void enableTransceiver ();

    /**
     * @brief Transition logic for data transmission. 
     * Handles chunked sending and state transitions based on success/failure.
     * @param ATHandler Pointer to AT command interface.
     * @param refreshTime Pointer to a timer used for retries and pacing.
     * @param message Pointer to the message to be sent.
     * @param serverTargetted Pointer to a struct with information of the destination TCP socket.
     * @param receivedMessage Pointer to buffer for a potential response (unused here).
     * @param newDataAvailable Pointer to flag indicating if new data was received (unused here).
     * @return CellularTransceiverStatus_t Status of the transmission.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable);

private:
    /**
     * @brief Sends a chunk of the given message through the network
     * to a TCP Socket.
     * Handles retries, timeouts, and confirmation of successful transmission.
     * @param ATHandler Pointer to AT command interface.
     * @param refreshTime Pointer to a timer used for retries and pacing.
     * @param message Pointer to the data chunk to send.
     * @param serverTargetted Pointer to a struct with information of the destination TCP socket.
     * @return true if the chunk was sent and acknowledged correctly.
     * @return false if the send failed or timed out.
     */
    bool sendChunck(ATCommandHandler *ATHandler,
    NonBlockingDelay *refreshTime, char *message,  RemoteServerInformation* serverTargette);

    CellularModule * mobileNetworkModule;   //!< Pointer to the owner module
    bool readyToSend;                       //!< Indicates whether a chunk can be sent
    bool transmissionEnable;                //!< Waits for prompt to transmit
    bool watingForConfirmation;             //!< Waiting for confirmation after sending
    int Attempts;                           //!< Retry counter
    int maxAttempts;                        //!< Maximum retry attempts
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _SENDING_H_