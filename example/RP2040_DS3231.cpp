#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "RP2040_DS3231.hpp"




    /**
 * @param Time::year      enter year
 * @param Time::Month     enter Month
 * @param Time::date      enter date
 * @param Time::day       enter day
 * @param Time::hour      enter hour
 * @param Time::minute    enter minute
 * @param Time::seconds   enter seconds
 */
struct DS_3231::Time {   
                uint8_t year = 24; 
                uint8_t month = 7;
                uint8_t date = 7;
                uint8_t day = 1;
                uint8_t hour = 1;
                uint8_t minutes = 40;
                uint8_t seconds = 0;
            };


DS_3231::DS_3231 (uint8_t _ds3231_adddr){
    DS3231_adddr  = _ds3231_adddr;
} //DS3231 Address

/**this is a general purpose moemory location for better memory managenent. 
 * this can be used by the programmer if required but better not to use
 * but can be freed afterwards for memory safty
 * @param general_purpose_memory_location uint8_t || 5 units ||
 */
uint8_t *general_purpose_memory_location = new uint8_t[5];


//////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This will set the RTC time in 12 Hour mode
 * @param *Time_setup location to time array
 */
inline void DS_3231::set_time_12_hr(uint8_t *Time_setup){
    uint8_t *initial_loc = Addr_Seconds;
    for (uint8_t i = 0; i < 7; i++)
    {
        Time_setup[i] = BCD(Time_setup[i]);
    }
    Time_setup[2] =  Time_setup[2]|(1<<6);
    write_array(Time_setup,initial_loc,sizeof(uint8_t[7]));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * This will set the RTC time in 24 Hour mode
 * @param *Time_setup location to time array
 */
inline void DS_3231::set_time_24_hr(uint8_t *Time_setup){
    uint8_t initial_loc = Addr_Seconds;
    for (uint8_t i = 0; i < 7; i++)
    {
        Time_setup[i] = BCD(Time_setup[i]);
    }
    Time_setup[2] =  Time_setup[2]&~(1<<6);
    write_array(Time_setup,Addr_Seconds,sizeof(uint8_t[7]));
}

void DS_3231::set_time(uint8_t *Time_setup){
    uint8_t *initial_loc = Addr_Seconds;
    for (uint8_t i = 0; i < 7; i++)
    {
        Time_setup[i] = BCD(Time_setup[i]);
    }
    //Time_setup[2] =  Time_setup[2]&~(1<<6);
    i2c_write_blocking(i2c0,DS3231_adddr,initial_loc,1,true);
    i2c_write_blocking(i2c0,DS3231_adddr,Time_setup,7,false);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * it will fetch the time for you to the given address/pointer of location as parameter
 * @param *location the pointer or address to the location where the time to be copied. 
 *                  it should be a 7 byte long uint8_t array
 */

inline void DS_3231::get_time(uint8_t *location){
    uint8_t *initial_loc = Addr_Seconds;
    #ifdef I2C1
    i2c_read_blocking(i2c1,DS3231_adddr,location,sizeof(uint8_t[7]),false);
    #else
    i2c_write_blocking(i2c0,DS3231_adddr,initial_loc,1,true);
    i2c_read_blocking(i2c0,DS3231_adddr,location,((sizeof(uint8_t[7]))/sizeof(uint8_t)),false);
    #endif
} //output will be at DS_3231::Time_Output

void DS_3231::read_time(uint8_t *location){
    uint8_t *initial_loc = Addr_Seconds;
    #ifdef I2C1
    i2c_read_blocking(i2c1,DS3231_adddr,location,sizeof(uint8_t[7]),false);
    #else
    i2c_write_blocking(i2c0,DS3231_adddr,initial_loc,1,false);
    i2c_read_blocking(i2c0,DS3231_adddr,location,((sizeof(uint8_t[7]))/sizeof(uint8_t)),false);
    #endif

    for (uint8_t i = 0; i < 7; i++) location[i] = bcd_to_binary(location[i]) ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * It will set alaram in RTC
 * @param Alarm 0 -> Alarm_1 || 1 -> Alarm_2
 * @param DY_DT 0 -> Date || 1 -> Day
 * @param hrs_fmt 0 -> 24 || 1 -> 12
 * @param data pointer or address to the data location   
 */
inline void DS_3231::set_Alarm(bool Alarm, bool DY_DT, bool hrs_fmt, uint8_t *data){
    for (uint8_t i = 0; i < 4; i++)
    {
        data[i] = BCD(data[i]); 
    }
    
    if (DY_DT) data[3] | (1<<6);
    if (hrs_fmt) data[2] | (1<<6);
    
    general_purpose_memory_location[0] = Addr_Alarm_1_Seconds;
    write_array(data,&general_purpose_memory_location[0],sizeof(uint8_t[4]));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * it will write the data into the control status register
 * @param data the 8bit data to be written
 */
inline void DS_3231::control_status_write(uint8_t data){
    general_purpose_memory_location[2] = Addr_Control_Status;
    write_byte(data,general_purpose_memory_location[2]);
} 

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * it will write the data into the control register
 * @param data the 8bit data to be written
 */
inline void DS_3231::control_write(uint8_t data){
    general_purpose_memory_location[3] = Addr_Control;
    write_byte(data,general_purpose_memory_location[3]);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * it wil give the time in binary form from bcd
 * @param *location the address of pointer to the location of aray
 * @param length the length of array 
 */
inline void DS_3231::get_time_from_bcd(uint8_t *location, uint8_t length){
    for (uint8_t i = 0; i < length; i++)
    {
        location[i] = bcd_to_binary(location[i]);
    }
    
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * it will read the temprature from in-built temprature sensor and store in a variable
 * the output will be in signed char format    
 * */
int8_t DS_3231::read_temp(void){
    general_purpose_memory_location[4] = Addr_MSB_Temp;
    return read_byte(general_purpose_memory_location[4]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
 * it will reat the aging offset register.
 * the output will be in signed char format  
 */
int8_t DS_3231::read_Aging_Offset(void){
    general_purpose_memory_location[0] = Addr_Aging_Offset;
    return read_byte(general_purpose_memory_location[0]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * It wll setup the Alarm_1 Bits Mask, definitly recomends to read datasheet
 * @param Type_set 0 
 * @param Type_set 1
 * @param Type_set 2
 * @param Type_set 3
 */
inline void DS_3231::Alarm_Bits_Mask_1(uint8_t Type_set){

    general_purpose_memory_location[0] = Addr_Alarm_1_Seconds;
    general_purpose_memory_location[1] = Addr_Alarm_1_Seconds+1;
    general_purpose_memory_location[2] = Addr_Alarm_1_Seconds+2;
    general_purpose_memory_location[3] = Addr_Alarm_1_Seconds+3;
    switch (Type_set)
    {
    case 0:
        write_byte((1<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[3]),general_purpose_memory_location[3]);
        break;
    case 1:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[3]),general_purpose_memory_location[3]);
        break;

    case 2:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[3]),general_purpose_memory_location[3]);
        break;

    case 3:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[3]),general_purpose_memory_location[3]);
        break;

    case 4:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[3]),general_purpose_memory_location[3]);
        break;

    default:
        break;
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * It wll setup the Alarm_2 Bits Mask, definitly recomends to read datasheet
 * @param Type_set 0 
 * @param Type_set 1
 * @param Type_set 2
 */
inline void DS_3231::Alarm_Bits_Mask_2(uint8_t Type_set){

    general_purpose_memory_location[0] = Addr_Alarm_2_Minutes;
    general_purpose_memory_location[1] = Addr_Alarm_2_Minutes+1;
    general_purpose_memory_location[2] = Addr_Alarm_2_Minutes+2;
    switch (Type_set)
    {
    case 0:
    
        write_byte((1<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        break;
    case 1:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        break;

    case 2:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((1<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        break;

    case 3:
        write_byte((0<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[1]),general_purpose_memory_location[1]);
        write_byte((0<<7)|read_byte(general_purpose_memory_location[2]),general_purpose_memory_location[2]);
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of 32khz output enable and soores in to a variable with bool dtype
 */
bool DS_3231::status_EN32kHz(void){
    general_purpose_memory_location[0] = Addr_Control_Status;
    if ((1<<3) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the 32khz output enable bit to high
 */
void DS_3231::set_EN32kHz(void){
    general_purpose_memory_location[0] = Addr_Control_Status;
    write_byte((1<<3)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the 32khz output enable bit to zero
 */
void DS_3231::reset_EN32kHz(void){
    general_purpose_memory_location[0] = Addr_Control_Status;
    write_byte(~(1<<3)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

void DS_3231::Alarm_Bits_Mask_1_Test(uint8_t val){
    Alarm_Bits_Mask_1(val);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of OSF bit and soores in to a variable with bool dtype
 */
bool DS_3231::status_osf(void){
    general_purpose_memory_location[0] = Addr_Control_Status;
    if ((1<<7) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the OSF bit to high
 */
void DS_3231::set_osf(void){
    general_purpose_memory_location[0] = Addr_Control_Status;
    write_byte((1<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the OSF bit to zero
 */
void DS_3231::reset_osf(void){
    general_purpose_memory_location[0] = Addr_Control_Status;
    write_byte(~(1<<7)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of EOSC bit and soores in to a variable with bool dtype
 * @param note it is inverted outputs ie, it is not inverted with respect to data sheet for som application
 */
bool DS_3231::status_EOSC(void){
    general_purpose_memory_location[0] = Addr_Control;
    if ((1<<7) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the EOSC bit to high
 */
void DS_3231::set_EOSC(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte(~(1<<7)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the EOSC bit to zero
 */
void DS_3231::reset_EOSC(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte((1<<7)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of OSF bit and soores in to a variable with bool dtype
 */
bool DS_3231::status_BBSQW(void){
    general_purpose_memory_location[0] = Addr_Control;
    if ((1<<6) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the BBSQW bit to high
 */
void DS_3231::set_BBSQW(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte((1<<6)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the BBSQW bit to zero
 */
void DS_3231::reset_BBSQW(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte(~(1<<6)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of CONV bit and soores in to a variable with bool dtype
 */
bool DS_3231::status_CONV(void){
    general_purpose_memory_location[0] = Addr_Control;
    if ((1<<5) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the CON bit to high
 */
void DS_3231::set_CONV(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte((1<<5)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**CON
 * set the EOSC bit to zero
 */
void DS_3231::reset_CONV(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte(~(1<<5)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of INTCN bit and soores in to a variable with bool dtype
 */
bool DS_3231::status_INTCN(void){
    general_purpose_memory_location[0] = Addr_Control;
    if ((1<<2) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the INTCN bit to high
 */
void DS_3231::set_INTCN(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte((1<<2)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the INTCN bit to zero
 */
void DS_3231::reset_INTCN(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte(~(1<<2)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of A2IE bit and soores in to a variable with bool dtype
 */
bool DS_3231::status_A2IE(void){
    general_purpose_memory_location[0] = Addr_Control;
    if ((1<<1) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the A2IE bit to high
 */
void DS_3231::set_A2IE(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte((1<<1)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the A2IE bit to zero
 */
void DS_3231::reset_A2IE(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte(~(1<<1)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * checks the status of A1IE bit and soores in to a variable with bool dtype
 */
bool DS_3231::status_A1IE(void){
    general_purpose_memory_location[0] = Addr_Control;
    if ((1<<0) & read_byte(general_purpose_memory_location[0])) return 1;
    else return 0;
}

/**
 * set the A1IE bit to high
 */
void DS_3231::set_A1IE(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte((1<<0)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

/**
 * set the A1IE bit to zero
 */
void DS_3231::reset_A1IE(void){
    general_purpose_memory_location[0] = Addr_Control;
    write_byte(~(1<<0)&read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * It can set the square wave output freqancy, definitly recomend to go throughdatasheet
 * @param Type_set 0
 * @param Type_set 1
 * @param Type_set 2
 * @param Type_set 3
 */
inline void DS_3231::set_sq_wave_freq(uint8_t Type_set){

    general_purpose_memory_location[0] = Addr_Control;
    switch (Type_set)
    {
    case 0:
        write_byte((0x03<<4)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        break;
    case 1:
        write_byte((0x02<<4)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        break;

    case 2:
        write_byte((0x01<<4)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        break;

    case 3:
        write_byte((0<<4)|read_byte(general_purpose_memory_location[0]),general_purpose_memory_location[0]);
        break;

    default:
        break;
    }
}



/**
 * It will convert time to BCD
 * @param time time in binary i.e, in uint8_t
 */
uint8_t DS_3231::BCD(uint8_t n){
    // Base Case: If n is 0, return 0 (since BCD for 0 is 0000)
    if (n == 0) {
        return 0;
    }

    // Get the last digit (LSB) of n
    int lastDigit = n % 10;

    // Recursively convert the remaining digits
    int bcdValue = BCD(n / 10);

    // Combine the BCD value with the current digit
    return (bcdValue << 4) | lastDigit;
}

/**
 * reads data from the i2c bus
 * @param location the DS3231 reg address from the data is to be retreved
 */
uint8_t DS_3231::read_byte(uint8_t location){
    uint8_t data_required = 0;
    #ifdef I2C1
    i2c_write_blocking(i2c1,DS3231_adddr,&location,1,true);
    i2c_read_blocking(i2c0,DS3231_adddr,&data_required,1,false);
    #else
    i2c_write_blocking(i2c0,DS3231_adddr,&location,1,true);
    i2c_read_blocking(i2c0,DS3231_adddr,&data_required,1,false);
    #endif

    return data_required;
}

/**
 * writes data to the i2c bus
 * @param data the data to be written
 * @param location the DS3231 reg address from the data is to be written
 */
inline void DS_3231::write_byte(uint8_t data, uint8_t location){
    #ifdef I2C1
    i2c_write_blocking(i2c1,DS3231_adddr,&location,1,true);
    i2c_write_blocking(i2c0,DS3231_adddr,&data,1,false);
    #else
    i2c_write_blocking(i2c0,DS3231_adddr,&location,1,true);
    i2c_write_blocking(i2c0,DS3231_adddr,&data,1,false);
    #endif
}

/**
 * writes an array of data in to the i2c bus
 * @param *data pointer or address to the data buffer
 * @param start_location the location where the writting starts in DS3231
 * @param data_size sizeof(data) 
 * 
 */
inline void DS_3231::write_array(uint8_t *data, uint8_t *start_location,size_t data_size){
    #ifdef I2C1
    i2c_write_blocking(i2c1,DS3231_adddr,start_location,(sizeof(start_location)/sizeof(uint8_t)),true);
    i2c_write_blocking(i2c0,DS3231_adddr,data,data_size,false);
    #else
    i2c_write_blocking(i2c0,DS3231_adddr,start_location,(sizeof(start_location)/sizeof(uint8_t)),false);
    i2c_write_blocking(i2c0,DS3231_adddr,data,(data_size/sizeof(uint8_t)),false);
    #endif
}

// Function to convert BCD to binary recursively
// @param bcd the bcd value to be converted to binary
uint8_t DS_3231::bcd_to_binary(unsigned int bcd) {
    // Base case: If BCD number is 0, return 0
    if (bcd == 0) {
        return 0;
    } 
    else {
        // Extract the least significant BCD digit
        unsigned int digit = bcd & 0xF;

        // Recursive call to process the rest of the BCD number
        // Multiply the recursive result by 10 (since we're converting BCD to decimal)
        return (bcd_to_binary(bcd >> 4) * 10) + digit;
    }

}




