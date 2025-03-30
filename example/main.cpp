#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "Liquid_Crystal_RP.hpp"
#include "RP2040_DS3231.hpp"
#include "pico/sleep.h"
#include <stdio.h>

#define _lcd_gpio_sda 16
#define _lcd_gpio_scl 17
#define _rtc_alarm_set_irq_intrpt_pin 22
#define _moist_irq_intrpt_pin 21
#define _rtc_alarm_out_irq_intrpt_pin 20

#define reset_pin 26
#define relay_pin 27
#define switch_pin  15

void set_rtc_alarm(uint gpio, uint32_t event){
  DS_3231 rtc(0x68);
  LiquidCrystal_I2C lcd(0x27,16,2,0);
  uint8_t tyu[3];
  printf("\nwrite the time in seconds  :");
  scanf("%d",tyu[0]);
  printf("\nwrite the time in minutes  :");
  scanf("%d",tyu[1]);
  printf("\nwrite the time in hours  :");
  scanf("%d",tyu[2]);

  i2c_write_blocking(i2c0,0x68,(uint8_t *)0x07,1,false);
  i2c_write_blocking(i2c0,0x68,&tyu[0],3,false);

  rtc.Alarm_Bits_Mask_1_Test(3);

  delete [] rtc.general_purpose_memory_location;
}

void intrpt_response(uint gpio, uint32_t event){

  gpio_init(relay_pin);
  gpio_set_dir(relay_pin,GPIO_OUT);

  gpio_init(switch_pin);
  gpio_set_dir(switch_pin,GPIO_OUT);
  gpio_put(switch_pin,true);

  busy_wait_ms(10);
  
  DS_3231 rtc(0x68);
  LiquidCrystal_I2C lcd(0x27,16,2,0);
  lcd.begin();
  lcd.clear();
  lcd.backlight();

  lcd.setCursor(2,0);
  uint8_t hello[] = "Starting...";
  lcd.display();
  lcd.write_string(&hello[0],sizeof(hello));
  lcd.display();
  busy_wait_ms(3500);

  lcd.clear();
  lcd.setCursor(0,0);
  uint8_t tyut[] ="reading RTC"; 
  lcd.write_string(&tyut[0],sizeof(tyut));
  lcd.display();
  busy_wait_ms(3500);

  lcd.clear();
  lcd.setCursor(0,0);
  uint8_t qwe[] ="TIME is :"; 
  lcd.write_string(&qwe[0],sizeof(qwe));
  lcd.display();
  busy_wait_ms(1500);

  lcd.setCursor(0,1);
  uint8_t poi[7];
  rtc.read_time(&poi[0]);

  sprintf((char *)poi, "%d : %",poi[2],poi[1]); 
  lcd.write_string(&poi[0],sizeof(poi));
  lcd.display();
  
  
  gpio_put(relay_pin,true);
  for (uint8_t i = 0; i < 10; i++)
  {
   busy_wait_ms(1000);
  }
  gpio_put(relay_pin,false);
  lcd.clear();

  sleep_ms(10);
  gpio_put(switch_pin,false);

  delete [] rtc.general_purpose_memory_location;
  
}

int main(){

    sleep_run_from_xosc();
    sleep_goto_dormant_until_edge_high(_rtc_alarm_out_irq_intrpt_pin);

    i2c_init(i2c0,PCF8574_BAUDRATE);
    stdio_init_all();

    gpio_init(_rtc_alarm_set_irq_intrpt_pin);
    gpio_init(_rtc_alarm_out_irq_intrpt_pin);
    gpio_init(_moist_irq_intrpt_pin);
    gpio_init(reset_pin);

    gpio_set_dir(_rtc_alarm_set_irq_intrpt_pin,GPIO_IN);
    gpio_set_dir(_rtc_alarm_out_irq_intrpt_pin,GPIO_IN);
    gpio_set_dir(_moist_irq_intrpt_pin,GPIO_IN);
    gpio_set_dir(reset_pin,GPIO_OUT);

    gpio_set_irq_enabled_with_callback(_rtc_alarm_set_irq_intrpt_pin,0x08,true,set_rtc_alarm);
    gpio_set_irq_enabled_with_callback(_rtc_alarm_out_irq_intrpt_pin|_moist_irq_intrpt_pin,0x08,true,intrpt_response);

    gpio_set_function(_lcd_gpio_sda, GPIO_FUNC_I2C);
    gpio_set_function(_lcd_gpio_scl, GPIO_FUNC_I2C);

    gpio_pull_up(_lcd_gpio_sda);
    gpio_pull_up(_lcd_gpio_scl);
    
    gpio_put(reset_pin,1);

    while (true) tight_loop_contents();
    
    return 0;
}


    /*
    LiquidCrystal_I2C lcd(0x27,16,2,0);
    lcd.begin();
    lcd.clear();
    lcd.backlight();

    sleep_ms(500);

    lcd.cursor();
    lcd.setCursor(0,0);
    uint8_t uiy[] = "Hello World";
    //lcd.display();
    lcd.write_string(&uiy[0],sizeof(uiy));
   
    lcd.display();

    sleep_ms(5000);
    lcd.clear();
    
   
   uint8_t time[8];  
    time[0] = 0;
    time[1] = rtc.BCD(07);
    time[2] = rtc.BCD(13);
    time[3] = rtc.BCD(23);
    time[4] = rtc.BCD(13);
    time[5] = rtc.BCD(7);
    time[6] = rtc.BCD(24);

   i2c_write_blocking(i2c0,0x68,0,1,false);
   i2c_write_blocking(i2c0,0x68,&time[0],7,false);
    

  while (true)
  {
    uint8_t l[7]; 
    rtc.read_time(&l[0]);
    sleep_ms(1000);
    printf("\n %x %x %x %x %x %x %x \n",time[6],time[5],time[4],time[3],time[2],time[1],time[0]);
    printf(" hour %d minute %d  sec %d\n",l[2],l[1],l[0]);
  }
  */
