#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include<Math.h>

#include <OneWire.h>
#include <DallasTemperature.h>   //ds18b20

#define FIREBASE_HOST "upravljanje-klimom-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "ECTG2a337UzpPvJxjZfTLr96q6aFClG3C2NUpgHy"
//#define WIFI_SSID "MojaTV_Full_228622"
//#define WIFI_PASSWORD "GIFTGTMCUNGIFTGTMCUN"
#define WIFI_SSID "H A R I S"
#define WIFI_PASSWORD "haris1234"

FirebaseData firebaseData;

// GPIO where the DS18B20 is connected to
const int oneWireBus = 2;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

//Pir Motion Sensor
int sensor = 13;  // Digital pin D7

int Status = 12; // Digital pin D6 

float optimalnaTemp;

int kontrola = 1;
int kontrola_prev = 0;
int pomocna;

int kontrolaLampice;

//int i = 0;

void setup() {
  // Start the Serial Monitor
  Serial.begin(9600);
  // Start the DS18B20 sensor
  sensors.begin();

  pinMode(sensor, INPUT);   // declare sensor as input
  pinMode(Status, OUTPUT);  // declare LED as output

  //pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(13, OUTPUT);
  
  Serial.setTimeout(2000);

  Serial.println("Communication started\n\n");
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  sensors.requestTemperatures();
  long state = digitalRead(sensor); 

  int temperatureC = sensors.getTempCByIndex(0);
  int temperatureF = sensors.getTempFByIndex(0);

 // Firebase.setFloat(firebaseData, "Temperatura/temperatureC", round(temperatureC * 100) / 100);
 // Firebase.setFloat(firebaseData, "Temperatura/temperatureF", round(temperatureF * 100) / 100);

  Firebase.setInt(firebaseData, "Temperatura/temperatureC", temperatureC);
  Firebase.setInt(firebaseData, "Temperatura/temperatureF", temperatureF);

  Firebase.getInt(firebaseData, "Temperatura/optimalna");
  optimalnaTemp = firebaseData.floatData();

  Serial.print("Trenutna temperatura: " + String(temperatureC) + " °C || ");
  Serial.print(String(temperatureF) + " °F");
  Serial.println(" ");
  Serial.println("Optimalna temperatura: " + String(optimalnaTemp) + " °C");
  Serial.println(" ");
  //delay(100);

  if(state == HIGH){
    digitalWrite(Status,HIGH);
    
    Serial.println("Motion detected!");
    Serial.println(" ");
    Serial.println("Izmjeri trenutnu temperaturu...");
    
    temperatureC = sensors.getTempCByIndex(0);
    temperatureF = sensors.getTempFByIndex(0);

    //Firebase.setFloat(firebaseData, "Temperatura/temperatureC", round(temperatureC * 100) / 100);
    //Firebase.setFloat(firebaseData, "Temperatura/temperatureF", round(temperatureF * 100) / 100);

    Firebase.setInt(firebaseData, "Temperatura/temperatureC", temperatureC);
    Firebase.setInt(firebaseData, "Temperatura/temperatureF", temperatureF);
    delay(1000);
    
    Firebase.setString(firebaseData, "PIR_Sensor/message", "Motion detected!");
    Firebase.setBool(firebaseData, "PIR_Sensor/IsDetected", true);
    
    if(temperatureC > optimalnaTemp){
      Serial.println("Trenutna temperatura veca od optimalne --> Ukljuci hladenje");
    
      Firebase.setInt(firebaseData, "Klima/kontrola", 2);
      Firebase.setInt(firebaseData, "Klima/komanda", 2);

      Firebase.getFloat(firebaseData, "Klima/kontrola");
      pomocna = firebaseData.intData();
      Serial.println("Kontrola hladenje: " + String(pomocna));
      
      delay(1000);
    }
    else if(temperatureC < optimalnaTemp){
      Serial.println("Trenutna temperatura manja od optimalne --> Ukljuci grijanje");

      Firebase.setInt(firebaseData, "Klima/kontrola", 3);
      Firebase.setInt(firebaseData, "Klima/komanda", 3);

      Firebase.getFloat(firebaseData, "Klima/kontrola");
      pomocna = firebaseData.intData();
      Serial.println("Kontrola grijanje: " + String(pomocna));
      
      delay(1000);     
    }
    else{
      Serial.println("Temperatura ista ugasi klimu!");
      Firebase.setInt(firebaseData, "Klima/kontrola", 1);
      Firebase.setInt(firebaseData, "Klima/komanda", 1);

      Firebase.getFloat(firebaseData, "Klima/kontrola");
      pomocna = firebaseData.intData();
      Serial.println("Kontrola ugasi: " + String(pomocna));
      
      delay(1000);
    }
  } else {
    digitalWrite(Status,LOW);
    Serial.println("No motion detected.");
    Firebase.setString(firebaseData, "PIR_Sensor/message", "No motion detected.");
    Firebase.setBool(firebaseData, "PIR_Sensor/IsDetected", false);
  }

  if(temperatureC == optimalnaTemp){
      //Serial.println("Trenuta Temperatura == Optimalna ---> Ugasi klimu!");
      Firebase.setInt(firebaseData, "Klima/kontrola", 1);
      Firebase.setInt(firebaseData, "Klima/komanda", 1);

      Firebase.getFloat(firebaseData, "Klima/kontrola");
      pomocna = firebaseData.intData();
      Serial.println("Kontrola ugasi: " + String(pomocna));
  } 

    if(Firebase.getInt(firebaseData, "Klima/komanda")){
    kontrola = firebaseData.intData();
    Serial.print("Vrijednost iz baze: ");
    Serial.println(kontrola);
    Serial.println("----------------------------------------------");
    
    if(kontrola_prev != kontrola)
    {
       kontrola_prev = kontrola;
       if(Firebase.setInt(firebaseData, "Klima/kontrola", kontrola)){
         delay(1000);
       }
       else {
          Serial.println(firebaseData.errorReason()); 
       }
    }
  }
  else {
    Serial.println(firebaseData.errorReason());  
  }

  Firebase.getInt(firebaseData, "Lampica/komanda");
  kontrolaLampice = firebaseData.intData();
  Serial.print("Kontrola lampice: ");
  Serial.println(kontrolaLampice);

  if(kontrolaLampice == 1){
   for(int i = 0; i < 5; i++){
      digitalWrite(Status, HIGH);
      delay(1000);
      digitalWrite(Status, LOW);
      delay(1000);
    }
  }else {
     digitalWrite(LED_BUILTIN, LOW);
  }

}
