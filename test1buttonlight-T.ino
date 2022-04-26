// LoRa 9x_TX

#include <SPI.h>
#include <RH_RF95.h>

byte buttonState1 = 0;     // current state of the button
byte lastButtonState1 = 0; // previous state of the button
byte buttonState2 = 0;     // current state of the button
byte lastButtonState2 = 0; // previous state of the button

#define RFM95_CS 4
#define RFM95_RST 2
#define RFM95_INT 3
#define btn1 10
#define btn2 9

// Change to 915.0 or other frequency, must match RX's freq!
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

  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);

  Wire.begin();
  Wire.setClock(400000L);

  Serial.begin(9600);
  delay(100);


  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  rf95.setTxPower(23, false);
}


void loop() {  
  buttonState1 = digitalRead(btn1);
  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == HIGH){
      sendRadio();
      delay(10);
    }
  } else {
    
  }
  lastButtonState1 = buttonState1;

  buttonState2 = digitalRead(btn2);
  if (buttonState2 != lastButtonState2) {
    if (buttonState2 == HIGH){
      sendRadio();
      delay(100);
    }
  } else {
    
  }
  lastButtonState2 = buttonState2;
}

void sendRadio() {
  Serial.println("Sending to rf95_server");
  if(buttonState1 == HIGH) {
    char radiopacket[20] = "Hi friend!";
    Serial.print("Sending "); Serial.println(radiopacket);
    radiopacket[19] = 0;
  
    Serial.println("Sending..."); delay(10);
    rf95.send((uint8_t *)radiopacket, 20);
  } else if (buttonState2 == HIGH) {
    char radiopacket[20] = "where are you?";
    Serial.print("Sending "); Serial.println(radiopacket);
    radiopacket[19] = 0;
  
    Serial.println("Sending..."); delay(10);
    rf95.send((uint8_t *)radiopacket, 20);
  }
    Serial.println("Waiting for packet to complete..."); delay(10);
    rf95.waitPacketSent();
  
    Serial.println("Waiting for reply..."); delay(10);
    if (rf95.waitAvailableTimeout(2000))
    { 
      // Should be a reply message for us now   
      if (rf95.recv(buf, &len))
     {       
        Serial.print("Got reply: ");
        Serial.println((char*)buf);
        msgOnScreen();
        digitalWrite(LED, HIGH);
        Serial.print("RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);    
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
    else
    {
      Serial.println("No reply, is there a listener around?");
    }
    delay(100);
    digitalWrite(LED, LOW);
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
