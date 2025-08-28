//=====[#include guards - begin]===============================================

#ifndef _AT_COMMAND_HANDLER_H_
#define _AT_COMMAND_HANDLER_H_

//==================[Libraries]===============================================

#include "mbed.h"
#include <string>

//=====[Declaration of public data types]======================================

//=====[Declaration of defines]========================================
#define BUFFER_LEN_AT_COMMAND_HANDLER 1024


//=====[Declaration of public classes]=========================================
/**
 * @class ATCommandHandler
 * @brief Handles communication with an AT-command-based module via UART.
 * This class provides methods to send AT commands and read responses
 * over a BufferedSerial interface. It manages buffering of incoming data
 * and allows line-by-line or character-by-character reading.
 */
class ATCommandHandler {
public:

//=====[Declaration of public methods]=========================================
    /**
    * @brief Constructor for the ATCommandHandler class.
    * Initializes the internal buffer index and assigns the provided UART interface
    * for serial communication with the module.
    * @param UART Pointer to a BufferedSerial object used for UART communication.
    * UART Must be initialized
    */
    ATCommandHandler (BufferedSerial * UART);

    /**
    * @brief Destructor for the ATCommandHandler class.
    * Releases the memory allocated for the UART communication interface.
    */
    virtual ~ATCommandHandler();

    /**
    * @brief Sends an AT command over the UART interface.
    * @param ATCommandToBeSend Pointer to the null-terminated AT command string to be sent.
    * @returns void
    */
    void sendATCommand (char * ATCommandToBeSend);

    /**
    * @brief Sends an AT command over the UART interface.
    * @param ATCommandToBeSend Pointer to the null-terminated AT command string to be sent.
    * @param length The number of characters in the command to be written 
    * @returns void
    */
    void sendATCommand (const char * ATCommandToBeSend, size_t length);

    /**
    * @brief Reads a single line of the AT command response from the UART interface.
    * This method reads characters one by one from the UART. When a line terminator 
    * (`\r` or `\n`) is detected and the buffer contains data, the string is 
    * null-terminated, copied into the user-provided buffer, and returned as a complete line.
    * @param StringToBeRead Pointer to the character array where the read response line will be stored.
    * Must be large enough to hold the response.
    * @returns bolean, true if a complete response line was read and copied, false otherwise.
    */
    bool readATResponse (char * StringToBeRead);

    /**
    * @brief Returns a pointer to the internal UART interface.
    * Allows external classes or modules to access the BufferedSerial
    * object used by this handler.
    * @returns Pointer to the BufferedSerial object used for communication.
    */
    BufferedSerial* getUART (void);

    /**
    * @brief Reads a single character from the UART interface.
    * If data is available on the UART, reads one character and stores it
    * in the provided variable. This function is non-blocking.
    * @param charRead Pointer to a char variable where the received character will be stored.
    * @returns bolean, true if a character was successfully read, false if no data was available.
    */
    bool readChar (char * charRead);

private:
//=====[Declaration of privates atributes]=========================================
    BufferedSerial* serialComunicationUART; //!< UART interface for communication.
    int bufferIndex;                        //!< Index for the internal response buffer.
    char buffer[BUFFER_LEN_AT_COMMAND_HANDLER];  //!< Buffer to store incoming data until a line is complete.
    
//=====[Declaration of privates methods]=========================================

};


//=====[Declarations (prototypes) of public functions]=========================

//=====[#include guards - end]=================================================

#endif // _AT_COMMAND_HANDLER_H_