#include <Servo.h>

// Arduino pin assignment
#define PIN_IR    A0         // IR sensor at Pin A0
#define PIN_LED   9
#define PIN_SERVO 10

// Servo duty cycle (microseconds) for different positions
#define _DUTY_MIN 1000       // Servo full clockwise position (0 degree)
#define _DUTY_NEU 1500       // Servo neutral position (90 degree)
#define _DUTY_MAX 2000       // Servo full counter-clockwise position (180 degree)

// Distance range for detection (in mm)
#define _DIST_MIN  100.0     // Minimum distance 100mm
#define _DIST_MAX  250.0     // Maximum distance 250mm

// EMA filter alpha (smoothing factor)
#define EMA_ALPHA  0.1       // Adjust alpha for smoothing

// Loop interval in milliseconds
#define LOOP_INTERVAL 20     // 20 ms

Servo myservo;
unsigned long last_loop_time = 0;   // Last loop execution time in milliseconds

float dist_ema = _DIST_MIN;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  
  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU); // Set servo to neutral position
  
  Serial.begin(1000000);    // Serial communication at 1,000,000 bps
}

void loop() {
  unsigned long time_curr = millis();
  
  // Wait until the next event time
  if (time_curr < last_loop_time + LOOP_INTERVAL) {
    return;
  }
  last_loop_time += LOOP_INTERVAL;

  // Read IR sensor value and convert to distance
  float a_value = analogRead(PIN_IR);
  float dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0;

  // Apply range filter (10cm to 25cm)
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, HIGH); // Turn on LED if in range
  } else {
    digitalWrite(PIN_LED, LOW);  // Turn off LED if out of range
  }

  // Apply EMA filter to smooth out the distance measurements
  dist_ema = EMA_ALPHA * dist_raw + (1 - EMA_ALPHA) * dist_ema;

  // Calculate duty cycle for servo based on smoothed distance (dist_ema)
  int duty = _DUTY_MIN + ((dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN)) / (_DIST_MAX - _DIST_MIN);

  // Write duty cycle to servo
  myservo.writeMicroseconds(duty);

  // Serial output for monitoring
  Serial.print("_DUTY_MIN:"); Serial.print(_DUTY_MIN);
  Serial.print(", _DIST_MIN:"); Serial.print(_DIST_MIN);
  Serial.print(", IR:"); Serial.print(a_value);
  Serial.print(", dist_raw:"); Serial.print(dist_raw);
  Serial.print(", ema:"); Serial.print(dist_ema);
  Serial.print(", servo:"); Serial.print(duty);
  Serial.print(", _DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(", _DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
