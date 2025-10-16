//=====[#include guards - begin]===============================================
#ifndef _RECEIVING_H_
#define _RECEIVING_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "Sending.h"
#include "CloseSocket.h"

//=====[Declaration of public data types]======================================
class CellularModule;   ///!< Forward declaration of the owning module.a
struct RemoteServerInformation;   ///< Forward declaration for a struct with infomation over the remote server

//=====[Declaration of public classes]=========================================
/**
 * @class Receiving
 * @brief Represents the Receiving state in the cellular transceiver state machine.
 * Responsible for retrieving messages from the TCP of a remote server.
 */
class Receiving : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    Receiving();

    /**
     * @brief Constructor with a pointer to the owner CellularModule.
     * @param mobileModule Pointer to the cellular module that owns this state.
     */
    Receiving (CellularModule * mobileModule);

    /**
     * @brief Destructor.
     */
    virtual ~Receiving ();

    /**
     * @brief Enables the transceiver (no effect in this state).
     */
    virtual void enableTransceiver ();

    /**
     * @brief Manages the reception of incoming data from the network.
     * @param ATHandler Pointer to AT command interface.
     * @param refreshTime Pointer to non-blocking delay used for retries.
     * @param message Pointer to the outgoing message (not used in this state).
     * @param serverTargetted Pointer to a struct with information over the remote server.
     * @param receivedMessage Pointer to the buffer to store received message.
     * @param newDataAvailable Pointer to a flag set to true if new data is received.
     * @return CellularTransceiverStatus_t Resulting transceiver status.
     */
    virtual  CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable);

private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; //!< Pointer to the owning module.
//=====[Declaration of privates methods]===========================================
    /**
     * @brief Verifies if the given response contains retrievable data.
     * @param response Raw AT response string.
     * @param retrivMessage Output buffer for extracted message (not used).
     * @return true if response contains a valid message, false otherwise.
     */
    bool checkResponse (char * response, char * retrivMessage);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _RECEIVING_H_