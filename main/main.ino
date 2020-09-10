/*

  Main module

  # Modified by Kyle T. Gabriel to fix issue with incorrect GPS data for TTNMapper

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

#include "configuration.h"
#include "rom/rtc.h"
#include "soc/rtc.h"
#include <Wire.h>
#include <Button2.h>
#include <Ticker.h>
#include <WiFi.h>
#if defined(PAYLOAD_USE_CAYENNE)
#include <CayenneLPP.h>
#endif

//#include <LowPower.h>

#include <OneWire.h>
#include <DallasTemperature.h>

int deviceCount = 0; //to count 1 wire devices

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

//Pass our OneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

bool ssd1306_found = false;

float temp = 0;
float onewire = 0;
float vbat = 0;
// Message counter, stored in RTC memory, survives deep sleep
RTC_DATA_ATTR uint32_t bootCount = 0;
RTC_DATA_ATTR uint32_t RTC_seqnoUp = 0;

touch_pad_t touchPin;

#if defined(PAYLOAD_USE_FULL)
uint8_t txBuffer[10];
#elif defined(PAYLOAD_USE_CAYENNE)
// CAYENNE DF
CayenneLPP lpp(51);
#endif

#define BUTTONS_MAP {38}

Button2 *pBtns = nullptr;
uint8_t g_btns[] =  BUTTONS_MAP;

#define ARRARY_SIZE(a)   (sizeof(a) / sizeof(a[0]))

Ticker btnTick;
uint8_t program = 0;

// -----------------------------------------------------------------------------
// Application
// -----------------------------------------------------------------------------

void send() {
  char buffer[40];

#if defined(PAYLOAD_USE_FULL)
  buildPacket(txBuffer);
#elif defined(PAYLOAD_USE_CAYENNE)
  // CAYENNE DF
  //buildPacket(lpp);
  Serial.println("Builpkt OK");
#endif


#if LORAWAN_CONFIRMED_EVERY > 0
  bool confirmed = (count % LORAWAN_CONFIRMED_EVERY == 0);
#else
  bool confirmed = false;
#endif

  ttn_cnt(RTC_seqnoUp);

#if defined(PAYLOAD_USE_FULL)
  ttn_send(txBuffer, sizeof(txBuffer), LORAWAN_PORT, confirmed);
#elif defined(PAYLOAD_USE_CAYENNE)
  // CAYENNE DF
  lpp.reset();
  sensors.requestTemperatures();
  for (int i = 0;  i < deviceCount;  i++)
  {
    temp = sensors.getTempCByIndex(i);
    lpp.addTemperature(i, temp);
    snprintf(buffer, sizeof(buffer), "temp %i : %4.2f Celcius\n", i, temp);
    screen_print(buffer);
  }
  // Battery Voltage
  vbat = (float)(analogRead(VBAT_PIN)) / 4095 * 2 * 3.3 * 1.1;
  lpp.addAnalogInput(deviceCount, vbat);
  snprintf(buffer, sizeof(buffer), "vbat: %4.2fm\n", vbat);
  screen_print(buffer);

  ttn_send(lpp.getBuffer(), lpp.getSize(), LORAWAN_PORT, confirmed);

  snprintf(buffer, sizeof(buffer), "Message sent : %i\n", RTC_seqnoUp);
  screen_print(buffer);

#endif

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN1, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN1, LOW);
  //count++;
  RTC_seqnoUp++;
  Serial.println(RTC_seqnoUp);
}

void sleep() {
#if SLEEP_BETWEEN_MESSAGES

  // Show the going to sleep message on the screen
  char buffer[20];
  snprintf(buffer, sizeof(buffer), "Sleeping in %3.1fs\n", (MESSAGE_TO_SLEEP_DELAY / 1000.0));
  screen_print(buffer);

  // Wait for MESSAGE_TO_SLEEP_DELAY millis to sleep
  delay(MESSAGE_TO_SLEEP_DELAY);

  // Turn off screen
  screen_off();
  LMIC_shutdown(); // cleanly shutdown the radio

  Wire.endTransmission(); // shutdown/power off I2C hardware,
  pinMode(SDA, INPUT); // needed because Wire.end() enables pullups, power Saving
  pinMode(SCL, INPUT);


  //sleep_interrupt(BUTTON_PIN, LOW);


  // We sleep for the interval between messages minus the current millis
  // this way we distribute the messages evenly every SEND_INTERVAL millis
  uint32_t sleep_for = (millis() < SEND_INTERVAL) ? SEND_INTERVAL - millis() : SEND_INTERVAL;
  Serial.println("\n going to sleep \n");
  sleep_millis(sleep_for);

#endif
}

void callback(uint8_t message) {
  if (EV_JOINING == message) screen_print("Joining TTN...\n");
  if (EV_JOINED == message) screen_print("TTN joined!\n");
  if (EV_JOIN_FAILED == message) screen_print("TTN join failed\n");
  if (EV_REJOIN_FAILED == message) screen_print("TTN rejoin failed\n");
  if (EV_RESET == message) screen_print("Reset TTN connection\n");
  if (EV_LINK_DEAD == message) screen_print("TTN link dead\n");
  if (EV_ACK == message) screen_print("ACK received\n");
  if (EV_PENDING == message) screen_print("Message discarded\n");
  //if (EV_QUEUED == message) screen_print("Message queued\n");

  if (EV_TXCOMPLETE == message) {
    sleep();
  }

  if (EV_RESPONSE == message) {

    screen_print("[TTN] Response: ");

    size_t len = ttn_response_len();
    uint8_t data[len];
    ttn_response(data, len);

    char buffer[6];
    for (uint8_t i = 0; i < len; i++) {
      snprintf(buffer, sizeof(buffer), "%02X", data[i]);
      screen_print(buffer);
    }
    screen_print("\n");
  }
}

uint32_t get_count() {
  return bootCount;
}

void scanI2Cdevice(void)
{
  byte err, addr;
  int nDevices = 0;
  for (addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("I2C device found at address 0x");
      if (addr < 16)
        Serial.print("0");
      Serial.print(addr, HEX);
      Serial.println(" !");
      nDevices++;

      if (addr == SSD1306_ADDRESS) {
        ssd1306_found = true;
        Serial.println("ssd1306 display found");
      }

    } else if (err == 4) {
      Serial.print("Unknow error at address 0x");
      if (addr < 16)
        Serial.print("0");
      Serial.println(addr, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

/************************************
        BUTTON
 * *********************************/
