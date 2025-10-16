//=====[#include guards - begin]===============================================
#ifndef _CREATE_SOCKET_H_
#define _CREATE_SOCKET_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "Sending.h"
#include "CloseSocket.h"

//=====[Declaration of public data types]======================================
class CellularModule;  ///< Forward declaration of CellularModule
struct RemoteServerInformation;  ///< Forward declaration for a struct with infomation over the remote server

//=====[Declaration of public classes]=========================================
/**
 * @class CreateSocket
 * @brief State responsible for creating a TCP socket on the cellular module.
 * 
 * This class belongs to the transceiver state machine and attempts to create
 * a TCP socket using parameters from a `TcpSocket` structure. Upon success, it 
 * transitions to the `Sending` state.
 */
class CreateSocket : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     * Initializes internal state without a reference to a CellularModule.
     */
    CreateSocket();

     /**
     * @brief Constructor with CellularModule association.
     * @param mobileModule Pointer to the CellularModule instance.
     */
    CreateSocket (CellularModule * mobileModule);

    /**
     * @brief Destructor. Clears reference to the associated CellularModule.
     */
    virtual ~CreateSocket ();

   /**
     * @brief Does nothing in this state. Required by interface.
     */
    virtual void enableTransceiver ();

    /**
     * @brief Sends AT commnads to the asociated mobile device
     * Attempting to create a TCP socket. In success progress into the
     * Sending State.
     * @param ATHandler Pointer to the AT command interface.
     * @param refreshTime Pointer to the non-blocking delay manager.
     * @param message Not used in this state.
     * @param serverTargetted structure containing the socket info (IP and port info).
     * @param receivedMessage Not used in this state.
     * @param newDataAvailable Not used in this state.
     * @return CellularTransceiverStatus_t Transceiver state after the attempt.
     */
    virtual  CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable);

private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; ///< Pointer to the associated cellular module
    bool readyToSend;                     ///< Flag indicating if the command can be sent
    int connectionAttempts;               ///< Current number of socket creation attempts
    int maxConnectionAttempts;            ///< Maximum number of allowed attempts before failure

//=====[Declaration of privates methods]=========================================
};

//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CREATE_SOCKET_H_