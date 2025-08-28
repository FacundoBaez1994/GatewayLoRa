//=====[#include guards - begin]===============================================

#ifndef _CONFIGURE_PDP_FOR_HTTP_H_
#define _CONFIGURE_PDP_FOR_HTTP_H_

//==================[Libraries]===============================================

#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "TransceiverUnavailable.h"
#include "PostHTTP.h"
#include "ConfigureSSL.h"

//=====[Declaration of public data types]======================================
class CellularModule;  ///< Forward declaration of CellularModule
struct TcpSocket;       ///< Forward declaration of TcpSocket

/**
 * @enum PDPConfigStatus_t
 * @brief Internal states for the PDP context configuration sequence.
 */
typedef enum {
    ASSING_PDPCONTEXT_TO_HTTP_STACK, ///< Assign PDP context to HTTP stack
    CONFIGURING_PDP_PROFILE,         ///< Configure PDP profile parameters
    ACTIVATING_PDP_CONTEXT,          ///< Activate PDP context
    QUERING_PDP_CONTEXT,             ///< Query PDP context status
    CONFIGURING_SERVER_DNS,          ///< Configure DNS server addresses
    PING_SERVER_DNS                  ///< Optional: perform DNS query to test resolution
} PDPConfigStatus_t;

//=====[Declaration of public classes]=========================================
/**
 * @class ConfigurePDPForHTTP
 * @brief Transceiver state responsible for setting up the PDP context for HTTP communication.
 * This class sequentially sends AT commands to configure, activate, and verify the
 * PDP context. It also handles DNS configuration and transitions to the SSL configuration
 * state if successful.
 */
class ConfigurePDPForHTTP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor. Initializes an empty instance without an associated module.
     */
    ConfigurePDPForHTTP();

    /**
     * @brief Constructor with module reference.
     * @param mobileModule Pointer to the CellularModule instance.
     */
    ConfigurePDPForHTTP (CellularModule * mobileModule);

    /**
     * @brief Destructor. Resets internal module reference.
     */
    virtual ~ConfigurePDPForHTTP ();

    /**
     * @brief No operation in this state. Required by interface.
     */
    virtual void enableTransceiver ();

    /**
     * @brief Executes the PDP configuration process by sending appropriate AT commands.
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
    int Attempts;                         ///< Number of current retry attempts
    int maxAttempts;                      ///< Maximum number of retry attempts allowed
    PDPConfigStatus_t currentStatus;      ///< Current step in PDP configuration process


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONFIGURE_PDP_FOR_HTTP_H_