void button_callback(Button2 &b)
{
  for (int i = 0; i < ARRARY_SIZE(g_btns); ++i) {
    if (pBtns[i] == b) {
      if (ssd1306_found) {
        //                ui.nextFrame();
      }
      program = program + 1 > 2 ? 0 : program + 1;
    }
  }
}

void button_loop()
{
  for (int i = 0; i < ARRARY_SIZE(g_btns); ++i) {
    pBtns[i].loop();
  }
}

void setup() {
  // Debug
#ifdef DEBUG_PORT
  DEBUG_PORT.begin(SERIAL_BAUD);
#endif
  Serial.println("DEBUT");
  //Reduce clock speed to reduce power consumption.
  rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  Serial.println("RTC_seqnoUp: " + String(RTC_seqnoUp));
  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Buttons & LED
  //pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_PIN1, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(LED_PIN1, HIGH);
  delay(LOGO_DELAY);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_PIN1, LOW);

  //  button_init();

  // Hello
  DEBUG_MSG(APP_NAME " " APP_VERSION "\n");

  // Display
  screen_setup();

  // Init temp
  temp_setup();
  Serial.println("Temp Setup finished");

  // Show logo on first boot
  if (0 == bootCount) {
    screen_print(APP_NAME " " APP_VERSION, 0, 0);
    //screen_show_logo();
    //screen_update();
    delay(LOGO_DELAY);
  }

  // TTN setup
  if (!ttn_setup()) {
    screen_print("[ERR] Radio module not found!\n");
    delay(MESSAGE_TO_SLEEP_DELAY);
    screen_off();
    sleep_forever();
  }

  ttn_register(callback);
  ttn_join();
  ttn_sf(LORAWAN_SF);
  ttn_adr(LORAWAN_ADR);

}

void loop() {

  ttn_loop();
  screen_loop();

  // Send every SEND_INTERVAL millis
  static uint32_t last = 0;
  static bool first = true;
  if (0 == last || millis() - last > SEND_INTERVAL) {

    last = millis();
    first = false;
    Serial.println("TRANSMITTING");
    send();
    Serial.println("TRANSMITED");
  }

  if (millis() > SEND_INTERVAL) {
    sleep();
  }

}

uint8_t findDevices()
{

  uint8_t address[8];
  uint8_t count = 0;


  if (oneWire.search(address))
  {
    Serial.print("\nuint8_t pin");
    Serial.print(ONE_WIRE_BUS, DEC);
    Serial.println("[][8] = {");
    do {
      count++;
      Serial.println("  {");
      for (uint8_t i = 0; i < 8; i++)
      {
        Serial.print("0x");
        if (address[i] < 0x10) Serial.print("0");
        Serial.print(address[i], HEX);
        if (i < 7) Serial.print(", ");
      }
      Serial.println("  },");
    } while (oneWire.search(address));

    Serial.println("};");
    Serial.print("// nr devices found: ");
    Serial.println(count);
  }

  return count;
}
