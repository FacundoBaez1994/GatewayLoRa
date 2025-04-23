//=====[Libraries]=============================================================

#include "BaseMessageHandler.h"
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============




//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================
/** 
* @brief attachs the callback function to the ticker
*/


//=====[Implementations of public methods]===================================
/** 
* @brief
* 
* @param 
*/
BaseMessageHandler::BaseMessageHandler () {
}


/** 
* @brief 
* 
* 
* @returns 
*/
BaseMessageHandler::~BaseMessageHandler () {

}


/** 
* @brief 
* 
* 
* @returns 
*/
MessageHandler * BaseMessageHandler::setNextHandler(MessageHandler * nextHandler) {
  this->nextHandler = nextHandler;
  return nextHandler;
}

MessageHandlerStatus_t BaseMessageHandler::handleMessage (char * message) {
    return MESSAGE_HANDLER_STATUS_PROCESSING_MESSAGE;
}









