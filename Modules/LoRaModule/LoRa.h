#ifndef LORA_H
#define LORA_H

#include "mbed.h"

#define PA_OUTPUT_RFO_PIN      0
#define PA_OUTPUT_PA_BOOST_PIN 1

class LoRaClass: public Stream
{
public:
    LoRaClass();

    int begin(long frequency);
    void end();


    ssize_t read(uint8_t* buffer, size_t length);

    void enableInvertIQ();
    void disableInvertIQ();

    int beginPacket(int implicitHeader = false);
    int endPacket();

    int parsePacket(int size = 0);
    int packetRssi();
    float packetSnr();

    void onReceive(void(*callback)(int));

    void receive(int size = 0);
    void idle();
    void sleep();

    void setTxPower(int level, int outputPin = PA_OUTPUT_PA_BOOST_PIN);
    void setFrequency(long frequency);
    void setSpreadingFactor(int sf);
    void setSignalBandwidth(long sbw);
    void setCodingRate4(int denominator);
    void setPreambleLength(long length);
    void setSyncWord(int sw);
    void enableCrc();
    void disableCrc();

    // deprecated
    void crc() {
        enableCrc();
    }
    void noCrc() {
        disableCrc();
    }

    uint8_t random();

    void dumpRegisters(Stream& out);

    int available();

protected:
    // from Print
    virtual int _putc(int value);
    virtual int _getc();


    int peek();

private:
    void explicitHeaderMode();
    void implicitHeaderMode();

    void handleDio0Rise();

    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    uint8_t singleTransfer(uint8_t address, uint8_t value);

    static void onDio0Rise();

private:
    SPI &spi;
    DigitalOut _ss;
    DigitalOut _reset;
    //InterruptIn _dio0;
    int _frequency;
    int _packetIndex;
    int _implicitHeaderMode;
    void (*_onReceive)(int);
};

extern LoRaClass LoRa;

#endif
