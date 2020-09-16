// Wifi-Connection
#include <WiFi.h>
// UDP-Protocoll
#include <EthernetUdp.h>
// Servo (Bibliothek: ESP32Servo)
#include <ESP32Servo.h>
// L298N (https://robojax.com/learn/arduino/?vid=robojax_ESP32_DC_motor_L298N_WiFi)
#include <Robojax_L298N_DC_motor.h>

// General
#define DEBUG false

// Wifi
const char* ssid = "Jenke_Schloss";
const char* password = "";
const char* hostname = "YellowSubmarine";

// UDP
unsigned int localPort = 1234;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
WiFiUDP Udp;

// Speed motor
#define motorDriverPin1 26
#define motorDriverPin2 27
#define motorDriverEnablePin 14
#define pwmChannelmotorDriver 2
const int CCW = 2; 
const int CW  = 1;
const int MOTOR_SPEED = 1;

// Tank motor
#define tankMotorPin1 25
#define tankMotorPin2 33
#define tankMotorEnablePin 32
#define pwmChannelTankMotor 3
const int MOTOR_TANK = 2;
Robojax_L298N_DC_motor motorDriver(
  motorDriverPin1, motorDriverPin2, motorDriverEnablePin, pwmChannelmotorDriver, 
  tankMotorPin1, tankMotorPin2, tankMotorEnablePin, pwmChannelTankMotor, 
  DEBUG ); 

// Side rudder
const int PWM_FREQ_SERVO = 50;
const int sideRudderPin = 12;
Servo sideRudderServo;

// Pitch elevator
const int pitchElevatorPin = 13;
Servo pitchElevatorServo;

/**
 * Setup: Run once
 */
void setup() {
  // Serial connection
  Serial.begin(115200);
  Serial.println("Starting Yellow Submarine");

  // Wifi-Connection
  connectToNetwork();

  println(WiFi.macAddress());
  println(WiFi.localIP().toString());
  
  // UDP-Listener
  Udp.begin(localPort);

  Serial.println("UDP running");
  
  // Speed + tank motor
  motorDriver.begin();
  
  // Side rudder
  sideRudderServo.setPeriodHertz(PWM_FREQ_SERVO);    // standard 50 hz servo
  sideRudderServo.attach(sideRudderPin, 500, 2400);

  // Pitch elevator
  pitchElevatorServo.setPeriodHertz(PWM_FREQ_SERVO);    // standard 50 hz servo
  pitchElevatorServo.attach(pitchElevatorPin, 500, 2400);
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
    println("Establishing connection to WiFi..");
  }
  Serial.println("Connected to Wifi network");
}

/** 
 * Check if UDP package has arrived, if yes - handle it 
 */
void checkUDP() {  
  int packetSize = Udp.parsePacket();
  if(Udp.available()){
    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);

    // Extract the content via the protocoll
    int speed = ubyte2int(packetBuffer[0]);
    int sideRudder = ubyte2int(packetBuffer[1]);
    int pitchElevator = ubyte2int(packetBuffer[2]);
    int pressureTankOperation = ubyte2int(packetBuffer[3]);

    // Debug output of the parameters
    print("speed: ");
    println(speed);
    print("side rudder: ");
    println(sideRudder);
    print("pitch elevator: ");
    println(pitchElevator);
    print("tank operation: ");
    println(pressureTankOperation);
    
    setSpeed(speed);
    setSideRudder(sideRudder);
    setPitchElevator(pitchElevator);
    setPump(pressureTankOperation);
  
    delay(10);
  }
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
  speed = max(-100, min(100, speed));
  int dir = CW;
  if ( abs(speed) < 30  ){
    // Stop
    motorDriver.brake(MOTOR_SPEED);
    return;
  }
  if ( speed < 0 ){
    dir = CCW;
  }
  motorDriver.rotate(MOTOR_SPEED, abs(speed), dir);
}

/**
 * Set pump motor according to pressureTankOperationeed. pressureTankOperation must be in [0,1,2]
 */
void setPump(int pressureTankOperation){
  pressureTankOperation = max(-100, min(100, pressureTankOperation));
  int dir = CW;
  if ( abs(pressureTankOperation) < 30  ){
    // Stop
    motorDriver.brake(MOTOR_TANK);
    return;
  }
  if ( pressureTankOperation < 0 ){
    dir = CCW;
  }
  motorDriver.rotate(MOTOR_TANK, abs(pressureTankOperation), dir);
}

/**
 * Set the side rudder servo. Angle must be in [-90,90];
 */
void setSideRudder(int angle){
  angle = max(-90, min(90, angle));
  int pos = angle + 90;
  sideRudderServo.write(pos);
}

/**
 * Set the pitch elevator servo. Angle must be in [-90,90];
 */
void setPitchElevator(int angle){
  angle = max(-90, min(90, angle));
  int pos = angle + 90;
  pitchElevatorServo.write(pos);
}

/**
 * Print msg to serial console (if DEBUG is defined).
 */
void print(const char* msg){
#if ( DEBUG == true )
  Serial.print(msg);
#endif
}

/**
 * Println msg to serial console (if DEBUG is defined).
 */
void println(String msg){
#if ( DEBUG == true )
  Serial.println(msg);
#endif
}


/**
 * Println msg to serial console (if DEBUG is defined).
 */
void println(int msg){
#if ( DEBUG == true )
  Serial.println(msg);
#endif
}

/**
 * Newline serial console (if DEBUG is defined).
 */
void println(){
#if ( DEBUG == true )
  Serial.println();
#endif
}
