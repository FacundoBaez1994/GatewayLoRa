//=====[#include guards - begin]===============================================

#ifndef _POST_HTTP_H_
#define _POST_HTTP_H_

//==================[Libraries]===============================================

#include "CellularModule.h"
#include "mbed.h"
#include "arm_book_lib.h"
#include "ATCommandHandler.h"
#include "TransceiverState.h"
#include "Non_Blocking_Delay.h"
#include "TransceiverUnavailable.h"
#include "DeactivatePDP.h"
#include "JWTManager.h"


//=====[Declaration of public data types]======================================
class CellularModule; 
struct RemoteServerInformation; ///< Forward declaration for a struct with infomation over the remote server

/**
 * @enum PostResult_t
 * @brief Represents the result of an HTTP POST operation.
 */
typedef enum {
    POST_OK,
    KEEP_TRYING_TO_POST,
    POST_FAILURE,
} PostResult_t;

/**
 * @enum PostStatus_t
 * @brief Represents the internal status of the HTTP POST state machine.
 */
typedef enum {
    SETTING_URL,
    POSTING_DATA,
    READING_DATA,
    DECODING_DATA,
} PostStatus_t;

//=====[Declaration of public classes]=========================================

/**
 * @class PostHTTP
 * @brief State class that handles HTTP POST operations in a transceiver state machine.
 * This class implements a transceiver state responsible for managing the lifecycle
 * of an HTTP POST request over a cellular network, including setting the URL, sending
 * the payload, reading the response, and decoding the content.
 */
class PostHTTP : public TransceiverState {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Default constructor.
     */
    PostHTTP();

    /**
     * @brief Constructor with cellular module reference.
     * @param mobileModule Pointer to the cellular module.
     */
    PostHTTP (CellularModule * mobileModule);

    
    /**
     * @brief Destructor.
     */
    virtual ~PostHTTP ();


    /**
     * @brief Enables the transceiver (no-op in this state).
     */
    virtual void enableTransceiver ();

    /**
     * @brief Main method used for making a HTTP POST request in order to
     * upload data to the network server. It also has the capacity of
     * retriev the POST response deconding and verifing it.
     * @param ATHandler Pointer to the AT command handler interface.
     * @param refreshTime Pointer to the non-blocking delay timer.
     * @param message Null-terminated message string to be sent.
     * @param serverTargetted Pointer to a struct with information of the remote server.
     * @param receivedMessage Buffer to store any received message.
     * @param newDataAvailable Pointer to flag indicating whether new data was received.
     * @return CellularTransceiverStatus_t Status of the transceiver after attempting the exchange.
     */
    virtual CellularTransceiverStatus_t exchangeMessages (ATCommandHandler * ATHandler,
    NonBlockingDelay * refreshTime, char * message, RemoteServerInformation* serverTargetted,
     char * receivedMessage, bool * newDataAvailable);
private:
//=====[Declaration of privates atributes]=========================================
    CellularModule * mobileNetworkModule;  ///< Pointer to the associated cellular module.
    bool readyToSend;                      ///< Indicates if the system is ready to initiate sending.
    int Attempts;                          ///< Counter for retry attempts.
    int maxAttempts;                       ///< Maximum number of retry attempts.
    PostStatus_t currentStatus;            ///< Current internal status of the HTTP POST state machine.
   // JWTManager * jwt;                      ///< Pointer to the JWT decoder utility.

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Checks the result of an HTTP POST operation from a response buffer.
     * @param responseBuffer The raw response string from the module.
     * @param dataLen Pointer to store the length of the received payload, if available.
     * @return PostResult_t The result of the check.
     */
    PostResult_t checkHTTPPostResult(char * responseBuffer, int * dataLen);
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _POST_HTTP_H_