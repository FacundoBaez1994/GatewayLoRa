//=====[Libraries]=============================================================

#include "ManualPowerOFFState.h" 
#include "PowerManager.h" //debido a declaracion adelantada
#include "Debugger.h" // due to global usbUart

//=====[Declaration of private defines]========================================
#define TURN_ON_COUNTER 1000

#define LOG_MESSAGE_CURRENT_STATE "\r\nManualPowerOFFState\r\n"
#define LOG_MESSAGE_CURRENT_STATE_LEN (sizeof(LOG_MESSAGE_CURRENT_STATE) - 1)

#define LOG_MESSAGE_CHANGE_STATE "Turning ON\r\n"
#define LOG_MESSAGE_CHANGE_STATE_LEN (sizeof(LOG_MESSAGE_CHANGE_STATE) - 1)
//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================


//=====[Implementations of private methods]===================================

//=====[Implementations of public methods]===================================
ManualPowerOFFState::ManualPowerOFFState () {
    uartUSB.write (LOG_MESSAGE_CURRENT_STATE, strlen (LOG_MESSAGE_CURRENT_STATE));  // debug only

    this->manager = nullptr;
    this->status = MANUAL_POWER_OFF;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningUP = false;
    
    this->manager->changePowerDownSignal (OFF);
}

ManualPowerOFFState::ManualPowerOFFState (PowerManager * newManager) {
    uartUSB.write (LOG_MESSAGE_CURRENT_STATE, strlen (LOG_MESSAGE_CURRENT_STATE));  // debug only
    
    this->manager = newManager;
    this->status = MANUAL_POWER_OFF;
    this->ManualTurningPower = false;
    this->SignalTurningPowerUp = false;
    this->TurningUP = false;

    this->manager->changePowerDownSignal (OFF);
}

ManualPowerOFFState::~ManualPowerOFFState () {
    this->manager = nullptr;
}

powerStatus_t ManualPowerOFFState::startStopUpdate (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {

    // If powerStatus is in OFF status the power is ON (negate logic)
    if (this->manager->readPowerStatus()  == OFF && buttonPushed == true) {
        if (this->turnONCounter > TURN_ON_COUNTER)  {
            uartUSB.write (LOG_MESSAGE_CHANGE_STATE, strlen (LOG_MESSAGE_CHANGE_STATE));  // debug only
            this->turnONCounter = 0;
            this->status = POWER_ON;
            this->buttonPushed = false;
            this->manager->changePowerState (new PowerONState ( this->manager) );
            return this->status;
        }
        this->turnONCounter ++;
    }

 
    // reverse logic
    // Reading button
    if (this->manager->readInputControlButton() == OFF) {
        this->ManualTurningPower = true;
        this->TurningUP = true;
        this->buttonPushed = true;
    }

    // Start Stop Signal
    if (ManualTurningPower == true && SignalTurningPowerUp == false && this->TurningUP  == true ) {

        //////////////////////////////////////////
        //char StringToSend [30] = "UP";
        //uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        //uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (true);
        this->ManualTurningPower = true;
        this->SignalTurningPowerUp = true;
        powerChangeDurationTimer->restart();
    }
    if (this->SignalTurningPowerUp  == true && powerChangeDurationTimer->read() && this->TurningUP  == true) {

        //////////////////////////////////////////
        //char StringToSend [30] = "DOWN";
        //uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
        //uartUSB.write ( "\r\n",  3 );  // debug only
         //////////////////////////////////////////
        this->manager->changeKeyDigitalSignal (false);
        this->SignalTurningPowerUp = false;
        powerChangeDurationTimer->restart();
        this->TurningUP = false;
    }
    return this->status;
}

bool ManualPowerOFFState::reboot (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

bool ManualPowerOFFState::goToSleep (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return false;
}

void ManualPowerOFFState::awake (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    return;
}

bool ManualPowerOFFState::turnOn (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    static bool SignalTurningOn = false;
    this->manager->changePowerDownSignal (ON);
    // If powerStatus is in OFF status the power is ON (negate logic)
    if (this->manager->readPowerStatus()  == OFF) {
        uartUSB.write (LOG_MESSAGE_CHANGE_STATE, strlen (LOG_MESSAGE_CHANGE_STATE));  // debug only
        this->manager->changePowerState (new PowerONState ( this->manager) );
        this->status = POWER_ON;
        return true;
    }


    // Start Stop Signal
    if (SignalTurningOn == false  ) {
         if (powerChangeDurationTimer->read()) {
            //////////////////////////////////////////
            //char StringToSend [30] = "UP";
            //uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            //uartUSB.write ( "\r\n",  3 );  // debug only
            //////////////////////////////////////////
            this->manager->changeKeyDigitalSignal (true);
            powerChangeDurationTimer->restart();
        SignalTurningOn = true;
        }
    } else {
        if (powerChangeDurationTimer->read()) {
            //////////////////////////////////////////
            //char StringToSend [30] = "DOWN";
            //uartUSB.write (StringToSend, strlen (StringToSend));  // debug only
            //uartUSB.write ( "\r\n",  3 );  // debug only
            //////////////////////////////////////////
            this->manager->changeKeyDigitalSignal (false);
            SignalTurningOn = false;
            powerChangeDurationTimer->restart();
        }
    }

    return false;
}


bool ManualPowerOFFState::turnOff (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer) {
    this->manager->changePowerDownSignal (OFF);
    return true;
}

bool ManualPowerOFFState::measureBattery (ATCommandHandler  * AThandler, NonBlockingDelay * powerChangeDurationTimer
    ,  BatteryData * currentBatteryData) {
    return false;
}
//=====[Implementations of private functions]==================================
