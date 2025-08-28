//=====[#include guards - begin]===============================================
#ifndef _CLOSE_SOCKET_H_
#define _CLOSE_SOCKET_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "DeactivatePDP.h"


//=====[Declaration of public data types]======================================
class CellularModule;  ///< Forward declaration of CellularModule
struct TcpSocket;       ///< Forward declaration of TcpSocket

//=====[Declaration of public classes]=========================================
/**
 * @class CloseSocket
 * @brief Transceiver state responsible for closing the TCP socket after data transmission.
 * This state attempts to gracefully close the socket and then transitions to PDP deactivation.
 */
class CloseSocket : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor with module reference and transmission result.
     * @param mobileModule Pointer to the CellularModule instance.
     * @param transmissionWasASuccess Indicates whether the transmission was successful.
     */
    CloseSocket(CellularModule * mobileModule, bool transmissionWasASuccess);

    /**
     * @brief Destructor. Resets internal module reference.
     */
    virtual ~CloseSocket ();

    /**
     * @brief No operation in this state. Required by interface.
     */
    virtual void enableTransceiver ();

    /**
     * @brief Attempts to close the socket connection via AT commands.
     * @param ATHandler Pointer to the AT command handler.
     * @param refreshTime Pointer to a non-blocking delay object for retries.
     * @param message Not used in this state.
     * @param socketTargetted Not used in this state.
     * @param receivedMessage Not used in this state.
     * @param newDataAvailable Not used in this state.
     * @return CellularTransceiverStatus_t Current status of the transceiver process.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
     
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; ///< Reference to the main cellular module
    bool readyToSend;                     ///< Indicates if the next command is ready to be sent
    bool transmissionWasASuccess;        ///< Tracks if transmission was successful
    int Attempts;                         ///< Number of current retry attempts
    int maxAttempts;                      ///< Maximum number of retry attempts allowed

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _ACTIVATE_PDP_H_