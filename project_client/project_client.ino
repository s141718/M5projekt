#include <Ds1302.h>
#include "WiFiS3.h"

char ssid[] = "FreshTomato24";
char pass[] = "stefan141718";
char server[] = "192.168.1.42";

Ds1302 rtc(5, 7, 6); // Initialize with pins: RST, CLK, DAT

int sensor = 13; // PIR sensor connected to digital pin 13
int ledPin = 12; // LED connected to digital pin 12

WiFiClient client;

void setup() {
  rtc.init(); // Initialize chip
  
  // Set date and time using the DateTime struct
  //Ds1302::DateTime currentTime = {24, 12, 11, 9, 54, 0, 3}; // YY, MM, DD, HH, MM, SS, DOW
  //rtc.setDateTime(&currentTime);

  pinMode(sensor, INPUT); // Set the PIR sensor as input
  pinMode(ledPin, OUTPUT); // Set the LED as output

  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

 // attempt to connect to WiFi network:
  int status = WL_IDLE_STATUS; 
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
     
    // wait 10 seconds for connection:
    delay(10000);
  }
}

void sendDateTime(const String& datetimeStr) {
  String encodedDatetime = datetimeStr;
  encodedDatetime.replace(" ", "%20"); // Encode spaces as %20
  String url = String("/report/") + encodedDatetime;
  if (client.connect(server, 5000)) { // Ensure the port matches your Flask server
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(server));
    client.println();
    client.stop(); // Close connection
  } else {
    Serial.println("Connection to server failed.");
  }
}

void loop() {
  int val = digitalRead(sensor); // Read the PIR sensor
  if (val == HIGH) {
    digitalWrite(ledPin, HIGH); // Turn the LED on
    Ds1302::DateTime currentTime;
    rtc.getDateTime(&currentTime);
    char datetimeBuffer[20]; // Buffer to hold the datetime
    sprintf(datetimeBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
      currentTime.year + 2000,
      currentTime.month,
      currentTime.day, 
      currentTime.hour, 
      currentTime.minute, 
      currentTime.second);
    String datetimeStr = String(datetimeBuffer);
    Serial.print("Motion detected at: ");
    Serial.println(datetimeStr);

    // Send datetime to Flask server
    sendDateTime(datetimeBuffer);

    } else {
      digitalWrite(ledPin, LOW); // Turn the LED off
  }
  delay(500); // Wait for 0.5 second before checking again
}