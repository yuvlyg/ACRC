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
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
int start_pos;
int end_pos;
SoftwareSerial BT(10, 11); // RX, TX

int KEY_PIN = 9; // set mode of the Bluetooth (AT or Pairing mode)
int SERVO_PIN = 2;
int LED_PIN = 12;

int ON_POS = 160;
int OFF_POS = 100;
int servo_pos = OFF_POS;

void setup()  
{
  
  myservo.attach(SERVO_PIN);
  
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
  move_servo(OFF_POS);
}


void handle_message(char message){
    if (message == '1'){
      move_servo(ON_POS);
    } else if (message == '0') {
      move_servo(OFF_POS);
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


void move_servo(int dst_pos){
  int diff = servo_pos > dst_pos ? -1 : 1;
  for(int pos = servo_pos; pos != dst_pos; pos += diff)     // goes from cur_pos to dst_pos 
  {
    digitalWrite(LED_PIN, pos % 20 < 10 ? HIGH : LOW);    
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  servo_pos = dst_pos;
  digitalWrite(LED_PIN, HIGH);
}

