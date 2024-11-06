# ESP32-Based Electronic Door Lock System with Blynk Integration

## Introduction
This project is an ESP32-based electronic door lock system that provides secure access control using a keypad, LCD display, relay module, and solenoid lock. The system integrates with the Blynk IoT platform, allowing for remote monitoring of lock status and alert notifications on unauthorized attempts. This system is ideal for home automation, office security, and other IoT applications.

[![Video Demonstration](https://github.com/gurkaran-singh1/ESP32-Soleniod-keypad-door-lock-system/blob/main/images/ESP32Proj.JPG)](https://seneca-my.sharepoint.com/:v:/g/personal/gurkaran-singh1_myseneca_ca/EdOMYshiMhtDp-pF4wCRwZAB-zZ4hN-iVrxcuteNEMH0LQ?e=qgxDds&nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D)

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
- **Half Breadboard** for connection purposes
- **2 9V Batteries** for powering soleniod lock
- **Power Supply Cable (Micro USB) and Jumper Wires** for connections

---

## Working Principle

### Solenoid Lock and Relay Operation
A solenoid lock is a device that uses electromagnetic energy to control the locking mechanism. When the relay connected to the solenoid lock is energized, it unlocks by allowing current to flow to the solenoid. In this project, the ESP32 triggers the relay based on password input, unlocking the solenoid for a set period.

The relay module acts as a switch between the ESP32 and the solenoid lock. It receives signals from the ESP32 to control the high-power circuit, enabling safe operation of the solenoid lock without exposing the ESP32 to high current loads.

Solenoid Lock includes two wires:
- Positive (+) wire (red): needs to be connected to power supply
- Negative (-) wire (black): needs to be connected to GND of DC power supply

---

## Code

You can download the .ino file in the Code folder as well

<details>
   <summary>📚 Click to reveal the code!</summary>
```
/*************************************************************

This is a simple demo of sending and receiving some data with Blynk IOT app
Be sure to check out other examples!
 *************************************************************/

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPLXXXX" // You will find this information while setting up Blynk IoT app
#define BLYNK_TEMPLATE_NAME "ESP32 Security Lock"
#define BLYNK_AUTH_TOKEN "xxx-xxxxx-xxxxx-xxxx"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// The libraries you will need for this project
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <HTTPClient.h>
#include <UrlEncode.h>


// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Gurkaran";
char pass[] = "xxxxxxx"; // WiFi Password
char auth[] = BLYNK_AUTH_TOKEN;

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "+1XXX-XXX-XXXX";
String apiKey = "87XXXX"; // You will get this key while setting up the WhatsApp API Bot

#define RELAY_PIN   19 // ESP32 pin GPIO19 connected to the relay
#define ROW_NUM     4  // keypad four rows
#define COLUMN_NUM  4  // keypad three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {12, 14, 27, 26};   // ESP32 pin: GPIO12, GPIO14, GPIO27, GPIO26 connected to the row pins
byte pin_column[COLUMN_NUM] = {25, 33, 32};  // ESP32 pin: GPIO25, GPIO33, GPIO32 connected to the column pins

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set LCD address and dimensions

const String correct_password = "2813"; // Change your password here
String input_password = "";
int incorrect_attempts = 0;
const int max_attempts = 3;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Initially lock the door
  lcd.setCursor(2,0);
  lcd.print(" WELCOME! ");
  lcd.setCursor(0, 1);
  lcd.print("ESP32 Project");
  delay(3000);
  lcd.clear();
  lcd.print("Enter Password");
  
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Blynk.begin(auth, ssid, pass, "blynk.cloud",80);  // This is connect the ESP32 to Blynk IoT app
 
}

void loop() {
  Blynk.run();
  char key = keypad.getKey();

  if (key != NO_KEY) {
    if (input_password.length() < 4) {
      input_password += key;
      lcd.setCursor(input_password.length() - 1, 1);
      lcd.print('*');  // Display '*' instead of the actual key pressed
    }

    if (input_password.length() == 4) {
      if (input_password == correct_password) {
        lcd.clear();
//        lcd.print("Success");
//        delay(2000);
        digitalWrite(RELAY_PIN, HIGH); // Unlock the door
        lcd.clear();
        lcd.print("Door Unlocked");
        delay(3000);
        digitalWrite(RELAY_PIN, LOW); // Relock the door
        lcd.clear();
        lcd.print("Enter Password");
        input_password = "";
        incorrect_attempts = 0;
      } else {
        lcd.clear();
        lcd.print("Wrong Password!");
        delay(2000);
        lcd.clear();
        lcd.print("Enter Password");
        input_password = "";
        incorrect_attempts++;
        if (incorrect_attempts == 3) {
          lcd.clear();
          lcd.print("WARNING STOP!");
          delay(2000);
          lcd.clear();
          lcd.print("Enter Password");
//          Blynk.logEvent("intrusion_alert", "Intrusion Detected!");
          sendMessage("ATTENTION!! SOMEONE IS TRYING TO GET IN!!");
        }
        if (incorrect_attempts > max_attempts) {
          lcd.clear();
          lcd.print("INTRUDER!!");
          Blynk.logEvent("intrusion_alert", "Intrusion Detected!"); // This event will be logged in Blynk dashboard.
//          sendMessage("ATTENTION!! SOMEONE IS TRYING TO GET IN!!");
        }
      }
    }
  }

}
void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

```
</details>

## Instructions

1. **Hardware Setup**:
   - Connect all the components - ESP32, Soleniod, LCD, Relay together. You can watch **Solenoid door lock system using 4x4 keypad** for reference (pasted the link in Refrences)
   - Ensure the relay is properly connected to the solenoid lock, and the ESP32's GPIO pin is set to control the relay.
   - You can use either a battery or power supply for powering the soleniod lock but make sure the lock has enough power to open and close smoothly.

2. **Software Setup**:
   - **Install Libraries**: Open the Arduino IDE software, I would recommend to use the [1.8.19v](https://downloads.arduino.cc/arduino-1.8.19-windows.zip) and install the following libraries:
      - LiquidCrystal_I2C.h
      - Keypad.h
      - WiFi.h
      - WiFiClient.h
      - BlynkSimpleEsp32.h
      - HTTPClient.h
      - UrlEncode.h
   - **Blynk IoT Platform**:
     - Create a Blynk project in the Blynk app or web interface, adding the ESP32 as a new device.
     - Get the `Template ID`, `Device Name`, and `Auth Token` from the Blynk platform.
     - Create virtual pins (e.g., V5 for door status and V0 for intrusion alert).
     - Check the link in references for more information. 

3. **Code Setup**:
   - **Update Credentials**: Open the code file and replace placeholders with your Wi-Fi SSID and password, and Blynk authentication token.
   - **Upload Code**: Connect your ESP32 to the computer via micro USB, select the correct board and COM port in the Arduino IDE, and upload the code.

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

---
## References

- [ESP32 - solenoid lock](https://esp32io.com/tutorials/esp32-solenoid-lock)
- [Create an ESP32 Door Lock System: Wokwi Beginner Tutorial](https://www.youtube.com/watch?v=ZLt_BqmmKKE)
- [Solenoid door lock system using 4x4 keypad](https://www.youtube.com/watch?v=lOoV563CqYA)
- [Blynk 2.0 Getting Started Tutorial, New Blynk App V2.0 with ESP32, Blynk.cloud setup](https://www.youtube.com/watch?v=EMSSJOblRcU)

## Contributing

Feel free to suggest improvements, report issues, or ask for help! You can reach me out on my email or LinkedIn. (Link in bio)
