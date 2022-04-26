 。// Arduino9x_RX

//// LoRa ////
#include <SPI.h>
#include <RH_RF95.h>
#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

  // receieve
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

//// OLED /////

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define I2C_ADDRESS 0x3C
#define RST_PIN -1

SSD1306AsciiWire oled;

// Blinky on receipt
#define LED 5

void setup() {
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  Wire.begin();
  Wire.setClock(400000L);

  Serial.begin(9600);
  delay(100);
  Serial.println("Arduino LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);

}

void loop()
{
  if (rf95.available())
  {
    
    if (rf95.recv(buf, &len)) {
      digitalWrite(LED, HIGH);
//      RH_RF95::printBuffer("Received: ", buf, len);
//      Serial.println(buf);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      
      msgOnScreen();
      msgResponse();
//      delay(10);
      digitalWrite(LED, LOW);
    } else {
      Serial.println("Receive failed");
    }
  }
}

void msgOnScreen() {
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(Adafruit5x7);

  uint32_t m = micros();
  oled.clear();
  oled.set1X();
  oled.print((char*)buf);
}

void msgResponse() {
        // Send a reply
//      if ((char*)buf = "Hi friend!") {
        uint8_t data[] = "hello you!";
//      } else {
//        uint8_t data[] = "what do you want";
//      }
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
}
