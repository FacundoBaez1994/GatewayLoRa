//=====[Libraries]=============================================================
#include "PowerONState.h"
#include "PowerManager.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAX_TRIES 10
#define MAX_COUNT_TURN_OFF 20

#define LOG_MESSAGE_CURRENT_STATE "\r\nPowerONState\r\n"
#define LOG_MESSAGE_CURRENT_STATE_LEN (sizeof(LOG_MESSAGE_CURRENT_STATE) - 1)

#define LOG_MESSAGE_POWER_OFF_DETECTED "POWER OFF DETECTED\r\n"
#define LOG_MESSAGE_POWER_OFF_DETECTED_LEN (sizeof(LOG_MESSAGE_POWER_OFF_DETECTED) - 1)

#define LOG_MESSAGE_AUTOMATIC_POWER_OFF "AUTOMATIC TURNING OFF\r\n"
#define LOG_MESSAGE_AUTOMATIC_POWER_OFF_LEN (sizeof(LOG_MESSAGE_AUTOMATIC_POWER_OFF) - 1)

#define LOG_MESSAGE_MANUAL_POWER_OFF "MANUAL TURNING OFF\r\n"
#define LOG_MESSAGE_MANUAL_POWER_OFF_LEN (sizeof(LOG_MESSAGE_MANUAL_POWER_OFF) - 1)

#define AT_CMD_POWER_DOWN "AT+QPOWD"
#define AT_CMD_POWER_DOWN_LEN (sizeof(AT_CMD_POWER_DOWN) - 1)

#define AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE "POWERED DOWN"
#define AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE_LEN (sizeof(AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE_DEVICE_TURNING_DOWN "TURNING DOWN\r\n"
#define LOG_MESSAGE_DEVICE_TURNING_DOWN_LEN (sizeof(LOG_MESSAGE_DEVICE_TURNING_DOWN ) - 1)

#define LOG_MESSAGE_DEVICE_REBOOTING "Rebooting\r\n"
#define LOG_MESSAGE_DEVICE_REBOOTING_LEN (sizeof(LOG_MESSAGE_DEVICE_REBOOTING ) - 1)

#define BUFFER 128

#define AT_CMD_SLEEP "AT+QSCLK=1"
#define AT_CMD_SLEEP_LEN (sizeof(AT_CMD_SLEEP) - 1)

#define AT_CMD_SLEEP_EXPECTED_RESPONSE "OK"
#define AT_CMD_SLEEP_EXPECTED_RESPONSE_LEN (sizeof( AT_CMD_SLEEP_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE_SLEEP "Go To Sleep\r\n"
#define LOG_MESSAGE_SLEEP_LEN (sizeof(LOG_MESSAGE_SLEEP ) - 1)

#define AT_CMD_BATTERY_MEASURE "AT+CBC"
#define AT_CMD_BATTERY_MEASURE_LEN (sizeof(AT_CMD_BATTERY_MEASURE) - 1)

#define LOG_MESSAGE_BATTERY_MEASURE "Measuring Battery\r\n"
#define LOG_MESSAGE_BATTERY_MEASURE_LEN (sizeof(LOG_MESSAGE_BATTERY_MEASURE) - 1)



//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
PowerONState::PowerONState () {
    uartUSB.write (LOG_MESSAGE_CURRENT_STATE, strlen (LOG_MESSAGE_CURRENT_STATE));

    this->manager = nullptr;
    this->status = POWER_ON;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
    this->turnOfWasCall = false;
}

PowerONState::PowerONState (PowerManager * newManager) {
    uartUSB.write (LOG_MESSAGE_CURRENT_STATE, strlen (LOG_MESSAGE_CURRENT_STATE));
    
    this->manager = newManager;
    this->status = POWER_ON;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
     this->turnOfWasCall  = false;
}

PowerONState::~PowerONState () {
    this->manager = nullptr;
}

powerStatus_t PowerONState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    int static turnOffCounter = 0;
    // PowerStatus ON equals 

    if ( this->turnOfWasCall == true) {
        return this->status;
    }

    if (this->manager->readPowerStatus()  == OFF) {
        turnOffCounter = 0;
    }
    if (this->manager->readPowerStatus()  == ON) {
         turnOffCounter ++;
    }
    if (turnOffCounter == MAX_COUNT_TURN_OFF) {
            uartUSB.write (LOG_MESSAGE_POWER_OFF_DETECTED, strlen (LOG_MESSAGE_POWER_OFF_DETECTED));  // debug only
        if (this->ManualTurningPower == false) {
            ////////////  //////////// ////////////
            uartUSB.write (LOG_MESSAGE_AUTOMATIC_POWER_OFF, strlen (LOG_MESSAGE_AUTOMATIC_POWER_OFF));  // debug only
            //////////// //////////// ////////////
            this->manager->changePowerState (new PowerOFFState ( this->manager) );
            this->status = POWER_OFF;
            return this->status;
        } else {
            uartUSB.write (LOG_MESSAGE_MANUAL_POWER_OFF, strlen (LOG_MESSAGE_MANUAL_POWER_OFF));  // debug only
            this->manager->changePowerState (new ManualPowerOFFState ( this->manager) );
            this->status = MANUAL_POWER_OFF;
            return this->status;

        }
    }

    // reverse logic
    // Reading button
    if (this->manager->readInputControlButton() == OFF) {
        this->ManualTurningPower = true;
         this->TurningDown = true;
    }

    // Start Stop Signal
    if (ManualTurningPower == true && SignalTurningPowerUp == false && this->TurningDown == true ) {
        this->manager->changeKeyDigitalSignal (true);
        this->ManualTurningPower = true;
        this->SignalTurningPowerUp = true;
        powerChangeDurationTimer->restart();
    }
    if (this->SignalTurningPowerUp  == true && powerChangeDurationTimer->read() && this->TurningDown == true) {
        this->manager->changeKeyDigitalSignal (false);
        this->SignalTurningPowerUp = false;
        powerChangeDurationTimer->restart();
        this->TurningDown = false;
    }
    return this->status;
}


