// Wifi-Connection
#include <WiFi.h>
// UDP-Protocoll
#include <EthernetUdp.h>

// Wifi
const char* ssid = "Jenke_Schloss";
const char* password = "holladi4";

// UDP
unsigned int localPort = 1234;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
WiFiUDP Udp;

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
}

/** 
 * Application loop
 */
void loop() {
  checkUDP();
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
    int pitchElevator = (int)packetBuffer[2] < 128 ? (int)packetBuffer[2] : -(256 - (int)packetBuffer[2]);
    int pressureTankOperation = (int)packetBuffer[3];
    
    // Debug output of the parameters
    Serial.print("speed: ");
    Serial.println(speed);
    Serial.print("side rudder: ");
    Serial.println(sideRudder);
    Serial.print("pitch elevator: ");
    Serial.println(pitchElevator);
    Serial.print("tank operation: ");
    Serial.println(pressureTankOperation);
  }
  delay(10);
}

/**
 * Convert unsigned byte into int value
 */
int ubyte2int(char data){
  return (int)packetBuffer[1] < 128 ? (int)packetBuffer[1] : -(256 - (int)packetBuffer[1]);
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
 * Connect to the nework specified by SSID and password
 */
void connectToNetwork() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to network");
}
