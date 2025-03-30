#include "pico/stdlib.h"

#ifndef RP2040_DS3231_HPP
#define RP2040_DS3231_hpp


#define Addr_Seconds    0x00    // seconds address
#define Addr_Minutes    0x01    // Minutes address
#define Addr_Hours      0x02    // hours address
#define Addr_Day        0x03    // day address
#define Addr_Date       0x04    // date address
#define Addr_Month      0x05    // month address
#define Addr_Year       0x06    // Year address

#define Addr_Alarm_1_Seconds     0x07   // alarm 1 seconds address
#define Addr_Alarm_1_Minutes     0x08   // alarm 1 Minutes address
#define Addr_Alarm_1_Hours       0x09   // alarm 1 hours address
#define Addr_Alarm_1_Day         0x0A   // alarm 1 day address
#define Addr_Alarm_1_Date        0x0A   // alarm 1 date address

#define Addr_Alarm_2_Minutes     0x0B   // alarm 2 minutes address
#define Addr_Alarm_2_Hours       0x0C   // alarm 2 hour address
#define Addr_Alarm_2_Day         0x0D   // alarm 2 day address
#define Addr_Alarm_2_Date        0x0D   // alarm 2 date address

#define Addr_Control             0x0E   // control address
#define Addr_Control_Status      0x0F   // control_status address
#define Addr_Aging_Offset        0x10   // Aging_offset address
#define Addr_MSB_Temp            0x11   // MSB Temprature address
#define Addr_LSB_Temp            0x12   // LSB Temprature address

#define length_8bit_arr(x) (sizeof(x)/sizeof(uint8_t))

class DS_3231{

    public:

    struct Time;

    DS_3231 (uint8_t _ds3231_adddr);

    /**this is a general purpose moemory location for better memory managenent. 
     * this can be used by the programmer if required but better not to use
     * but can be freed afterwards for memory safty
     * @param general_purpose_memory_location uint8_t || 5 units ||
     */
    uint8_t *general_purpose_memory_location;   


    /**
     * This will set the RTC time in 12 Hour mode
     * @param *Time_setup location to time array
     */
    inline void set_time_12_hr(uint8_t *Time_setup);

    /**
     * This will set the RTC time in 24 Hour mode
     * @param *Time_setup location to time array
     */
    inline void set_time_24_hr(uint8_t *Time_setup);

    /**
     * it will fetch the time for you to the given address/pointer of location as parameter
     * @param *location the pointer or address to the location where the time to be copied. 
     *                  it should be a 7 byte long uint8_t array
     */
    inline void get_time(uint8_t *location);

    void read_time(uint8_t *location);

    /**
     * It will set alaram in RTC
     * @param Alarm 0 -> Alarm_1 || 1 -> Alarm_2
     * @param DY_DT 0 -> Date || 1 -> Day
     * @param hrs_fmt 0 -> 24 || 1 -> 12
     * @param data pointer or address to the data location   
     */
    inline void set_Alarm(bool Alarm, bool DY_DT, bool hrs_fmt, uint8_t *data);


    /**
     * it will write the data into the control status register
     * @param data the 8bit data to be written
     */
    inline void control_status_write(uint8_t data);

    /**
     * it will write the data into the control register
     * @param data the 8bit data to be written
     */
    inline void control_write(uint8_t data);

    /**
     * it will read the temprature from in-built temprature sensor and store in a variable
     */
    int8_t read_temp(void);

    /**
     * it will reat the aging offset register
     */
    int8_t read_Aging_Offset(void);

    /**
     * It wll setup the Alarm_1 Bits Mask, definitly recomends to read datasheet
     * @param Type_set 0 
     * @param Type_set 1
     * @param Type_set 2
     * @param Type_set 3
     */
    inline void Alarm_Bits_Mask_1(uint8_t Type_set);
    
