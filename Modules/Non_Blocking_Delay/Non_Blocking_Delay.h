//=====[#include guards - begin]===============================================

#ifndef _NON_BLOCKING_DELAY_H_
#define _NON_BLOCKING_DELAY_H_

//==================[Libraries]===============================================

#include "mbed.h"

//=====[Declaration of public data types]======================================
/**
 * @typedef tick_t
 * @brief Represents a tick-based time unit for delays.
 */
typedef uint64_t tick_t;

//=====[Declaration of public classes]=========================================
/**
 * @class NonBlockingDelay
 * @brief Implements a delay mechanism that does not block the main loop.
 * 
 * @details This class uses a hardware ticker to increment a global tick counter.
 * It allows measuring elapsed time without blocking the processor, which is 
 * particularly useful in embedded systems where cooperative multitasking is preferred.
 */
class NonBlockingDelay {
public:

//=====[Declaration of public methods]=========================================
    /**
     * @brief Constructor that initializes the delay with a given duration.
     * @param durationValue Duration of the delay in ticks.
     */
    NonBlockingDelay ( tick_t durationValue);

    /**
     * @brief Destructor frees memory, deatch ticker.
     */
    ~NonBlockingDelay();

    /**
     * @brief Checks whether the delay duration has elapsed.
     * @details If the delay was not running, it starts it. If the time has elapsed,
     * the delay stops and returns true.
     * @return true if delay time has elapsed, false otherwise.
     */
    bool read( void );

    /**
     * @brief Sets a new duration for the delay.
     * @param durationValue New duration in ticks.
     */
    void write(  tick_t durationValue );

    /**
     * @brief Restarts the delay from the current tick.
     */
    void restart ();

    /**
     * @brief Returns the tick count at which the delay started.
     * @return Start time in ticks.
     */
    tick_t getStartTime();

    /**
     * @brief Indicates whether the delay is currently active.
     * @return true if running, false otherwise.
     */
    bool getRunningStatus ();
    
    /**
     * @brief Returns the current global tick count.
     * @return Current tick count.
     */
    static tick_t getCurrentTick(); 
private:
//=====[Declaration of privates atributes]=========================================
    tick_t startTime;   ///< Tick count when the delay started
    tick_t duration;    ///< Duration of the delay in ticks
    bool isRunning;     ///< Whether the delay is currently active
    Ticker ticker;      ///< Ticker used to update the global tick counter

//=====[Declaration of privates methods]=========================================
    /**
     * @brief Initializes the internal ticker.
     * @details Attaches a callback to increment the global tick counter every 10 ms.
     */
    void tickInit();
};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _NON_BLOCKING_DELAY_H