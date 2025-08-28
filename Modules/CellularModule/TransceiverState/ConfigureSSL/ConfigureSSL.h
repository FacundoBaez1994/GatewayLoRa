//=====[#include guards - begin]===============================================
#ifndef _CONFIGURE_SSL
#define _CONFIGURE_SSL

//==================[Libraries]===============================================
#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "TransceiverUnavailable.h"
#include "PostHTTP.h"

//=====[Declaration of public data types]======================================
class CellularModule;  ///< Forward declaration of CellularModule
struct TcpSocket;       ///< Forward declaration of TcpSocket

/**
 * @enum SSLConfigStatus_t
 * @brief Internal states for configuring SSL parameters on the cellular module.
 */
typedef enum {
    SETTING_SSL_CONTEXT,       ///< Setting the SSL context ID
    SETTING_SSL_VERSION,       ///< Setting the SSL version
    SETTING_SSL_CIPHER_SUITE,  ///< Setting the SSL cipher suite
    SETTING_SSL_LEVEL          ///< Setting the SSL security level
} SSLConfigStatus_t;

//=====[Declaration of public classes]=========================================
/**
 * @class ConfigureSSL
 * @brief State that configures SSL parameters on the cellular module.
 * This class belongs to the transceiver state machine. It sends the necessary
 * AT commands to prepare SSL context and settings, and transitions to the 
 * PostHTTP state after successful configuration.
 */
class ConfigureSSL : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     * Initializes without associating a CellularModule.
     */
    ConfigureSSL();

    /**
     * @brief Constructor with CellularModule association.
     * @param mobileModule Pointer to the CellularModule instance.
     */
    ConfigureSSL (CellularModule * mobileModule);

    /**
     * @brief Destructor. Releases resources and resets pointer.
     */
    virtual ~ConfigureSSL ();

    /**
     * @brief Does nothing in this state. Required by interface.
     */
    virtual void enableTransceiver ();

    /**
     * @brief Executes SSL configuration sequence via AT commands.
     * @param ATHandler Pointer to the AT command handler.
     * @param refreshTime Pointer to the non-blocking delay manager.
     * @param message Not used in this state.
     * @param socketTargetted Not used in this state.
     * @param receivedMessage Not used in this state.
     * @param newDataAvailable Not used in this state.
     * @return CellularTransceiverStatus_t Transceiver status after processing.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, TcpSocket * socketTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule; ///< Pointer to the cellular module
    bool readyToSend;                     ///< Flag indicating readiness to send the next command
    int Attempts;                         ///< Number of attempts already made
    int maxAttempts;                      ///< Maximum allowed attempts
    SSLConfigStatus_t currentStatus;      ///< Current step in SSL configuration


//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CONFIGURE_PDP_FOR_HTTP_H_