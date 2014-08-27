  /*
 HC-05.ino
 
 (C)2014 noro24.de, http://www.noro24.de
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 any later version. http://www.gnu.org/licenses/
 
Arduino pins: 
 - RX is digital pin 10 (connect to Bluetooth TX )
 - TX is digital pin 11 (connect to Bluetooth RX)
 */
 
#include <SoftwareSerial.h>
#include <Servo.h> 

Servo upper_servo;  // create servo object to control a servo 
Servo lower_servo;

SoftwareSerial BT(10, 11); // RX, TX
  
int KEY_PIN = 9; // set mode of the Bluetooth (AT or Pairing mode)
int U_SERVO_PIN = 2;
int L_SERVO_PIN = 3; //TODO connect
int LED_PIN = 12;

int U_ON_POS = 160;
int U_OFF_POS = 100;
int L_ON_POS = 160;
int L_OFF_POS = 100;

int INITIAL_SERVO_DELAY = 15;
int servo_delay = INITIAL_SERVO_DELAY;

// config delay parsing
int tmp_delay;
bool configuring_delay = false;

//enums
const int UPPER = 0;
const int LOWER = 1;
const int MOVE_UPPER = 0x30;         // '0'
const int MOVE_LOWER = 0x31;         // '1'
const int CONFIG_DELAY_START = 0x64; // 'd'
const int CONFIG_DELAY_STOP = 0x73;  // 's'



void setup()  
{
  
  upper_servo.attach(U_SERVO_PIN);
  lower_servo.attach(L_SERVO_PIN);
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600); // for AT mode
  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }*/

  // set the data rate for HC-05
  BT.begin(9600);

  pinMode(KEY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  digitalWrite(KEY_PIN, HIGH); // AT mode
  //digitalWrite(KEY_PIN, LOW); // Pairing mode
  
  delay(500);
  move_servo(UPPER, U_OFF_POS);
  move_servo(LOWER, L_OFF_POS);
}


void handle_message(char message){
    if (message == '1'){
      move_servo(UPPER, U_ON_POS);
      move_servo(UPPER, U_OFF_POS);
    } else if (message == '0') {
      move_servo(LOWER, L_ON_POS);
      move_servo(LOWER, L_OFF_POS);
    }
}

void loop() 
{
  digitalWrite(LED_PIN, HIGH);
  char t = 0x0;
  if (BT.available()){
    t = BT.read();
    Serial.write(t); // write all on the SerialMonitor what is getting from Bluetooth
  }
  if (Serial.available()){
    t = Serial.read(); // for debugging - handle message from serial as well
  }
  if (t != 0x0) { // got input from either BT/Serial
    if (configuring_delay) {
      if (t == CONFIG_DELAY_STOP) {
        servo_delay = tmp_delay;
        tmp_delay = 0;
        configuring_delay = false;
        Serial.print("\nconfiguring delay: ");
        Serial.print(servo_delay);
      } else {
        tmp_delay *= 10;
        tmp_delay += t - '0';
        Serial.print("\ntmp delay is ");
        Serial.print(tmp_delay);
      }
    } else {
      switch (t) {
        case MOVE_UPPER:
        case MOVE_LOWER:
          handle_message(t);
          break;
        case CONFIG_DELAY_START:
          tmp_delay = 0;
          configuring_delay = true;
          break;
      }
    }
  }
}


void move_servo(int servo_id, int dst_pos){
  Servo * servo;
  int pos;
  
  if (servo_id == UPPER){
    servo = &upper_servo;
  } else {
    servo = &lower_servo;
  }
  pos = servo->read();
  int diff = pos > dst_pos ? -1 : 1;
  for(; pos != dst_pos; pos += diff)     // goes from cur_pos to dst_pos 
  {
    digitalWrite(LED_PIN, pos % 20 < 10 ? HIGH : LOW);    
    servo->write(pos);              // tell servo to go to position in variable 'pos' 
    delay(servo_delay);                      // waits 15ms for the servo to reach the position 
  }

  digitalWrite(LED_PIN, HIGH);
}

