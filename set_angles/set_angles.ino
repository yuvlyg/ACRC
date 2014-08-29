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
int L_SERVO_PIN = 3;
int LED_PIN = 12;

int U_ON_POS = 160;
int U_OFF_POS = 100;
int L_ON_POS = 160;
int L_OFF_POS = 100;

int SERVO_DELAY = 15;

//enums
const int UPPER = 0;
const int LOWER = 1;


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
    if (message >= '1' && message <= '4'){
      Serial.print("Valid message...\n");
    }
    if (message == '1'){
      move_servo(UPPER, upper_servo.read() + 5);
    } else if (message == '2') {
      move_servo(UPPER, upper_servo.read() - 5);
    } else if (message == '3'){
      move_servo(LOWER, lower_servo.read() + 5);
    } else if (message == '4') {
      move_servo(LOWER, lower_servo.read() - 5);
    } else {
      Serial.print("Unknown message: ");
      Serial.print(message);
      Serial.print("\n");
    }
    Serial.print("Upper position: ");
    Serial.print(upper_servo.read());
    Serial.print(", Lower position: ");
    Serial.print(lower_servo.read());
    Serial.print("\n");
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
  if (t != 0x0 && t != '\n') { // got input from either BT/Serial
    handle_message(t);
  }
}


void move_servo(int servo_id, int dst_pos){
  Serial.print("moving servo to ");
  Serial.print(dst_pos);
  Serial.print("\n");
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
    delay(15);                      // waits 15ms for the servo to reach the position 
  }

  digitalWrite(LED_PIN, HIGH);
}

