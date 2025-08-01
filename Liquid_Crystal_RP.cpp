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

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "Liquid_Crystal_RP.hpp"


#define PCF8574_BAUDRATE    100000

//Default I2C is i2c0 
class LiquidCrystal_I2C {
    public:

            
        LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize){

            _addr = lcd_addr;
            _cols = lcd_cols;
            _rows = lcd_rows;
            _charsize = charsize;
            _backlightval = LCD_BACKLIGHT;
        }


        void begin(){

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
        void clear(){
            command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
            sleep_us(2000);  // this command takes a long time!
        }
        
        void home(){
            command(LCD_RETURNHOME);  // set cursor position to zero
            sleep_us(2000);  // this command takes a long time!
        }

        void setCursor(uint8_t col, uint8_t row){
            int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
            if (row > _rows) {
                row = _rows-1;    // we count rows starting w/0
            }
            command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
        }

        // Turn the display on/off (quickly)
        void noDisplay() {
            _displaycontrol &= ~LCD_DISPLAYON;
            command(LCD_DISPLAYCONTROL | _displaycontrol);
        }

        void display() {
            _displaycontrol |= LCD_DISPLAYON;
            command(LCD_DISPLAYCONTROL | _displaycontrol);
        }

        // Turns the underline cursor on/off
        void noCursor() {
            _displaycontrol &= ~LCD_CURSORON;
            command(LCD_DISPLAYCONTROL | _displaycontrol);
        }
        void cursor() {
            _displaycontrol |= LCD_CURSORON;
            command(LCD_DISPLAYCONTROL | _displaycontrol);
        }

        // Turn on and off the blinking cursor
        void noBlink() {
            _displaycontrol &= ~LCD_BLINKON;
            command(LCD_DISPLAYCONTROL | _displaycontrol);
        }
        void blink() {
            _displaycontrol |= LCD_BLINKON;
            command(LCD_DISPLAYCONTROL | _displaycontrol);
        }

        // These commands scroll the display without changing the RAM
        void scrollDisplayLeft(void) {
            command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
        }
        void scrollDisplayRight(void) {
            command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
        }

        // This is for text that flows Left to Right
        void leftToRight(void) {
            _displaymode |= LCD_ENTRYLEFT;
            command(LCD_ENTRYMODESET | _displaymode);
        }

        // This is for text that flows Right to Left
        void rightToLeft(void) {
            _displaymode &= ~LCD_ENTRYLEFT;
            command(LCD_ENTRYMODESET | _displaymode);
        }

        // This will 'right justify' text from the cursor
        void autoscroll(void) {
            _displaymode |= LCD_ENTRYSHIFTINCREMENT;
            command(LCD_ENTRYMODESET | _displaymode);
        }

        // This will 'left justify' text from the cursor
        void noAutoscroll(void) {
            _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
            command(LCD_ENTRYMODESET | _displaymode);
        }

        // Allows us to fill the first 8 CGRAM locations
        // with custom characters
        void createChar(uint8_t location, uint8_t charmap[]) {
            location &= 0x7; // we only have 8 locations 0-7
            command(LCD_SETCGRAMADDR | (location << 3));
            for (int i=0; i<8; i++) {
                write(charmap[i]);
            }
        }

        // Turn the (optional) backlight off/on
        void noBacklight(void) {
            _backlightval=LCD_NOBACKLIGHT;
            expanderWrite(0);
        }

        void backlight(void) {
            _backlightval=LCD_BACKLIGHT;
            expanderWrite(0);
        }
        bool getBacklight() {
        return _backlightval == LCD_BACKLIGHT;
        }

        /*********** mid level commands, for sending data/cmds */

         void command(uint8_t value) {
            send(value, 0);
        }

         size_t write(uint8_t value) {
            send(value, Rs);
            return 1;
        }

         void print(uint8_t value) {
            send(value, 0);
        }
        void printstr(const uint8_t *c, uint8_t size){
            //This function is not identical to the function used for "real" I2C displays
            //it's here so the user sketch doesn't have to be changed
            
            uint8_t len = size/sizeof(uint8_t);

            for (uint8_t i = 0; i < len; i++){
                print(c[i]);
                sleep_us(1);
            }
        
        }

        /************ low level data pushing commands **********/

    private:
        void send(uint8_t value, uint8_t mode) {
            uint8_t highnib=value&0xf0;
	        uint8_t lownib=(value<<4)&0xf0;

            write4bits((highnib)|mode);
	        write4bits((lownib)|mode);
        }
        
        void write4bits(uint8_t value){
            expanderWrite(value);
	        pulseEnable(value);
        }
        #ifdef I2C1
        void expanderWrite(uint8_t _data){
            _data = _data | _backlightval;
            i2c_write_blocking(i2c1, _addr, &_data, sizeof(_data), false);
            sleep_us(10);
        }
        #else
        void expanderWrite(uint8_t _data){
            _data = _data | _backlightval;
            i2c_write_blocking(i2c0, _addr, &_data, sizeof(_data), false);
            sleep_us(10);
        }
        #endif
        void pulseEnable(uint8_t _data){
            expanderWrite(_data | En);	// En high
            sleep_us(1);		// enable pulse must be >450ns

            expanderWrite(_data & ~En);	// En low
            sleep_us(50);		// commands need > 37us to settle
        }

};
