# ESP32-Based Electronic Door Lock System with Blynk Integration

## Introduction
This project is an ESP32-based electronic door lock system that provides secure access control using a keypad, LCD display, relay module, and solenoid lock. The system integrates with the Blynk IoT platform, allowing for remote monitoring of lock status and alert notifications on unauthorized attempts. This system is ideal for home automation, office security, and other IoT applications.

## Table of Contents
1. [Hardware Used](#hardware-used)
2. [Working Principle](#working-principle)
   - [Solenoid Lock and Relay Operation](#solenoid-lock-and-relay-operation)
3. [Code](#code)
4. [Instructions](#instructions)
5. [Uses and Functionality](#uses-and-functionality)

---

## Hardware Used
- **ESP32 DOIT DevKit V1**
- **4x4 Keypad** for password entry
- **16x2 I2C LCD Display** for real-time feedback
- **Relay Module** to control the solenoid lock
- **Solenoid Lock** for locking/unlocking mechanism
- **Blynk IoT Platform** for remote monitoring and alerts
- **Power Supply and Jumper Wires** for connections

---

## Working Principle

### Solenoid Lock and Relay Operation
A solenoid lock is a device that uses electromagnetic energy to control the locking mechanism. When the relay connected to the solenoid lock is energized, it unlocks by allowing current to flow to the solenoid. In this project, the ESP32 triggers the relay based on password input, unlocking the solenoid for a set period.

The relay module acts as a switch between the ESP32 and the solenoid lock. It receives signals from the ESP32 to control the high-power circuit, enabling safe operation of the solenoid lock without exposing the ESP32 to high current loads.

---

## Code
```cpp
// Full code integrating LCD, Keypad, Blynk, and relay module for the door lock system
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Define Blynk credentials
#define BLYNK_TEMPLATE_ID "YourTemplateID"
#define BLYNK_TEMPLATE_NAME "ESP32 Security Lock"
#define BLYNK_AUTH_TOKEN "YourAuthToken"

// WiFi credentials
char ssid[] = "YourSSID";
char pass[] = "YourPassword";

// Pin Definitions
#define RELAY_PIN   19
#define ROW_NUM     4
#define COLUMN_NUM  4

// Keypad and LCD Configuration
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte pin_rows[ROW_NUM] = {12, 14, 27, 26};
byte pin_column[COLUMN_NUM] = {25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const String correct_password = "1234";
String input_password = "";
int incorrect_attempts = 0;
const int max_attempts = 3;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  lcd.setCursor(2, 0);
  lcd.print("WELCOME!");
  lcd.setCursor(0, 1);
  lcd.print("Electronic Lock");
  delay(3000);
  lcd.clear();
  lcd.print("Enter Password");
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    if (input_password.length() < 4) {
      input_password += key;
      lcd.setCursor(input_password.length() - 1, 1);
      lcd.print('*');
    }
    if (input_password.length() == 4) {
      if (input_password == correct_password) {
        lcd.clear();
        lcd.print("Access Granted");
        digitalWrite(RELAY_PIN, LOW);
        delay(5000);
        digitalWrite(RELAY_PIN, HIGH);
        lcd.clear();
        lcd.print("Enter Password");
        input_password = "";
        incorrect_attempts = 0;
        Blynk.virtualWrite(V5, "Door Locked");
      } else {
        lcd.clear();
        lcd.print("Access Denied");
        delay(2000);
        lcd.clear();
        lcd.print("Enter Password");
        input_password = "";
        incorrect_attempts++;
        if (incorrect_attempts >= max_attempts) {
          Blynk.event("intrusion_alert", "Intrusion Detected!");
        }
      }
    }
  }
  Blynk.run();
}

## Instructions

1. **Hardware Setup**:
   - Connect the ESP32 to the relay, LCD, and keypad as shown in the circuit diagram.
   - Ensure the relay is properly connected to the solenoid lock, and the ESP32's GPIO pin is set to control the relay.

2. **Software Setup**:
   - **Install Libraries**: Open the Arduino IDE and install the `Keypad`, `LiquidCrystal_I2C`, `WiFi`, and `Blynk` libraries.
   - **Blynk IoT Platform**:
     - Create a Blynk project in the Blynk app or web interface, adding the ESP32 as a new device.
     - Get the `Template ID`, `Device Name`, and `Auth Token` from the Blynk platform.
     - Create virtual pins (e.g., V5 for door status and V0 for intrusion alert).

3. **Code Setup**:
   - **Update Credentials**: Open the code file and replace placeholders with your Wi-Fi SSID and password, and Blynk authentication token.
   - **Upload Code**: Connect your ESP32 to the computer via USB, select the correct board and COM port in the Arduino IDE, and upload the code.

4. **Running the System**:
   - **Power Up**: Once the code is uploaded, disconnect and reconnect the ESP32 to power on the system.
   - **Enter Password**: On the LCD display, a prompt will ask for the password. Use the keypad to enter the password.
   - **Password Verification**:
     - If the correct password is entered, the door unlocks momentarily, displaying "Access Granted" on the LCD and updating the Blynk app.
     - If an incorrect password is entered three times, the system triggers an "Intrusion Detected" alert in the Blynk app.

---

## Uses and Functionality

- **Access Control**: The system provides a secure, password-protected access mechanism, ideal for homes and offices requiring controlled entry points.
- **Remote Monitoring**: Integration with the Blynk platform allows users to monitor the door lock status remotely and receive real-time notifications of intrusion attempts.
- **Security Alerts**: If multiple incorrect password attempts are detected, the system automatically sends an alert notification through Blynk, enhancing security by informing users of potential unauthorized access.
