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