bool PowerONState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool readyToSend = true;
    char StringToSend [AT_CMD_POWER_DOWN_LEN + 1] = AT_CMD_POWER_DOWN;
    char StringToBeRead [BUFFER];
    char ExpectedResponse [ AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE_LEN + 1] =  AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_DEVICE_REBOOTING_LEN + 1] = LOG_MESSAGE_DEVICE_REBOOTING;

    if (readyToSend == true) {
        AThandler->sendATCommand (StringToSend);
        readyToSend = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only 
    }


    if ( AThandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            uartUSB.write (LOG_MESSAGE_DEVICE_TURNING_DOWN, strlen (LOG_MESSAGE_DEVICE_TURNING_DOWN ));  // debug only           
            this->manager->changePowerState (new PowerOFFState (this->manager));
            return true;
        }
    }

    if (powerChangeDurationTimer->read()) {
        readyToSend = true;
    }
    return false;
}



bool PowerONState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
   static bool readyToSend = true;
    char StringToSend [ AT_CMD_SLEEP_LEN + 1] =  AT_CMD_SLEEP;
    char StringToBeRead [ BUFFER];
    char ExpectedResponse [AT_CMD_SLEEP_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_SLEEP_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_SLEEP_LEN + 1] = LOG_MESSAGE_SLEEP;

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }


    if ( AThandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->manager->changeDTRSignal(ON);         
            this->manager->changePowerState (new SleepState (this->manager));
            return true; 
        }
    }

    if (powerChangeDurationTimer->read()) {
        readyToSend = true;
    }
    return false;
}

void PowerONState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

bool PowerONState::turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return true;
}

bool PowerONState::turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool readyToSend = true;
    static bool hardPowerOffUnderProcess = false;
    static int retryCounter = 0;
    char StringToSend [AT_CMD_POWER_DOWN_LEN + 1] = AT_CMD_POWER_DOWN;
    char StringToBeRead [BUFFER];
    char ExpectedResponse [AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE_LEN + 1] = AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_DEVICE_TURNING_DOWN_LEN + 1] = LOG_MESSAGE_DEVICE_TURNING_DOWN;

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        this->turnOfWasCall  = true;
        readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( AThandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {         
            this->manager->changePowerState (new ManualPowerOFFState (this->manager));
            return true;
        }
    }

    if (retryCounter <= MAX_TRIES) {
        if (powerChangeDurationTimer->read()  ) {
            retryCounter++;
            readyToSend = true;
        }
    } else { //// if there is no response form the module use the hard power option
        if (powerChangeDurationTimer->read()  ) {
            if (hardPowerOffUnderProcess  == false) {
                this->manager->changePowerDownSignal (OFF);
                hardPowerOffUnderProcess = true;
            } else {
                this->manager->changePowerDownSignal (ON);
                readyToSend = true;
                hardPowerOffUnderProcess = false;
                retryCounter = 0;
                this->manager->changePowerState (new ManualPowerOFFState (this->manager));
                return true;

            }

        }
    }
    
    return false;
}


bool PowerONState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
    static bool readyToSend = true;
    char StringToSend [AT_CMD_BATTERY_MEASURE_LEN + 1] = AT_CMD_BATTERY_MEASURE;
    char StringToBeRead [BUFFER];
    char StringToSendUSB [LOG_MESSAGE_BATTERY_MEASURE_LEN + 1] = LOG_MESSAGE_BATTERY_MEASURE;

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }

    if ( AThandler->readATResponse ( StringToBeRead) == true) {
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        if (this->retrivBatteryData (StringToBeRead, currentBatteryData) == true) {
            return true;
        }
    }

    if (powerChangeDurationTimer->read()) {
        readyToSend = true;
    }
    return false;
}

//=====[Implementations of private functions]==================================
bool PowerONState::retrivBatteryData(char* stringToAnalyse, BatteryData* currentBatteryData) {
    const char* prefix = "+CBC:";
    if (strncmp(stringToAnalyse, prefix, strlen(prefix)) != 0) {
        return false;
    }

    char* dataStart = stringToAnalyse + strlen(prefix);

    int bcs, bcl;
    float voltage;
    if (sscanf(dataStart, "%d,%d,%f", &bcs, &bcl, &voltage) == 3) {
        currentBatteryData->batteryChargeStatus = bcs;
        currentBatteryData->chargeLevel = bcl;
        currentBatteryData->voltage = voltage;

            uartUSB.write("Battery Charge Status: ", strlen("Battery Charge Status: "));
            char statusStr[5];
            sprintf(statusStr, "%d",  currentBatteryData->batteryChargeStatus);
            uartUSB.write(statusStr, strlen(statusStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Charge: ", strlen("Charge: "));
            char chargeStr[5];
            sprintf(chargeStr, "%d", currentBatteryData->chargeLevel);
            uartUSB.write(chargeStr, strlen(chargeStr));
            uartUSB.write("\r\n", 2);

            uartUSB.write("Voltage: ", strlen("Voltage: "));
            char voltageStr[5];
            sprintf(voltageStr, "%.2f",  currentBatteryData->voltage );
            uartUSB.write(voltageStr, strlen(voltageStr));
            uartUSB.write("\r\n", 2);

        return true; 
    }

    return false;
}