#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char* ssid = "ROBOTICA";  // Enter your WiFi name
const char* password = "iroboticaufal";   // Enter WiFi password

const char* broker_url = "10.0.0.109";
const int broker_port = 1883;

// Topics
const char* velocityTopic = "conveyor/vel";
const char* stateTopic = "conveyor/state";
const char* orientationTopic = "conveyor/orientation";

// Wifi and Broker configuration
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
bool flag = 0;

// Motor A
int motor1Pin1 = 16; 
int motor1Pin2 = 17; 
int enable1Pin = 5; 

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 255;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char tmp_msg[length + 1];  // +1 for the null terminator
  for (int i = 0; i < length; i++) {
    tmp_msg[i] = (char)payload[i];
  }
  tmp_msg[length] = '\0';  // Null-terminate the string

  if (strcmp(topic, stateTopic) == 0) {
    if (strcmp(tmp_msg, "1") == 0) {
      client.publish("conveyor/feedback", "Turning on the conveyor");
      digitalWrite(motor1Pin1, HIGH);
      digitalWrite(motor1Pin2, LOW);

      

    } else if (strcmp(tmp_msg, "0") == 0) {
      client.publish("conveyor/feedback", "Turning off the conveyor");
      digitalWrite(motor1Pin1, LOW);
      digitalWrite(motor1Pin2, LOW); 
    } 
  } 

  Serial.print(tmp_msg);
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(velocityTopic);
      client.subscribe(stateTopic);
      client.subscribe(orientationTopic);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  ledcWrite(pwmChannel, dutyCycle); 

  Serial.begin(9600);
  setup_wifi();
  client.setServer(broker_url, broker_port);
  client.setCallback(callback);

  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW); 
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  // // Move the DC motor forward at maximum speed
  // Serial.println("Moving Forward");
  // digitalWrite(motor1Pin1, LOW);
  // digitalWrite(motor1Pin2, HIGH); 
  // delay(2000);

  // // Stop the DC motor
  // Serial.println("Motor stopped");
  // digitalWrite(motor1Pin1, LOW);
  // digitalWrite(motor1Pin2, LOW);
  // delay(1000);

  // // Move DC motor backwards at maximum speed
  // Serial.println("Moving Backwards");
  // digitalWrite(motor1Pin1, HIGH);
  // digitalWrite(motor1Pin2, LOW); 
  // delay(2000);

  // // Stop the DC motor
  // Serial.println("Motor stopped");
  // digitalWrite(motor1Pin1, LOW);
  // digitalWrite(motor1Pin2, LOW);
  // delay(1000);

  // // Move DC motor forward with increasing speed
  // digitalWrite(motor1Pin1, HIGH);
  // digitalWrite(motor1Pin2, LOW);
  // while (dutyCycle <= 255){
  //   ledcWrite(pwmChannel, dutyCycle);   
  //   Serial.print("Forward with duty cycle: ");
  //   Serial.println(dutyCycle);
  //   dutyCycle = dutyCycle + 5;
  //   delay(500);
  // }
  // dutyCycle = 200;
}