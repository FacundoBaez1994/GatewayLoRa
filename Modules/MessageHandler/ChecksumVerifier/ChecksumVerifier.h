//=====[#include guards - begin]===============================================
#ifndef _CHECKSUM_VERIFIER_H_
#define _CHECKSUM_VERIFIER_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include <string.h>
#include <stdio.h>
#include <MbedCRC.h>
 
//=====[Declaration of public data types]======================================
class MessageHandler;  ///< Forward declaration for Chain of Responsibility

//=====[Declaration of public classes]=========================================
/**
 * @class ChecksumVerifier
 * @brief Concrete handler that verifies the integrity of a message using CRC32.
 * @details This class implements CRC32 checksum verification as part of a
 * message processing chain. It checks the last 8 characters of the message
 * as a hexadecimal-encoded CRC and compares it with the computed value.
 * It follows the Chain of Responsibility design pattern.
 */
class ChecksumVerifier : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Construct a new ChecksumVerifier object.
     */
    ChecksumVerifier();

    /**
     * @brief Destroy the ChecksumVerifier object.
     */
    virtual ~ChecksumVerifier ();

    /**
     * @brief Verifies CRC32 checksum appended to the message.
     * 
     * @param message Pointer to the message buffer (null-terminated string).
     * @param sizeOfMessage Length of the message buffer.
     * @return Status indicating success or type of failure.
     */
    virtual MessageHandlerStatus_t handleMessage (char* message, unsigned int sizeOfMessage) override;

private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //  _CHECKSUM_VERIFIER_H_