    /**
     * It wll setup the Alarm_2 Bits Mask, definitly recomends to read datasheet
     * @param Type_set 0 
     * @param Type_set 1
     * @param Type_set 2
     */
    inline void Alarm_Bits_Mask_2(uint8_t Type_set);

    //tst
    void Alarm_Bits_Mask_1_Test(uint8_t val);

    /**
     * checks the status of 32khz output enable and soores in to a variable with bool dtype
     */
    bool status_EN32kHz(void);
    
    /**
     * set the 32khz output enable bit to high
     */
    void set_EN32kHz(void);

    /**
     * it wil give the time in binary form from bcd
     * @param *location the address of pointer to the location of aray
     * @param length the length of array 
     */
    inline void get_time_from_bcd(uint8_t *location, uint8_t length);
    
    /**
     * set the 32khz output enable bit to zero
     */
    void reset_EN32kHz(void);

    /**
     * checks the status of OSF bit and soores in to a variable with bool dtype
     */
    bool status_osf(void);

    /**
     * set the OSF bit to high
     */
    void set_osf(void);

    /**
     * set the OSF bit to zero
     */
    void reset_osf(void);

    /**
     * checks the status of EOSC bit and soores in to a variable with bool dtype
     * @param note it is inverted outputs ie, it is not inverted with respect to data sheet for som application
     */
    bool status_EOSC(void);

    /**
     * set the EOSC bit to high
     */
    void set_EOSC(void);
    
    /**
     * set the EOSC bit to zero
     */
    void reset_EOSC(void);

    /**
     * checks the status of OSF bit and soores in to a variable with bool dtype
     */
    bool status_BBSQW(void);////////////

    /**
     * set the BBSQW bit to high
     */
    void set_BBSQW(void);

    /**
     * set the BBSQW bit to zero
     */
    void reset_BBSQW(void);

    /**
     * checks the status of CONV bit and soores in to a variable with bool dtype
     */
    bool status_CONV(void);

    /**
     * set the CON bit to high
     */
    void set_CONV(void);

    /**CON
     * set the EOSC bit to zero
     */
    void reset_CONV(void);

    /**
     * checks the status of INTCN bit and soores in to a variable with bool dtype
     */
    bool status_INTCN(void);

    /**
     * set the INTCN bit to high
     */
    void set_INTCN(void);

    /**
     * set the INTCN bit to zero
     */
    void reset_INTCN(void);

    /**
     * checks the status of A2IE bit and soores in to a variable with bool dtype
     */
    bool status_A2IE(void);

    /**
     * set the A2IE bit to high
     */
    void set_A2IE(void);

    /**
     * set the A2IE bit to zero
     */
    void reset_A2IE(void);

    /**
     * checks the status of A1IE bit and soores in to a variable with bool dtype
     */
    bool status_A1IE(void);

    /**
     * set the A1IE bit to high
     */
    void set_A1IE(void);

    /**
     * set the A1IE bit to zero
     */
    void reset_A1IE(void);

    /**
     * It can set the square wave output freqancy, definitly recomend to go throughdatasheet
     * @param Type_set 0
     * @param Type_set 1
     * @param Type_set 2
     * @param Type_set 3
     */
    inline void set_sq_wave_freq(uint8_t Type_set);

    void set_time(uint8_t *Time_setup);

    uint8_t bcd_to_binary(unsigned int bcd);

    /**
     * It will convert time to BCD
     * @param time time in binary i.e, in uint8_t
     */
    uint8_t BCD(uint8_t time);

    private:
    
    uint8_t read_byte(uint8_t location);

    inline void write_byte(uint8_t data, uint8_t location);

    inline void write_array(uint8_t *data, uint8_t *start_location,size_t data_size);

    uint8_t BCD_TIME[7];
    uint8_t *_ptr_BCD_TIME = &BCD_TIME[0];
    uint8_t _len_BCD_Time = length_8bit_arr(BCD_TIME);
    uint8_t DS3231_adddr;                       // type in the DS3231 I2C address
};

#endif