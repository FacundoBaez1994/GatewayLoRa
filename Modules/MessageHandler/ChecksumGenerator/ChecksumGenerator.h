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
class MessageHandler; //debido a declaracion adelantada
//struct CellInformation;

//=====[Declaration of public classes]=========================================
/*
 *  Abstract class 
 * 
 */
class ChecksumGenerator : public BaseMessageHandler {
public:
//=====[Declaration of public methods]=========================================
    ChecksumGenerator();
    virtual ~ChecksumGenerator ();
    virtual MessageHandlerStatus_t handleMessage (char * message) override;
    //virtual MessageHandler setNextHandler (MessageHandler * nextHandler);
private:
//=====[Declaration of privates atributes]=========================================

//=====[Declaration of privates methods]=========================================
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif //   _CHECKSUM_GENERATOR_H_