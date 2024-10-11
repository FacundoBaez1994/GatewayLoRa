//=====[Libraries]=============================================================

#include "PowerONState.h"
#include "PowerManager.h" //debido a declaracion adelantada
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
PowerONState::PowerONState () {
    this->manager = NULL;
    this->status = POWER_ON;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief
* 
* @param 
*/
PowerONState::PowerONState (PowerManager * newManager) {
    this->manager = newManager;
    this->status = POWER_ON;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningDown = false;
}


/** 
* @brief 
* 
* 
* @returns 
*/
PowerONState::~PowerONState () {
    this->manager = NULL;
}


/** 
* @brief 
* 
* 
* @returns 
*/
powerStatus_t PowerONState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {

    // PowerStatus ON equals 
    if (this->manager->readPowerStatus()  == ON) {
        ////////////  //////////// ////////////
            char StringToSend [30] = "POWER OFF DETECTED";;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            //////////// //////////// ////////////
        if (this->ManualTurningPower == false) {
            ////////////  //////////// ////////////
            char StringToSend [30] = "AUTOMATIC TURNING OFF";;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            //////////// //////////// ////////////
            this->manager->changePowerState (new PowerOFFState ( this->manager) );
            this->status = POWER_OFF;
            return this->status;
        } else {
            ////////////  //////////// ////////////
            char StringToSend [30] = "MANUAL TURNING OFF";;
            uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            //////////// //////////// ////////////
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

        //////////////////////////////////////////
        char StringToSend [30] = "UP";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (true);
        this->ManualTurningPower = true;
        this->SignalTurningPowerUp = true;
        powerChangeDurationTimer->restart();
    }
    if (this->SignalTurningPowerUp  == true && powerChangeDurationTimer->read() && this->TurningDown == true) {

        //////////////////////////////////////////
        char StringToSend [30] = "DOWN";
        uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (false);
        this->SignalTurningPowerUp = false;
        powerChangeDurationTimer->restart();
        this->TurningDown = false;
    }
    return this->status;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerONState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool readyToSend = true;
    char StringToSend [15] = "AT+QPOWD";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "POWERED DOWN";
    char StringToSendUSB [40] = "Rebooting";

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }


    if ( AThandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "TURNING DOWN";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            ////   ////   ////   ////   ////   ////            
            this->manager->changePowerState (new PowerOFFState (this->manager));
        }
    }

    if (powerChangeDurationTimer->read()) {
        readyToSend = true;
    }
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool PowerONState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
   static bool readyToSend = true;
    char StringToSend [15] = "AT+QSCLK=1";
    char StringToBeRead [256];
    char ExpectedResponse [15] = "OK";
    char StringToSendUSB [40] = "Go To Sleep";

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }


    if ( AThandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

        if (strcmp (StringToBeRead, ExpectedResponse) == 0) {
            ////   ////   ////   ////   ////   ////
            char StringToSendUSB [40] = "Entering Sleep Mode";
            uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
            uartUSB.write ( "\r\n",  3 );  // debug only
            this->manager->changeDTRSignal(ON);
            ////   ////   ////   ////   ////   ////            
            this->manager->changePowerState (new SleepState (this->manager));
            return true; 
        }
    }

    if (powerChangeDurationTimer->read()) {
        readyToSend = true;
    }
    return false;
}

/** 
* @brief 
* 
* 
* @returns 
*/
void PowerONState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

/** 
* @brief 
* 
* 
* @returns 
*/
bool PowerONState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
    static bool readyToSend = true;
    char StringToSend [15] = "AT+CBC";
    char StringToBeRead [256];
    char StringToSendUSB [40] = "Measuring Battery";

    if (readyToSend == true) {
        AThandler->sendATCommand(StringToSend);
        readyToSend = false;
        ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToSendUSB , strlen (StringToSendUSB ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        uartUSB.write (StringToSend  , strlen (StringToSend  ));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
        ////   ////   ////   ////   ////   ////   
    }

    if ( AThandler->readATResponse ( StringToBeRead) == true) {
         ////   ////   ////   ////   ////   ////
        uartUSB.write (StringToBeRead , strlen (StringToBeRead));  // debug only
        uartUSB.write ( "\r\n",  3 );  // debug only
         ////   ////   ////   ////   ////   ////

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
        return false;  // No comienza con "+CBC:"
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

    return false;  // No se pudieron extraer los datos correctamente
}