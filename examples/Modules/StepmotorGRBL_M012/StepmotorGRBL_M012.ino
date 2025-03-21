/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * @Hardwares: M5Core + Module Stepmotor
 * @Platform Version: Arduino M5Stack Board Manager v2.1.3
 * @Dependent Library:
 * M5Stack@^0.4.6: https://github.com/m5stack/M5Stack
 */

#include <M5Stack.h>
#include <Wire.h>

// STEP-MOTOR Module TEST Example,If Button A was pressed, stepmotor will rotate
// back and forth at a time.
// 步进电机模块测试示例，如果按下按钮A，步进电机将一次来回旋转。

/*
 * The I2C address of StepMotor Module is 0x70 by default.
 * StepMotor 模块的 I2C 地址默认为 0x70。
 * But if you change this I2C address through burning this firmware
 * 但是如果你可以通过烧这个固件来改变这个I2C地址
 * (https://github.com/m5stack/stepmotor_module/blob/master/Firmware%20for%20stepmotor%20module/GRBL-Arduino-Library/examples/GRBL_I2C/GRBL_I2C_0x71.hex),
 * After you need to use I2C address `0x71` for correct communication.
 * 此后您需要使用 I2C 地址“0x71”才能正确通信。
 */

#define STEPMOTOR_I2C_ADDR (0x70)
// #define STEPMOTOR_I2C_ADDR (0x71)

void setup()
{
    M5.begin();
    M5.Power.begin();
    Wire.begin();
    m5.Lcd.setTextColor(WHITE, BLACK);
    m5.Lcd.setTextSize(2);
    m5.lcd.setBrightness(100);
    M5.Lcd.setCursor(4, 10);
    M5.Lcd.println("StepMotor Test: 0x70");
    M5.Lcd.setCursor(4, 30);
    M5.Lcd.println("Press A: 0x70");
}

void SendByte(byte addr, byte b)
{
    Wire.beginTransmission(addr);
    Wire.write(b);
    Wire.endTransmission();
}

void SendCommand(byte addr, char *c)
{
    Wire.beginTransmission(addr);
    while ((*c) != 0) {
        Wire.write(*c);
        c++;
    }
    Wire.write(0x0d);
    Wire.write(0x0a);
    Wire.endTransmission();
}

void loop()
{
    /*If Button A was pressed,stepmotor will rotate back and forth at a time.\\
      如果按下按钮A，步进电机将一次来回旋转*/
    if (digitalRead(39) == LOW)  // A button
    {
        while (digitalRead(39) == LOW) delay(1);
        SendCommand(STEPMOTOR_I2C_ADDR, "G1 X20Y20Z20 F500");
        SendCommand(STEPMOTOR_I2C_ADDR, "G1 X0Y0Z0 F400");
    }
    if (digitalRead(37) == LOW)  // C button
    {
        while (1) {
            SendCommand(STEPMOTOR_I2C_ADDR, "G1 X0Y0Z0 F500");
            delay(1000);
            M5.Lcd.print(".");
            delay(1000);
            SendCommand(STEPMOTOR_I2C_ADDR, "G1 X5Y5Z5 F500");
            delay(1000);
            M5.Lcd.print(".");
            delay(1000);
        }
    }
    // Get Data from Module.  从模块获取数据。
    Wire.requestFrom(STEPMOTOR_I2C_ADDR, 1);
    if (Wire.available() > 0) {
        int u = Wire.read();
        if (u != 0) Serial.write(u);
    }
    delay(1);
    // Send Data to Module.  向模块发送数据
    while (Serial.available() > 0) {
        int inByte = Serial.read();
        SendByte(STEPMOTOR_I2C_ADDR, inByte);
    }
}
