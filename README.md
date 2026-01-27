![](./resources/fiubaLogo.png)

# Project "Rastreador GNSS para Tareas Investigativas" (GNSS Tracker for investigative Tasks)
# - LoRa Gateway -

# Brief

The Firmware contained in this repository form part of the project GNSS Tracker for investigation Tasks which
is the final work of the degree Electronic Engenieering from the
[Faculty of Engenieering of the University of Buenos Aires](https://www.fi.uba.ar/).

This code consist in the code upload into the a LoRa Gateway, while the others two key components of the project are
a [Tracker](https://github.com/FacundoBaez1994/RastreadorGNSSParaTareasInvestigativas) and the [Remote Control Server](https://github.com/joaquinelordi/Torcaza)
named Torcaza after an Argentinean dove.

The GNSS tracker system for investigative tasks, consists of an autonomous device built with the objective of getting the localization 
of himself in real time, and sending it to a remote base, in order to keep the track of a vehicle terrestrial or naval.
The device was initially conceived for his use in the Argentinian Coast Guard, but is extrapolable to almost any LEA, 
(Law Enforcement Agent) and it was devised after a real problem. The Tracker uses and LTE mobile network module as the main transceiver
but it also has incorporated an LoRa transciever module useful in areas without mobile network. Therefore, a device that work as 
a LoRa Gateway was added to the proyect.

The LoRa Gateway receives the transmission message from the tracker and forwards it to the remote server either through its
 Ethernet module—if connected to a nearby router with internet access—or via its own LTE mobile network module, 
 assuming that mobile network service is available at the Gateway's location but not in the Tracker's area.

Throughout the development, Students have applied knowledge multiple areas of the Electronic Engineering curriculum such as Embedded Systems,
Algorithms, programming, PCB design, general electronics, informatic security, data communication, among others.

# Hardware

The hardware of the tracker consists in various OEM Modules listed:

1. Nucleo-L432KC Board from STMicroelectronics
2. EC21-Aux from Quectel. GNSS-LTE Module.
3. RFM95 LoRa - which has incorporated a Semtech SX1276.
4. 2 LM2596 StepDown voltage regulator
5. enc28j60 Ethernet module

The system is powered by a 12V 5A Gel Battery, therefore the device works with a voltage ranging in 10V to 14,4V.
The OEM modules works 3.3V and 4.0V, therefore the battery voltage is reduced using the stepDown regulators.

# Firmware


The firmware was developed in the C++ programming languages and using the NUCLEO family develop board from ARM, making use of [mbed-os]
(https://github.com/ARMmbed/mbed-os) which provides many useful tools and libraries for embedded development. 

C++ provides the possibility of using the paradigm of Object-Oriented Programing (OOP) while still enabling low-level capabilities like bit by bit
register manipulation which is essential for embedded system develop. Sticking with the OOP paradigm and respecting all his fundamental pillars,
allow to generate easy to read/understand, easy to maintain and reusable for future projects code. Desing Pattern has been used such has Chain of Responsibility,
Singleton, State Pattern and Facade.

The main ruting of the firmware it can be described has a big non blocking Polling routine based on nested State Patterns following a Class 
Hierarchy based on abstration levels of programming or how close to the OEM Modules the Classes operates beeing Tracker the Class that sits on top
of this Hierarchy, because is the only call by main (). Despite been a polling routine it also some interruption to handle differents timers.
This Polling routine makes the Nucleo board ask to one at time modules for the information needed and actions to do in order to perform 
the tracker rutine, for example through sending AT command at the LTE/GNNS Module make it connect to the mobile newtwoks. 

The archives are organized mostly by the standard were the ones with .h extention are class declaration (with there attributes and methods)
 and the archives with .cpp contains the implementation of the methods.

The firmware is self documented using doxygen type of commentaries in almost class, function, method, enumerative type, e.g.

# Members

## authors

1. Baez Facundo
2. Elordi Joaquin

## tutor

1. Eng. Graña Jorge

# Credits (Libraries used)

Outside the ones written by mbed Microcontroller Library this Firmware make uses of many libraries of
various authors customized to fit into the project and work properly with mbed-os. 
The following list shows all the libraries used:

- [LoRa](https://github.com/sandeepmistry/arduino-LoRa) by Sandeep Mistry
- [CustomJWT](https://github.com/Ant2000/CustomJWT) by Antony Jose Kuruvilla,
- [Cypher AES](https://os.mbed.com/users/neilt6/code/AES/) by Neil Thiessen,
- NonBlocking Delay and Miscellaneous codes from Arm Book  [A Beginner's-Guide-to-Designing-Embedded-System-Applications-on-Arm-Cortex-M-Microcontrollers](https://github.com/arm-university/A-Beginners-Guide-to-Designing-Embedded-System-Applications-on-Arm-Cortex-M-Microcontrollers) by Dr. Eng. Ariel Lutenberg
- [UIPEthernet](https://github.com/UIPEthernet/UIPEthernet) by Juraj Andrássy,
### License and contributions

The software is provided under Apache-2.0 license. Contributions to this project are accepted under the same license. Please see [CONTRIBUTING.md](./CONTRIBUTING.md) for more info.

This project contains code from other projects. The original license text is included in those source files. They must comply with our license guide.
