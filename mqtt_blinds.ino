#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Stepper.h>

// Network and MQTT info
const char* ssid = "SSID_HERE"; //Wifi ssid 
const char* password = "PASSWORD_HERE";//Wifi password
const char* mqtt_server = "10.0.1.1"; //MQTT server IP
const char* clientId = "bedroom_blinds"; //MQTT topic
const char* userName = "MQTT_USER"; //MQTT user
const char* passWord = "MQTT_PASS"; //MQTT pass

// Number of steps per revolution
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution

// initialize the stepper library on D1,D2,D5,D6
Stepper myStepper(stepsPerRevolution, D1, D2, D5, D6);
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Command from MQTT broker is : [");
  Serial.print(topic);
  int p =(char)payload[0]-'0';
  int stepperLoop = 0;
  // step one revolution  in one direction:
  if(p==1) 
  {
    myStepper.step(stepsPerRevolution);
    Serial.print("  clockwise" );
  }
  // step one revolution in the other direction:
  else if(p==2)
  {
    myStepper.step(-stepsPerRevolution);
    Serial.print("  counterclockwise" );
  }
  else if(p==3)
  {
    Serial.print("  clockwise loop: " );
    for (stepperLoop = 0; stepperLoop < 40; stepperLoop++) {
      myStepper.step(+stepsPerRevolution);
      delay(10);
    }
  }
  else if(p==4)
  {
    Serial.print("  counterclockwise loop: " );
    for (stepperLoop = 0; stepperLoop < 40; stepperLoop++) {
      myStepper.step(-stepsPerRevolution);
      delay(10);
    }
  }
  else if(p == 0)
  {
    digitalWrite (D1, LOW);
    digitalWrite (D2, LOW);
    digitalWrite (D5, LOW);
    digitalWrite (D6, LOW);
    Serial.print("  off" );

  }
   Serial.println();
}
 
//  Serial.println();
 //end callback

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
        // if (client.connect(clientId.c_str()))
    //if your MQTT broker has clientID,username and password
    if (client.connect(clientId.c_str(), userName, passWord))
    {
      Serial.println("connected");
     //once connected to MQTT broker, subscribe command if any
      client.subscribe("blindsCommand");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} //end reconnect()

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // set the speed at 80 rpm:
  myStepper.setSpeed(80);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
