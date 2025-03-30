/**
 * created by K M Ydukrishnan
 * It is a Pico SDK version for fdebander Arduino-LiquidCrystal-I2C-library
 * It is designed to ensure max compatibility with Arduino-LiquidCrystal-I2C-library
 * 
 * Note: 	It is still inn design phase so few commands will not work as intended
 * 			please provide your valuable feedback
 * 
 * credits: Arduino, fdebander
 **/

//#include "hardware/i2c.h"
//#include "pico/stdlib.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "Liquid_Crystal_RP.hpp"

// commands
#ifndef LCD_CLEARDISPLAY
#define LCD_CLEARDISPLAY 0x01
#endif

#ifndef LCD_RETURNHOME
#define LCD_RETURNHOME 0x02
#endif

#ifndef LCD_ENTRYMODESET
#define LCD_ENTRYMODESET 0x04
#endif

#ifndef LCD_DISPLAYCONTROL
#define LCD_DISPLAYCONTROL 0x08
#endif

#ifndef LCD_CURSORSHIFT
#define LCD_CURSORSHIFT 0x10
#endif

#ifndef LCD_FUNCTIONSET
#define LCD_FUNCTIONSET 0x20
#endif

#ifndef LCD_SETCGRAMADDR
#define LCD_SETCGRAMADDR 0x40
#endif

#ifndef LCD_SETDDRAMADDR
#define LCD_SETDDRAMADDR 0x80
#endif

// flags for display entry mode

#ifndef LCD_ENTRYRIGHT
#define LCD_ENTRYRIGHT 0x00
#endif

#ifndef LCD_ENTRYLEFT
#define LCD_ENTRYLEFT 0x02
#endif

#ifndef LCD_ENTRYSHIFTINCREMENT
#define LCD_ENTRYSHIFTINCREMENT 0x01
#endif

#ifndef LCD_ENTRYSHIFTDECREMENT
#define LCD_ENTRYSHIFTDECREMENT 0x00
#endif

// flags for display on/off control

#ifndef LCD_DISPLAYON
#define LCD_DISPLAYON 0x04
#endif

#ifndef LCD_DISPLAYOFF
#define LCD_DISPLAYOFF 0x00
#endif

#ifndef LCD_CURSORON
#define LCD_CURSORON 0x02
#endif

#ifndef LCD_CURSOROFF
#define LCD_CURSOROFF 0x00
#endif

#ifndef LCD_BLINKON
#define LCD_BLINKON 0x01
#endif

#ifndef LCD_BLINKOFF
#define LCD_BLINKOFF 0x00
#endif

// flags for display/cursor shift

#ifndef LCD_DISPLAYMOVE
#define LCD_DISPLAYMOVE 0x08
#endif

#ifndef LCD_CURSORMOVE
#define LCD_CURSORMOVE 0x00
#endif

#ifndef LCD_MOVERIGHT
#define LCD_MOVERIGHT 0x04
#endif

#ifndef LCD_MOVELEFT
#define LCD_MOVELEFT 0x00
#endif


// flags for function set

#ifndef LCD_8BITMODE
#define LCD_8BITMODE 0x10
#endif

#ifndef LCD_4BITMODE
#define LCD_4BITMODE 0x00
#endif

#ifndef LCD_2LINE
#define LCD_2LINE 0x08
#endif

#ifndef LCD_1LINE
#define LCD_1LINE 0x00
#endif

#ifndef LCD_5x10DOTS
#define LCD_5x10DOTS 0x04
#endif

#ifndef LCD_5x8DOTS
#define LCD_5x8DOTS 0x00
#endif

// flags for backlight control

#ifndef LCD_BACKLIGHT
#define LCD_BACKLIGHT 0x08
#endif

#ifndef LCD_NOBACKLIGHT
#define LCD_NOBACKLIGHT 0x00
#endif

#ifndef En 
#define En 0b00000100  // Enable bit
#endif

#ifndef Rw
#define Rw 0b00000010  // Read/Write bit
#endif

#ifndef Rs
#define Rs 0b00000001  // Register select bit
#endif

#define PCF8574_BAUDRATE    100000

//Default I2C is i2c0 
            
LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize){

    _addr = lcd_addr;
    _cols = lcd_cols;
    _rows = lcd_rows;
    _charsize = charsize;
    _backlightval = LCD_BACKLIGHT;
}


void LiquidCrystal_I2C::begin(){

	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (_rows > 1) {
		_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((_charsize != 0) && (_rows == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	sleep_ms(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	sleep_ms(1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
	sleep_us(4500); // wait min 4.1ms

	// second try
	write4bits(0x03 << 4);
	sleep_us(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4);
	sleep_us(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;

	display();

	// clear it off
	clear();

	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);

	home();
}

/********** high level commands, for the user! */
void LiquidCrystal_I2C::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	sleep_us(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	sleep_us(2000);  // this command takes a long time!
}

void LiquidCrystal_I2C::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > _rows) {
		row = _rows-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal_I2C::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal_I2C::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal_I2C::write_charactor(uint8_t val){
	write(val);
}

void LiquidCrystal_I2C::write_string(uint8_t *val, size_t size){
	uint8_t len = size/sizeof(uint8_t);

	for (uint8_t i = 0; i < (len-1); i++){
		write(val[i]);
		sleep_us(600);
	}
}

// Turn on and off the blinking cursor
void LiquidCrystal_I2C::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void LiquidCrystal_I2C::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal_I2C::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LiquidCrystal_I2C::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal_I2C::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal_I2C::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal_I2C::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal_I2C::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal_I2C::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void LiquidCrystal_I2C::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void LiquidCrystal_I2C::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}
bool LiquidCrystal_I2C::getBacklight() {
return _backlightval == LCD_BACKLIGHT;
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal_I2C::command(uint8_t value) {
	send(value, 0);
}

inline size_t LiquidCrystal_I2C::write(uint8_t value) {
	send(value, Rs);
	return 1;
}
/*
inline void LiquidCrystal_I2C::print_byt(uint8_t value) {
	send(value, 0);
}*/
void LiquidCrystal_I2C::printstr(const uint8_t *c, uint8_t size){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed
	
	uint8_t len = size/sizeof(uint8_t);

	for (uint8_t i = 0; i < len; i++){
		send(c[i],0);
		sleep_us(10);
	}

}

/************ low level data pushing commands **********/

void LiquidCrystal_I2C::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;

	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void LiquidCrystal_I2C::write4bits(uint8_t value){
	expanderWrite(value);
	pulseEnable(value);
}
#ifdef I2C1
void LiquidCrystal_I2C::expanderWrite(uint8_t _data){
	_data = _data | _backlightval;
	i2c_write_blocking(i2c1, _addr, &_data, sizeof(_data), false);
	sleep_ms(600);
}
#else
void LiquidCrystal_I2C::expanderWrite(uint8_t _data){
	_data = _data | _backlightval;
	i2c_write_blocking(i2c0, _addr, &_data, sizeof(_data), false);
	sleep_us(600);
}
#endif
void LiquidCrystal_I2C::pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	sleep_us(600);		// enable pulse must be >450ns

	expanderWrite(_data & ~En);	// En low
	sleep_us(600);		// commands need > 37us to settle
}
