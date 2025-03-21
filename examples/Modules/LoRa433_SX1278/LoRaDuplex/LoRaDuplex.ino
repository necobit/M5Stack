/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * @Hardwares: M5Core + Module LoRa433
 * @Platform Version: Arduino M5Stack Board Manager v2.1.3
 * @Dependent Library:
 * M5Stack@^0.4.6: https://github.com/m5stack/M5Stack
 * arduino-LoRa：https://github.com/sandeepmistry/arduino-LoRa
 */

#include <LoRa.h>
#include <M5Stack.h>

// LoRa Duplex communication
// Sends a message every half second, and polls continually
// for new incoming messages. Implements a one-byte addressing scheme,
// with 0xFF as the broadcast address.
// Uses readString() from Stream class to read payload. The Stream class'
// timeout may affect other functuons, like the radio's callback. For an
// created 28 April 2017
// by Tom Igoe

String outgoing;  // outgoing message

byte msgCount = 0;  // count of outgoing messages
// byte localAddress = 0xFF;     // address of this device
// byte destination = 0xBB;      // destination to send to

byte localAddress = 0xBB;  // address of this device
byte destination  = 0xFF;  // destination to send to

long lastSendTime = 0;     // last send time
int interval      = 2000;  // interval between sends

void header(const char *string, uint16_t color)
{
    M5.Lcd.fillScreen(color);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLUE);
    M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4);
}

void setup()
{
    M5.begin();
    M5.Power.begin();
    while (!Serial);
    header("LoRa Duplex", TFT_BLACK);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.drawString("Please use serial port to view data.", 0, 80, 2);

    Serial.println("LoRa Duplex");

    // override the default CS, reset, and IRQ pins (optional)
    LoRa.setPins();  // set CS, reset, IRQ pin

    if (!LoRa.begin(433E6)) {  // initialize ratio at 915 MHz
        Serial.println("LoRa init failed. Check your connections.");
        while (true);  // if failed, do nothing
    }

    Serial.println("LoRa init succeeded.");
}

void loop()
{
    if (millis() - lastSendTime > interval) {
        String message = "HeLoRa World!";  // send a message
        sendMessage(message);
        Serial.println("Sending " + message);
        lastSendTime = millis();             // timestamp the message
        interval     = random(2000) + 1000;  // 2-3 seconds
    }

    // parse for a packet, and call onReceive with the result:
    onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing)
{
    LoRa.beginPacket();             // start packet
    LoRa.write(destination);        // add destination address
    LoRa.write(localAddress);       // add sender address
    LoRa.write(msgCount);           // add message ID
    LoRa.write(outgoing.length());  // add payload length
    LoRa.print(outgoing);           // add payload
    LoRa.endPacket();               // finish packet and send it
    msgCount++;                     // increment message ID
}

void onReceive(int packetSize)
{
    if (packetSize == 0) return;  // if there's no packet, return

    // read packet header bytes:
    int recipient       = LoRa.read();  // recipient address
    byte sender         = LoRa.read();  // sender address
    byte incomingMsgId  = LoRa.read();  // incoming msg ID
    byte incomingLength = LoRa.read();  // incoming msg length

    String incoming = "";

    while (LoRa.available()) {
        incoming += (char)LoRa.read();
    }

    if (incomingLength != incoming.length()) {  // check length for error
        Serial.println("error: message length does not match length");
        return;  // skip rest of function
    }

    // if the recipient isn't this device or broadcast,
    if (recipient != localAddress && recipient != 0xFF) {
        Serial.println("This message is not for me.");
        return;  // skip rest of function
    }

    // if message is for this device, or broadcast, print details:
    Serial.println("Received from: 0x" + String(sender, HEX));
    Serial.println("Sent to: 0x" + String(recipient, HEX));
    Serial.println("Message ID: " + String(incomingMsgId));
    Serial.println("Message length: " + String(incomingLength));
    Serial.println("Message: " + incoming);
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    Serial.println("Snr: " + String(LoRa.packetSnr()));
    Serial.println();
}
