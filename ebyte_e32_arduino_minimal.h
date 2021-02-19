#pragma once
#include <Arduino.h>

class E32LORA {
public:
    enum class Modes{Normal, Wake_Up, Power_Saving,Sleep};
    enum class UART_RATES{U1200,U2400,U4800,U9600,U19200,U38400,U57600,U115200};
    enum class AIR_RATES{A300,A1200,A2400,A4800,A9600,A19200};
    enum class UART_PARITY_BITS{U8N1,U8O1,U8E1};
    enum class TRANSMISSION_MODE{FIXED,TRANSPARENT};
    enum class TRANSMISSION_POWER{dbm30,dbm27,dbm24,dbm21};

    E32LORA(HardwareSerial& serial_connection,int i_aux_pin,int i_m0_pin,int i_m1_pin):m_serial(serial_connection),m0(i_m0_pin),m1(i_m1_pin),aux_pin(i_aux_pin){
        pinMode(i_aux_pin,INPUT);
        pinMode(i_m0_pin,OUTPUT);
        pinMode(i_m1_pin,OUTPUT);
        change_mode(Modes::Normal);
        m_serial.flush();
    }

    String read_operating_parameters();

    String read_version_number();

    void send(char * buf, uint16_t length);

    void resetModule();

    void set_new_paramters(bool save_when_power_down, uint8_t adress_low,uint8_t adress_high,uint8_t channel,UART_PARITY_BITS,UART_RATES,AIR_RATES,TRANSMISSION_MODE,TRANSMISSION_POWER,bool debug);

    void sendCommand(uint8_t CMD[],uint8_t size);

private:



    void change_mode(Modes new_mode);
    Modes m_current_mode;

    HardwareSerial& m_serial;
    int m0,m1,aux_pin;

    void setbit(uint8_t & number,int whichbit){
        number |= 1UL << whichbit;
    }

};




