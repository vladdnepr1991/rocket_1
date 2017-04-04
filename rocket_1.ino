#include <Wire.h>
#include <Servo.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Servo myservo;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

const int SERVO_PIN = 2;
const int CLOSE_DOOR_PIN = 12;
const int OPEN_DOOR_PIN = 11;
const int PRESS_SENSOR_WORK_PIN = 4;
const int PRESS_SENSOR_NOT_WORK_PIN = 3;

bool isPressureSensorReady = false;
int servoOpenPos = 10;
int servoClosePos = 175;
int altDiff = 2;
float startAlt = 0;
float maxAlt = 0;
float currentAlt = 0;

float getCurrentAltitude() {
  sensors_event_t event;
  float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
  
  bmp.getEvent(&event);
  
  return bmp.pressureToAltitude(seaLevelPressure, event.pressure);
}

void toggleParachuteState(bool state) {
  myservo.write(state ? servoOpenPos : servoClosePos);
}

void openParachuteByConditions(float maxAlt, float currentAlt, int altDiff) {
  if (maxAlt - currentAlt > altDiff) {
    toggleParachuteState(true);
  }
}

void buttonsListener() {
  bool isOpnenDoorBtnClicked = digitalRead(OPEN_DOOR_PIN);
  bool isCloseDoorBtnClicked = digitalRead(CLOSE_DOOR_PIN);

  if (!isOpnenDoorBtnClicked) {
    toggleParachuteState(true);
  }

  if (!isCloseDoorBtnClicked) {
    toggleParachuteState(false);
  }
}

void toggleWorkIndicator(bool state) {
  digitalWrite(PRESS_SENSOR_WORK_PIN, state);
  digitalWrite(PRESS_SENSOR_NOT_WORK_PIN, !state);
}

void logAltitude() {
  Serial.print("isPressureSensorReady: ");
  Serial.println(isPressureSensorReady);
  
  Serial.print("openBtn: ");
  Serial.println(digitalRead(OPEN_DOOR_PIN));

  Serial.print("closeBtn: ");
  Serial.println(digitalRead(CLOSE_DOOR_PIN));  
  
  Serial.print("current: ");
  Serial.println(currentAlt);

  Serial.print("start: ");
  Serial.println(startAlt);
  
  Serial.print("max: ");
  Serial.println(maxAlt);
  Serial.println("__________________"); 
}

void setup(void) {
  isPressureSensorReady = bmp.begin();
  
  Serial.begin(9600);
  myservo.attach(SERVO_PIN);
  pinMode(CLOSE_DOOR_PIN, INPUT_PULLUP);
  pinMode(OPEN_DOOR_PIN, INPUT_PULLUP);
  pinMode(PRESS_SENSOR_WORK_PIN, OUTPUT);
  pinMode(PRESS_SENSOR_NOT_WORK_PIN, OUTPUT);
  toggleParachuteState(false);
  toggleWorkIndicator(isPressureSensorReady);

  startAlt = getCurrentAltitude();
}

void loop(void) {
  currentAlt = getCurrentAltitude();
  maxAlt = currentAlt > maxAlt ? currentAlt : maxAlt;

  buttonsListener();
  openParachuteByConditions(maxAlt, currentAlt, altDiff);
  toggleWorkIndicator(isPressureSensorReady);
//  logAltitude();
}
