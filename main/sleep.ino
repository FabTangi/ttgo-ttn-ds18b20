/*

Sleep module

Copyright (C) 2018 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <esp_sleep.h>
//#include "esp_deep_sleep.h"
//wifi and bluetooth libraries
/*
#include <esp_wifi.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt.h>
//rtc libraries
#include "driver/rtc_io.h"
*/
void sleep_interrupt(uint8_t gpio, uint8_t mode) {
    //esp_sleep_enable_ext0_wakeup((gpio_num_t) gpio, mode);
}

void sleep_interrupt_mask(uint64_t mask, uint8_t mode) {
    //esp_sleep_enable_ext1_wakeup(mask, (esp_sleep_ext1_wakeup_mode_t) mode);
}

void sleep_millis(uint64_t ms) {


   //removedn make rebooting
   esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_ON);
   esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON);
   esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
   
   Serial.println("Configured all RTC Peripherals to be powered down in sleep");
    //esp_wifi_stop();
    //esp_wifi_deinit();
    //esp_bluedroid_disable();
    //esp_bluedroid_deinit();
    //esp_bt_controller_disable();
    //esp_bt_controller_deinit();
   
    esp_sleep_enable_timer_wakeup(ms * 1000);

    Serial.flush(); 
    delay(100);
    esp_deep_sleep_start();
    //esp_light_sleep_start(); //17mA
}

void sleep_seconds(uint32_t seconds) {
  Serial.println("sleep seconds");
    esp_sleep_enable_timer_wakeup(seconds * 1000000);
    esp_deep_sleep_start();
}

void sleep_forever() {
    esp_deep_sleep_start();
}

void print_wakeup_reason() {
  digitalWrite(BUILTIN_LED, HIGH);
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); print_wakeup_touchpad(); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

/*
Method to print the touchpad by which ESP32
has been awaken from sleep
*/
void print_wakeup_touchpad(){
  touch_pad_t pin;

  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch(touchPin)
  {
    case 0  : Serial.println("Touch detected on GPIO 4"); break;
    case 1  : Serial.println("Touch detected on GPIO 0"); break;
    case 2  : Serial.println("Touch detected on GPIO 2"); break;
    case 3  : Serial.println("Touch detected on GPIO 15"); break;
    case 4  : Serial.println("Touch detected on GPIO 13"); break;
    case 5  : Serial.println("Touch detected on GPIO 12"); break;
    case 6  : Serial.println("Touch detected on GPIO 14"); break;
    case 7  : Serial.println("Touch detected on GPIO 27"); break;
    case 8  : Serial.println("Touch detected on GPIO 33"); break;
    case 9  : Serial.println("Touch detected on GPIO 32"); break;
    default : Serial.println("Wakeup not by touchpad"); break;
  }
}
