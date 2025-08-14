//=====[#include guards - begin]===============================================
#ifndef _CHECKSUM_GENERATOR_H_
#define  _CHECKSUM_GENERATOR_H_

//==================[Libraries]===============================================
#include "mbed.h"
#include "arm_book_lib.h"
#include "BaseMessageHandler.h"
#include <string.h>
#include <stdio.h>
#include <MbedCRC.h>
 
//=====[Declaration of public data types]======================================
class MessageHandler; ///< Forward declaration for Chain of Responsibility

//=====[Declaration of public classes]=========================================
/**
 * @class ChecksumGenerator
 * @brief Concrete handler that appends a CRC32 checksum to the message.
 * @details This class computes the CRC32 checksum for a given message and
 * appends it (as an 8-character hexadecimal string) at the end. It is intended
 * to ensure integrity during message transmission. It is part of the Chain of
 * Responsibility design pattern.
 */
class ChecksumGenerator : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    /**
     * @brief Construct a new ChecksumGenerator object.
     */
    ChecksumGenerator();

    /**
     * @brief Destroy the ChecksumGenerator object.
     */
    virtual ~ChecksumGenerator ();

    /**
     * @brief Computes and appends a CRC32 checksum to the message.
     * 
     * @param message Pointer to the message buffer (null-terminated string).
     * @param sizeOfMessage Length of the message buffer.
     * @return Status indicating success or type of failure.
     */
    virtual MessageHandlerStatus_t handleMessage (char * message, unsigned int sizeOfMessage) override;
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CHECKSUM_GENERATOR_H_