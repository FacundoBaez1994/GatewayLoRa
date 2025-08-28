//=====[Libraries]=============================================================

#include "Non_Blocking_Delay.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


static tick_t tickCounter = 0;


//=====[Declarations (prototypes) of private functions]========================

void tickerCallback();
tick_t tickRead();

//=====[Implementations of private methods]===================================

void NonBlockingDelay::tickInit() {
    this->ticker.attach( tickerCallback, 0.01);
}

//=====[Implementations of public methods]===================================
NonBlockingDelay::NonBlockingDelay (tick_t durationValue ) {
    this->tickInit ();
    this->duration = durationValue;
    this->isRunning = false;
}

NonBlockingDelay::~NonBlockingDelay() {
    ticker.detach();  
}


bool NonBlockingDelay::read (void) {
   bool timeArrived = false;
   tick_t elapsedTime;

   if( !this->isRunning ) {
        this->startTime = tickCounter;
        this->isRunning = true;
   } else {
      elapsedTime = tickCounter - this->startTime;
      if ( elapsedTime >= this->duration ) {
         timeArrived = true;
         this->isRunning = false;
      }
   }
   return timeArrived;
}


tick_t NonBlockingDelay::getStartTime() {
   return this->startTime;
}


bool NonBlockingDelay::getRunningStatus(){
    return this->isRunning;
}


void NonBlockingDelay::write( tick_t durationValue ) {
   this->duration = durationValue;
}


void NonBlockingDelay::restart(  ) {
    this->startTime = tickCounter;
    this->isRunning = true;
}



tick_t NonBlockingDelay::getCurrentTick() {
    return tickCounter;
}

//=====[Implementations of private functions]==================================
/** 
* @brief callback function adds 1 to a counter
* @note ISR cannot form part of the class
*/
void tickerCallback( void )  {
    tickCounter++;
}

/** 
* @brief returns counter
* @return tickCounter a counter value
*/
tick_t tickRead() {
    return tickCounter;
}


