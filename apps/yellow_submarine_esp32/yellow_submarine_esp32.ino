// Wifi-Connection
#include <WiFi.h>
// UDP-Protocoll
#include <EthernetUdp.h>

// Wifi
const char* ssid = "Jenke_Schloss";
const char* password = "holladi4";
const char* hostname = "YellowSubmarine";

// UDP
unsigned int localPort = 1234;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
WiFiUDP Udp;

// PWM properties (required with ESP32)
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

// Speed motor
const int speedMotorPin1 = 26;
const int speedMotorPin2 = 27;
const int speedMotorEnablePin = 14;

/**
 * Setup: Run once
 */
void setup() {
  // Serial connection
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting Yellow Submarine");
  Serial.println();

  // Wifi-Connection
  connectToNetwork();
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  
  // UDP-Listener
  Udp.begin(localPort);
  Serial.println("UDP running");

  // PWM settings
  ledcSetup(pwmChannel, freq, resolution);

  // Speed motor
  pinMode (speedMotorPin1, OUTPUT);
  pinMode (speedMotorPin2, OUTPUT);
  pinMode (speedMotorEnablePin, OUTPUT);
  ledcAttachPin(speedMotorEnablePin, pwmChannel);
  digitalWrite(speedMotorPin1, LOW);
  digitalWrite(speedMotorPin2, LOW);
}

/** 
 * Application loop
 */
void loop() {
  checkUDP();
}

/**
 * Connect to the nework specified by SSID and password
 */
void connectToNetwork() {
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");
}

/** 
 * Check if UDP package has arrived, if yes - handle it 
 */
void checkUDP() {  
  int packetSize = Udp.parsePacket();
  if(Udp.available())
  {
    //Serial.print("Received packet of size ");
    //Serial.println(packetSize);
    //Serial.print("From ");

    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);

    // Extract the content via the protocoll
    int speed = ubyte2int(packetBuffer[0]);
    int sideRudder = ubyte2int(packetBuffer[1]);
    int pitchElevator = ubyte2int(packetBuffer[2]);
    int pressureTankOperation = ubyte2int(packetBuffer[3]);
    
    // Debug output of the parameters
    Serial.print("speed: ");
    Serial.println(speed);
    Serial.print("side rudder: ");
    Serial.println(sideRudder);
    Serial.print("pitch elevator: ");
    Serial.println(pitchElevator);
    Serial.print("tank operation: ");
    Serial.println(pressureTankOperation);
    setSpeed(speed);
  }
  delay(10);
}

/**
 * Convert unsigned byte into int value
 */
int ubyte2int(char data){
  return (int)data < 128 ? (int)data : -(256 - (int)data);
}

/** 
 * Translate WIfi encryption constants 
 */
String translateEncryptionType(wifi_auth_mode_t encryptionType) { 
  switch (encryptionType) {
    case (WIFI_AUTH_OPEN):
      return "Open";
    case (WIFI_AUTH_WEP):
      return "WEP";
    case (WIFI_AUTH_WPA_PSK):
      return "WPA_PSK";
    case (WIFI_AUTH_WPA2_PSK):
      return "WPA2_PSK";
    case (WIFI_AUTH_WPA_WPA2_PSK):
      return "WPA_WPA2_PSK";
    case (WIFI_AUTH_WPA2_ENTERPRISE):
      return "WPA2_ENTERPRISE";
  }
}

/**
 * Set speed motor according to speed. speed must be in [0,100]
 */
void setSpeed(int speed){
  if ( abs(speed) < 10  ){
    // Stop
    digitalWrite(speedMotorPin1, LOW);
    digitalWrite(speedMotorPin2, LOW);  
    return;
  }
  if ( speed > 0 ){
    // Forwards
    digitalWrite(speedMotorPin1, LOW);
    digitalWrite(speedMotorPin2, HIGH);  
  } else {
    // Backwards
    digitalWrite(speedMotorPin1, HIGH);
    digitalWrite(speedMotorPin2, LOW);    
  }
  int pwm = max(0,min(255,(int)(speed*2.55)));
  ledcWrite(pwmChannel, pwm);
}
