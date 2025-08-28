//=====[Libraries]=============================================================

#include "SleepState.h"
#include "PowerManager.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define MAX_TRIES 10

#define LOG_MESSAGE_CURRENT_STATE "\r\nSleepState\r\n"
#define LOG_MESSAGE_CURRENT_STATE_LEN (sizeof(LOG_MESSAGE_CURRENT_STATE) - 1)

#define LOG_MESSAGE_AWAKE "\r\nAwakening\r\n"
#define LOG_MESSAGE_AWAKE_LEN (sizeof(LOG_MESSAGE_CURRENT_STATE) - 1)

#define AT_CMD_POWER_DOWN "AT+QPOWD"
#define AT_CMD_POWER_DOWN_LEN (sizeof(AT_CMD_POWER_DOWN) - 1)

#define AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE "POWERED DOWN"
#define AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE_LEN (sizeof(AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE) - 1)

#define LOG_MESSAGE_DEVICE_TURNING_DOWN "TURNING DOWN\r\n"
#define LOG_MESSAGE_DEVICE_TURNING_DOWN_LEN (sizeof(LOG_MESSAGE_DEVICE_TURNING_DOWN ) - 1)

#define BUFFER 128
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
SleepState::SleepState () {
    uartUSB.write (LOG_MESSAGE_CURRENT_STATE, strlen (LOG_MESSAGE_CURRENT_STATE));

    this->manager = nullptr;
    this->status = SLEEP;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}

SleepState::SleepState (PowerManager* newManager) {
    uartUSB.write (LOG_MESSAGE_CURRENT_STATE, strlen (LOG_MESSAGE_CURRENT_STATE));
    
    this->manager = newManager;
    this->status = SLEEP;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}

SleepState::~SleepState () {
    this->manager = nullptr;
}

powerStatus_t SleepState::startStopUpdate (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer) {
    return SLEEP;
}

bool SleepState::reboot (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer) {
    return false;
}

bool SleepState::goToSleep (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer) {
  return true;
}

void SleepState::awake (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer) {
    uartUSB.write ( LOG_MESSAGE_AWAKE , strlen ( LOG_MESSAGE_AWAKE ));  // debug only
    this->manager->changeDTRSignal(OFF);
    this->manager->changePowerState (new PowerONState ( this->manager) );
}

bool SleepState::measureBattery (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
   return false;
}

bool SleepState::turnOn (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer) {
    this->awake(AThandler, powerChangeDurationTimer);
    return true;
}

bool SleepState::turnOff (ATCommandHandler* AThandler, NonBlockingDelay* powerChangeDurationTimer) {
    static bool readyToSend = true;
    static bool hardPowerOffUnderProcess = false;
    static int retryCounter = 0;
    char StringToSend [AT_CMD_POWER_DOWN_LEN + 1] = AT_CMD_POWER_DOWN;
    char StringToBeRead [BUFFER];
    char ExpectedResponse [ AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE_LEN + 1] =  AT_CMD_POWER_DOWNN_EXPECTED_RESPONSE;
    char StringToSendUSB [LOG_MESSAGE_DEVICE_TURNING_DOWN_LEN + 1] = LOG_MESSAGE_DEVICE_TURNING_DOWN;

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
    }


    if ( AThandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only          
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