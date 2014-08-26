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

int upper_servo_pos = U_OFF_POS;
int lower_servo_pos = L_OFF_POS;

//enums
int UPPER = 0;
int LOWER = 1;

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
  if (BT.available()){
    char t = BT.read();
    Serial.write(t); // write all on the SerialMonitor what is getting from Bluetooth
    handle_message(t);
  }
  if (Serial.available()){
    char t = Serial.read();
    handle_message(t); // for debugging - handle message from serial as well
  }
}


void move_servo(int servo_id, int dst_pos){
  Servo * servo;
  int * pos;
  if (servo_id == UPPER){
    servo = &upper_servo;
    pos = &upper_servo_pos;
  } else {
    servo = &lower_servo;
    pos = &lower_servo_pos;
  }
  int diff = *pos > dst_pos ? -1 : 1;
  for(; *pos != dst_pos; *pos += diff)     // goes from cur_pos to dst_pos 
  {
    digitalWrite(LED_PIN, *pos % 20 < 10 ? HIGH : LOW);    
    servo->write(*pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }

  digitalWrite(LED_PIN, HIGH);
}

