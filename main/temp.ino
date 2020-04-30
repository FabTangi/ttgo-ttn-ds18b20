/*

  Temp module

  Copyright (C) 2020 by TL

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

#include <OneWire.h>
#include <DallasTemperature.h>

uint32_t Temp;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

//Pass our OneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void temp_setup() {
    // Start the DS18B20 sensor
    sensors.begin();
}


static void temp_loop() {       
   sensors.requestTemperatures(); 
   temp=sensors.getTempCByIndex(0)*100;
   Serial.println(temp);
   // Battery Voltage
   vbat = (float)(analogRead(VBAT_PIN)) / 4095*2*3.3*1.1;
  /*
  The ADC value is a 12-bit number, so the maximum value is 4095 (counting from 0).
  To convert the ADC integer value to a real voltage you’ll need to divide it by the maximum value of 4095,
  then double it (note above that Adafruit halves the voltage), then multiply that by the reference voltage of the ESP32 which 
  is 3.3V and then vinally, multiply that again by the ADC Reference Voltage of 1100mV.
  */
  
   
}

  #if defined(PAYLOAD_USE_FULL)
    void buildPacket(uint8_t txBuffer[16])
    {

   sensors.requestTemperatures(); 
   temp=sensors.getTempCByIndex(0)*100;
   Serial.println(temp);

   // Battery Voltage
   vbat = (float)(analogRead(VBAT_PIN)) / 4095*2*3.3*1.1;
   Serial.println("Vbat = "); Serial.println(vbat); Serial.println(" Volts");

   txBuffer[0] = (int)temp >> 8;
   txBuffer[1] = (int)temp & 0xFF;
   txBuffer[2] = (analogRead(ONE_WIRE_BUS)/15);
   txBuffer[3] = highByte(int(round(vbat*100)));
   txBuffer[4] = lowByte(int(round(vbat*100)));
   txBuffer[5] = highByte(bootCount);
   txBuffer[6] = lowByte(bootCount);

        
    }

    #elif defined(PAYLOAD_USE_CAYENNE)

    void buildPacket(uint8_t txBuffer[16])
    {

   sensors.requestTemperatures(); 
   temp=sensors.getTempCByIndex(0)*100;
   Serial.println(temp);

   // Battery Voltage
   vbat = (float)(analogRead(VBAT_PIN)) / 4095*2*3.3*1.1;
   Serial.println("Vbat = "); Serial.println(vbat); Serial.println(" Volts");



   txBuffer[2] = (int)temp/10 >> 8;
   txBuffer[3] = (int)temp/10 & 0xFF;
   //txBuffer[4] = (analogRead(ONE_WIRE_BUS)/15);
   txBuffer[6] = highByte(int(round(vbat*100)));
   txBuffer[7] = lowByte(int(round(vbat*100)));
   //txBuffer[7] = highByte(bootCount);
   //txBuffer[8] = lowByte(bootCount);

       
    }
    #endif
