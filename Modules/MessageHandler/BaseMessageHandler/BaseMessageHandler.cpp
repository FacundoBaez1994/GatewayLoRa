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

//=====[Implementations of public methods]===================================
BaseMessageHandler::BaseMessageHandler () {
}


BaseMessageHandler::~BaseMessageHandler () {

}

MessageHandler * BaseMessageHandler::setNextHandler(MessageHandler * nextHandler) {
  this->nextHandler = nextHandler;
  return nextHandler;
}

MessageHandlerStatus_t BaseMessageHandler::handleMessage (char * message, unsigned int sizeOfMessage) {
    return MESSAGE_HANDLER_STATUS_PROCESSING_MESSAGE;
}









