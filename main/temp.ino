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




void temp_setup() {
  // Start the DS18B20 sensor
  sensors.begin();
  delay(500);
  while (deviceCount == 0)
  {
    delay(500);
  //  deviceCount = sensors.getDeviceCount();
    deviceCount =findDevices();
    Serial.println(deviceCount + "devices found");
    Serial.println(deviceCount);
    sensors.requestTemperatures();
    Serial.println("Temperature 0: ");
    Serial.println(sensors.getTempCByIndex(0));
    Serial.println("Temperature 1 : ");
    Serial.println(sensors.getTempCByIndex(1));
  }
}


static void temp_loop() {
  Serial.println("temp loop");
  sensors.requestTemperatures();

  for (int i = 0;  i < deviceCount;  i++)
  {
    temp = sensors.getTempCByIndex(i) * 100;
    Serial.println(i);
    Serial.println(temp);
  }
  // Battery Voltage
  vbat = (float)(analogRead(VBAT_PIN)) / 4095 * 2 * 3.3 * 1.1;
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
  temp = sensors.getTempCByIndex(0) * 100;
  Serial.println(temp);

  // Battery Voltage
  vbat = (float)(analogRead(VBAT_PIN)) / 4095 * 2 * 3.3 * 1.1;
  Serial.println("Vbat = "); Serial.println(vbat); Serial.println(" Volts");

  txBuffer[0] = (int)temp >> 8;
  txBuffer[1] = (int)temp & 0xFF;
  txBuffer[2] = (analogRead(ONE_WIRE_BUS) / 15);
  txBuffer[3] = highByte(int(round(vbat * 100)));
  txBuffer[4] = lowByte(int(round(vbat * 100)));
  txBuffer[5] = highByte(bootCount);
  txBuffer[6] = lowByte(bootCount);


}

#elif defined(PAYLOAD_USE_CAYENNE)


#endif
