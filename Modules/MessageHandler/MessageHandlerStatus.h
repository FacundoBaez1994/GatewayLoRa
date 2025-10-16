#ifndef _MESSAGE_HANDLER_STATUS_H_
#define _MESSAGE_HANDLER_STATUS_H_

/**
 * @enum MessageHandlerStatus_t
 * @brief Represents the possible outcomes of a message handling operation.
 * @details This enumeration is used by the MessageHandler interface and its
 * implementations to indicate the result of message processing.
 */
typedef enum {
    MESSAGE_HANDLER_STATUS_PROCESSING_MESSAGE,        //< The message is currently being processed.
    MESSAGE_HANDLER_STATUS_PROCESSED,                 //< The message was successfully processed.
       MESSAGE_HANDLER_STATUS_ERROR_NULL_PTR,         //< an uninitialize buffer was recepted as param.
    MESSAGE_HANDLER_STATUS_ERROR_PACKET_TOO_SMALL,    //< The input packet was too small to be valid.
    MESSAGE_HANDLER_STATUS_ERROR_BUFFER_TOO_SMALL,    //< The output buffer is too small to hold the response.
    MESSAGE_HANDLER_STATUS_CHECKSUM_ERROR,            //< The message has an invalid checksum.
    MESSAGE_HANDLER_STATUS_FAIL_TO_COMPUTE_CHECKSUM,  //< Failed to compute the checksum during validation.
    MESSAGE_HANDLER_STATUS_AUTHENTIFICATION_MISSMATCH //< The message failed authentication (e.g., token mismatch).
} MessageHandlerStatus_t;

#endif // _MESSAGE_HANDLER_STATUS_H_