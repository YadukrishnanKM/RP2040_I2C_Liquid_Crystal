# RP2040 RTC Alarm & Interrupt Handling

## Overview
This project demonstrates the use of the Raspberry Pi Pico (RP2040) for real-time clock (RTC) alarm-based interrupt handling using the DS3231 RTC module and an I2C-based LCD display.

**Author**: K M Ydukrishnan  
**License**: MIT  

## Features
- Configures and sets alarms on the DS3231 RTC module
- Uses interrupt pins to trigger events based on RTC alarms
- Displays RTC time on a PCF8574-based I2C LCD
- Controls GPIO-based relays and switches upon alarm activation

## Hardware Requirements
- Raspberry Pi Pico or RP2040-based microcontroller
- DS3231 RTC module (I2C interface)
- PCF8574-based I2C LCD display (e.g., 16x2, 20x4)
- Relay module (optional)
- Switches and pull-up resistors (as required)

## Wiring Instructions

| Component | Pico Pin |
|-----------|---------|
| DS3231 SDA | GP16 |
| DS3231 SCL | GP17 |
| LCD SDA | GP16 |
| LCD SCL | GP17 |
| RTC Alarm Interrupt | GP20 |
| Moisture Sensor Interrupt | GP21 |
| Alarm Set Interrupt | GP22 |
| Reset Pin | GP26 |
| Relay Output | GP27 |
| Switch Input | GP15 |

> **Note:** Ensure that I2C pull-up resistors are present on the SDA and SCL lines.

## Installation & Compilation
### 1. Clone the Repository
```sh
  git clone https://github.com/YadukrishnanKM/RP2040_I2C_Liquid_Crystal/tree/main/example
```
> **Note (Recommended):** or else you can proceed with normal setup of above normal [CMAKE](https://github.com/YadukrishnanKM/RP2040_I2C_Liquid_Crystal/blob/main/example/CMakeLists.txt) file
### 2. CMake Configuration
Add the following `CMakeLists.txt` file in your project directory:
```cmake
cmake_minimum_required(VERSION 3.13)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(RP2040_RTC_Interrupts)

pico_sdk_init()

add_executable(RP2040_RTC_Interrupts
    main.cpp
    Liquid_Crystal_RP.cpp
    RP2040_DS3231.cpp
)

# Link Pico SDK and hardware libraries
target_link_libraries(RP2040_RTC_Interrupts pico_stdlib hardware_i2c hardware_rtc)

# Add include directories
target_include_directories(RP2040_RTC_Interrupts PUBLIC ${CMAKE_CURRENT_LIST_DIR})

pico_add_extra_outputs(RP2040_RTC_Interrupts)
```

### 3. Build and Flash
```sh
mkdir build && cd build
cmake ..
make -j4
picotool load -f RP2040_RTC_Interrupts.uf2
```
> **Note (Recommended):** in case of dysfunctionality  you can proceed with normal setup 

## Example Code Usage
```cpp
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "Liquid_Crystal_RP.hpp"
#include "RP2040_DS3231.hpp"
#include "pico/sleep.h"

#define LCD_SDA 16
#define LCD_SCL 17
#define RTC_ALARM_PIN 20

int main() {
    stdio_init_all();
    i2c_init(i2c0, 100000);
    
    DS_3231 rtc(0x68);
    LiquidCrystal_I2C lcd(0x27, 16, 2, 0);
    lcd.begin();
    lcd.backlight();

    while (1) {
        uint8_t time[7];
        rtc.read_time(time);
        lcd.setCursor(0, 0);
        lcd.printstr((const uint8_t *)"Time: ", 6);
        lcd.setCursor(6, 0);
        char buffer[10];
        sprintf(buffer, "%02d:%02d:%02d", time[2], time[1], time[0]);
        lcd.printstr((const uint8_t *)buffer, 8);
        sleep_ms(1000);
    }
    return 0;
}
```

## Future Improvements
- Improved power management with deep sleep modes
- Support for multiple alarms
- Data logging capabilities with an SD card
- Wireless notification integration

## Contributions
Contributions are welcome! Please submit pull requests or open issues for feature requests or bug reports.

**License:** MIT

