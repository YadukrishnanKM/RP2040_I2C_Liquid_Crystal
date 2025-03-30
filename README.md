# LiquidCrystal_I2C for Raspberry Pi Pico

## Overview
This library is a Raspberry Pi Pico SDK implementation of the popular Arduino LiquidCrystal I2C library. It is designed for compatibility with the original library while utilizing the Pico SDK for hardware interaction.

**Author**: K M Ydukrishnan  
**Based on**: Arduino LiquidCrystal I2C by fdebander  
**License**: MIT  

> **Note:** This library is currently in the design phase. Some commands may not work as intended. Your feedback is valuable for improvements.

## Features
- Compatible with **PCF8574 I2C-based** LCD displays
- Supports 16x2, 20x4, and other compatible LCD screens
- Implements fundamental LCD operations:
  - Display text and custom characters
  - Cursor movement and display control
  - Scrolling, text alignment, and backlight control
- Designed for **maximum compatibility** with the Arduino LiquidCrystal I2C library

## Hardware Requirements
- Raspberry Pi Pico or RP2040-based microcontroller
- I2C-compatible LCD display (e.g., 16x2 or 20x4)
- PCF8574-based I2C expander module

## Wiring Instructions

| LCD Pin | PCF8574 (I2C Expander) | Raspberry Pi Pico |
|---------|-------------------------|-------------------|
| VCC     | VCC                     | 3.3V             |
| GND     | GND                     | GND              |
| SDA     | SDA                     | GP4 (I2C0 SDA)   |
| SCL     | SCL                     | GP5 (I2C0 SCL)   |

> **Note:** The default I2C bus is `i2c0`, but it can be changed based on hardware requirements.

## Installation
1. Clone this repository into your Pico SDK project:
   ```sh
   git clone https://github.com/YadukrishnanKM/RP2040_I2C_Liquid_Crystal
   ```
2. Add the library to your `CMakeLists.txt`:
   ```cmake
   add_subdirectory(LiquidCrystal_I2C_Pico)
   target_link_libraries(your_project_name PRIVATE LiquidCrystal_I2C_Pico)
   ```
3. Include the header in your project:
   ```cpp
   #include "Liquid_Crystal_RP.hpp"
   ```

## CMake Configuration
To build this library using the Raspberry Pi Pico SDK, ensure your `CMakeLists.txt` file includes the following:

```cmake
cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(LiquidCrystal_I2C_Pico)

pico_sdk_init()

add_library(LiquidCrystal_I2C_Pico STATIC Liquid_Crystal_RP.cpp)

# Link with Pico SDK
target_link_libraries(LiquidCrystal_I2C_Pico pico_stdlib hardware_i2c)

# Add include directories
target_include_directories(LiquidCrystal_I2C_Pico PUBLIC ${CMAKE_CURRENT_LIST_DIR})
```

## API Reference

### Constructor
```cpp
LiquidCrystal_I2C(uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t charsize);
```
- `lcd_addr`: I2C address of the LCD module
- `lcd_cols`: Number of columns (e.g., 16 for a 16x2 display)
- `lcd_rows`: Number of rows (e.g., 2 for a 16x2 display)
- `charsize`: LCD character size (use `LCD_5x10DOTS` or `LCD_5x8DOTS`)

### Initialization
```cpp
void begin();
```
Initializes the LCD in **4-bit mode** with default settings.

### Basic Display Functions
```cpp
void clear();       // Clears the screen
void home();        // Moves cursor to home position
void setCursor(uint8_t col, uint8_t row); // Set cursor position
```

### Display Control
```cpp
void display();      // Turns on the display
void noDisplay();    // Turns off the display
void cursor();       // Shows cursor
void noCursor();     // Hides cursor
void blink();        // Blinking cursor on
void noBlink();      // Blinking cursor off
```

### Scrolling and Text Alignment
```cpp
void scrollDisplayLeft();   // Scroll text left
void scrollDisplayRight();  // Scroll text right
void leftToRight();         // Set text direction left to right
void rightToLeft();         // Set text direction right to left
void autoscroll();          // Auto-scroll enabled
void noAutoscroll();        // Auto-scroll disabled
```

### Backlight Control
```cpp
void backlight();     // Turns backlight on
void noBacklight();   // Turns backlight off
bool getBacklight();  // Returns current backlight status
```

### Printing Text
```cpp
void print(uint8_t value);  // Prints a single character
void printstr(const uint8_t *c, uint8_t size);  // Prints a string
```

### Custom Characters
```cpp
void createChar(uint8_t location, uint8_t charmap[]);
```
- `location`: Stores the character at a CGRAM location (0-7)
- `charmap`: 8-byte array representing the custom character

### Low-Level Communication
```cpp
void command(uint8_t cmd);  // Send command to LCD
size_t write(uint8_t value); // Write a single character
```

## Example Code
```cpp
#include "Liquid_Crystal_RP.hpp"
#include "pico/stdlib.h"

int main() {
    stdio_init_all();
    LiquidCrystal_I2C lcd(0x27, 16, 2, LCD_5x8DOTS);
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.printstr((const uint8_t *)"Hello, Pico!", 12);
    while (1);
}
```

## Known Issues & Future Improvements
- Some commands may not be fully implemented
- Future support for multiple I2C buses
- Enhanced compatibility testing with more LCD models

## Contribution & Feedback
Contributions are welcome! Please open an issue or submit a pull request with improvements.

For feedback, please reach out via GitHub or email: `your.email@example.com`.

---

**Credits**:
- Arduino Team
- fdebander for the original LiquidCrystal I2C Library
- Raspberry Pi Pico SDK

**License:** MIT

