#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define DHTPIN D7 // digital pin dht11
#define DHTTYPE DHT11 // inisialisasi sensor dht11

//MQTT Topic
#define MQTT_TEMP "temperature"
#define MQTT_HUMD "humidity"

const char* ssid = "PKL ATMDUINO"; 
const char* password = "wkwkwk"; 
const char* mqtt_server = "broker.mqtt-dashboard.com";
unsigned long startMillis=0; //variabel menyimpan milidetik terakhir dari loop

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void DHT_config(){
 float tempRead = dht.readTemperature();
 float humdRead = dht.readHumidity();
 if (isnan(tempRead) || isnan(humdRead))
 {
  client.publish(MQTT_TEMP, "0");
  client.publish(MQTT_HUMD, "0");
  return;
 }
 client.publish(MQTT_TEMP, String(tempRead).c_str());
 client.publish(MQTT_HUMD, String(humdRead).c_str());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "belajarmqtt";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) 
    {
      Serial.println("connected");
      client.publish("temperature",  "Online");
      client.subscribe("humidity");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup(){
  Serial.begin(9600);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop(){
  unsigned long currentMillis = millis(); // variabel mengambil waktu yang berjalan dan menyimpan di variabel milis_sekarang
  if (currentMillis - startMillis >= 2000) { //setiap milis_sekarang - hitungan_milis yang mencapai nilai lebih besar atau sama dengan 2000
  
    float tempRead = dht.readTemperature();
    float humdRead = dht.readHumidity();
  
    DHT_config();
    if (!client.connected()){
      reconnect();
    }
    client.loop();
    Serial.print(F("Humidity: "));
    Serial.print(humdRead);
    Serial.println(F("°C "));
    Serial.print(F("  Temperature: "));
    Serial.print(tempRead);
    Serial.println(F("°C "));
  }
}
