#include "ebyte_e32_arduino_minimal.h"


void E32LORA::change_mode(E32LORA::Modes new_mode) {
    m_current_mode = new_mode;
    switch (new_mode) {
        case Modes::Normal:
            digitalWrite(m0, LOW);
            digitalWrite(m1, LOW);
            break;
        case Modes::Wake_Up:
            digitalWrite(m0, HIGH);
            digitalWrite(m1, LOW);
            break;
        case Modes::Power_Saving:
            digitalWrite(m0, LOW);
            digitalWrite(m1, HIGH);
            break;
        case Modes::Sleep:
            digitalWrite(m0, HIGH);
            digitalWrite(m1, HIGH);
            break;
    }
    delay(50);
}

String E32LORA::read_operating_parameters() {
    m_serial.flush();
    uint8_t res [3]{0xc1, 0xC1, 0xC1};
    sendCommand(res,3);
    String to_ret;
    while (m_serial.available()) {
        to_ret += String(m_serial.read(),HEX);
        to_ret += ",";
    }
    change_mode(Modes::Normal);
    return to_ret;
}

void E32LORA::sendCommand(uint8_t CMD[],uint8_t size) {
    change_mode(Modes::Sleep);
    m_serial.write(CMD, size);
    delay(50);
}

void E32LORA::resetModule() {
    uint8_t res [3]{0xc4, 0xC4, 0xC4};
    sendCommand(res,3);
    change_mode(Modes::Normal);
}

String E32LORA::read_version_number() {
    m_serial.flush();
    uint8_t res [3]{0xc3, 0xC3, 0xC3};
    sendCommand(res,3);
    String to_ret = "";
    while (m_serial.available()) {
        to_ret += String(m_serial.read(),HEX);
        to_ret += ",";
    }
    change_mode(Modes::Normal);
    return to_ret;
}

void E32LORA::set_new_paramters(bool save_when_power_down, uint8_t adress_low, uint8_t adress_high, uint8_t channel=0x06,
                                E32LORA::UART_PARITY_BITS parityBits = UART_PARITY_BITS::U8N1,
                                E32LORA::UART_RATES
                                uartRates = UART_RATES::U9600, E32LORA::AIR_RATES
                                airRates = AIR_RATES::A2400, E32LORA::TRANSMISSION_MODE
                                transmissionMode = TRANSMISSION_MODE::TRANSPARENT,
                                E32LORA::TRANSMISSION_POWER
                                transmissionPower = TRANSMISSION_POWER::dbm30,bool DEBUG=false) {

    uint8_t to_send[6];

    //set the HEAD parameter (Byte Nbr 1)
    if (save_when_power_down) {
        to_send[0] = 0xC0;
    } else {
        to_send[0] = 0xC2;
    }

    to_send[1] = adress_high;
    to_send[2] = adress_low;

    //the third byte will be split up into a bitset
    uint8_t SPED = 0;

    switch (parityBits) {
        case UART_PARITY_BITS::U8N1:
            break;
        case UART_PARITY_BITS::U8E1:
            setbit(SPED, 7);
            break;
        case UART_PARITY_BITS::U8O1:
            setbit(SPED, 6);
            break;
    }

    switch (uartRates) {
        case UART_RATES::U1200:
            break;
        case UART_RATES::U2400:
            setbit(SPED, 3);
            break;
        case UART_RATES::U4800:
            setbit(SPED, 4);
            break;
        case UART_RATES::U9600:
            setbit(SPED, 3);
            setbit(SPED, 4);
            break;
        case UART_RATES::U19200:
            setbit(SPED, 5);
            break;
        case UART_RATES::U38400:
            setbit(SPED, 3);
            setbit(SPED, 5);
            break;
        case UART_RATES::U57600:
            setbit(SPED, 5);
            setbit(SPED, 4);
            break;
        case UART_RATES::U115200:
            setbit(SPED, 5);
            setbit(SPED, 4);
            setbit(SPED, 3);
            break;
    }

    switch (airRates) {
        case AIR_RATES::A300:
            break;
        case AIR_RATES::A1200:
            setbit(SPED, 0);
            break;
        case AIR_RATES::A2400:
            setbit(SPED, 1);
            break;
        case AIR_RATES::A4800:
            setbit(SPED, 1);
            setbit(SPED, 0);
            break;
        case AIR_RATES::A9600:
            setbit(SPED, 2);
            break;
        case AIR_RATES::A19200:
            setbit(SPED, 0);
            setbit(SPED, 1);
            setbit(SPED, 2);
            break;
    }

    uint8_t CHAN = channel;

    uint8_t OPTION=0;
    if (transmissionMode == TRANSMISSION_MODE::FIXED)
    {
        setbit(OPTION,7);
    }
    setbit(OPTION,6);


    switch (transmissionPower) {
        case TRANSMISSION_POWER::dbm30:
            break;
        case TRANSMISSION_POWER::dbm27:
            setbit(OPTION,0);
            break;
        case TRANSMISSION_POWER::dbm24:
            setbit(OPTION,1);
            break;
        case TRANSMISSION_POWER::dbm21:
            setbit(OPTION,1);
            setbit(OPTION,0);
            break;
    }

    to_send[3] = SPED;
    to_send[4] = CHAN;
    to_send[5] = OPTION;

    if(DEBUG){
        Serial.print("New parameters:");
        for(auto x:to_send){
            Serial.print(x,HEX);
            Serial.print(",");
        }
        Serial.println("");
    }
    sendCommand(to_send,6);
}

void E32LORA::send(char *buf, uint16_t length) {
    m_serial.write(buf,length);
}

