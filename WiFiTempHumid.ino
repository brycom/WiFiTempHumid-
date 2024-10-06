#include <ArduinoHttpClient.h>

#include "WiFiS3.h"
#include <DHT.h>

#include "secrats.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

char serverIp[] = SECRET_IP;
int port = 8080;
WiFiClient wifi;
HttpClient httpClient = HttpClient(wifi, serverIp,port);


#define DHTTYPE DHT11
const int DHTPIN = 7;
const int tempLED = 8;  
const int humidityLED = 9;
const int WiFiLed = 2;
DHT dht(DHTPIN, DHTTYPE);

int status = WL_IDLE_STATUS;
WiFiServer server(80);

float temp = 0.0;
float humidity = 0.0;
int sykelNumber = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(tempLED, OUTPUT);
  pinMode(humidityLED, OUTPUT);
  pinMode(WiFiLed, OUTPUT);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }
  server.begin();
  printWifiStatus();
}

void loop() {
  digitalWrite(WiFiLed, HIGH);
  delay(300);
  digitalWrite(WiFiLed, LOW);
  delay(600);
  WiFiClient client = server.available();

  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();
  sykelNumber += 1;
  Serial.println(sykelNumber);

  if(sykelNumber > 10){
    temp = 0;
  }


  if ((newTemp - temp)> 0.2 || (newHumidity - humidity) > 0.9) {
    sykelNumber = 0;
    if (newTemp != temp) {
      digitalWrite(tempLED, HIGH);
      delay(300);
      digitalWrite(tempLED, LOW);
      delay(300);
      temp = newTemp;
      Serial.println("Här kommer ny temp!!");
      
    }
    if (newHumidity != humidity) {
      digitalWrite(humidityLED, HIGH);
      delay(300);
      digitalWrite(humidityLED, LOW);
      delay(300);
      humidity = newHumidity;
      Serial.println("Här kommer ny luftfuktighet!!");
    }
    Serial.print("temp: ");
    Serial.println(newTemp);
    Serial.print("Luftfuktighet: ");
    Serial.println(newHumidity);
    postTempAndHumidity(newTemp, newHumidity);
  } 
  if((newTemp - temp) < -0.2 || (newHumidity - humidity) < - 0.9){
    sykelNumber = 0;
        if (newTemp != temp) {
      digitalWrite(tempLED, HIGH);
      delay(300);
      digitalWrite(tempLED, LOW);
      delay(300);
      temp = newTemp;
      Serial.println("Här kommer ny temp!!");
      
    }
    if (newHumidity != humidity) {
      digitalWrite(humidityLED, HIGH);
      delay(300);
      digitalWrite(humidityLED, LOW);
      delay(300);
      humidity = newHumidity;
      Serial.println("Här kommer ny luftfuktighet!!");
    }
    Serial.print("temp: ");
    Serial.println(newTemp);
    Serial.print("Luftfuktighet: ");
    Serial.println(newHumidity);
    postTempAndHumidity(newTemp, newHumidity);
  }

  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();


    client.stop();
  }
  delay(10000);
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("ip adress: ");
  Serial.println(ip);
}

void postTempAndHumidity(float temp,float humid){
    String data = "{\"temperature\": ";
    data += String(temp, 2);
    data += ", \"humidity\": ";
    data += String(humid, 2);
    data += "}";

  httpClient.beginRequest();
  httpClient.post("/arduino/input", "application/json", data);
  httpClient.sendHeader("Content-Type", "application/json");
  httpClient.sendHeader("Content-Length", data.length());

  httpClient.beginBody();
  httpClient.print(data);

  httpClient.endRequest();

    int statusCode = httpClient.responseStatusCode();
    String response = httpClient.responseBody();
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
  
}
