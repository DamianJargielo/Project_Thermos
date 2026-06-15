/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiUdp.h>

#include <WiFiAP.h>



// Set these to your desired credentials.
const char *ssid = "RobotWifi";
const char *password = "Projekt11";

const char *udpAddress = "192.168.4.1";
const int udpPort = 10000;
WiFiUDP udp;
const int FL_F_pin = 16;
const int FL_R_pin = 17;
const int FR_F_pin = 18;
const int FR_R_pin = 19;
const int RL_F_pin = 21;
const int RL_R_pin = 22;
const int RR_F_pin = 23;
const int RR_R_pin = 25;

const int pwmPin1 = 26; 
const int pwmPin2 = 27; 
// Custom PWM settings
const uint32_t frequency = 333; // Frequency in Hz (e.g., 5 kHz)
const uint8_t resolution = 8;
struct Serial_input{
int16_t forward_back_ax;
int16_t left_right_ax;
int16_t hor_rotate_ax;
uint8_t servo_pos;

};
struct motors_speed{
int16_t front_l;
int16_t front_r;
int16_t rear_l;
int16_t  rear_r;

};
const float acceleration_coef =0.5;

uint8_t data_recieved = 0;
uint8_t stage = 0;
int negative = 0;
struct Serial_input usb_in;
struct motors_speed vel;
void setup() {
  vel.front_l = 0;
  vel.front_r = 0;
  vel.rear_l = 0;
  vel.rear_r = 0;

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
ledcAttach(pwmPin1, frequency, resolution);
ledcAttach(pwmPin2, frequency, resolution);
  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);


  Serial.println("Server started");
   udp.begin(udpPort);
   ledcWrite(pwmPin1, 128);
   ledcWrite(pwmPin2, 128);
}

void loop() {
  int8_t rot_left_right = 0; //0 - going straight, 1 - turning left, (-1) - turning right
 
int packetSize = udp.parsePacket();

 if(packetSize) {
 
// uint8_t velocity = udp.read();
    char packetBuffer[22] = { 0 };      // buffer to hold incoming packet,
    udp.read(packetBuffer, packetSize); 
    uint8_t value = 0;
    usb_in.forward_back_ax = 0;
    usb_in.left_right_ax = 0;
    usb_in.hor_rotate_ax = 0;
    usb_in.servo_pos = 0;
for(int i = 0; i<22; i++){
int test = (int)packetBuffer[i];
Serial.println(value);
  if(value == 0){
    if(packetBuffer[i] != ','){
if(packetBuffer[i] == '-'){
negative = 1;
  
}
else{
  if((int)packetBuffer[i]>= 48 && (int)packetBuffer[i] <=57){
usb_in.forward_back_ax *= 10;
usb_in.forward_back_ax += (int)packetBuffer[i]- 48;
  }

}

      
    }
    else{
    if(negative == 1){

      negative = 0;
    }
    else{
usb_in.forward_back_ax *= -1;
      
    }
value ++;
      
    }
  }
   else if(value == 1){
    if(packetBuffer[i] != ','){
if(packetBuffer[i] == '-'){
negative = 1;
  
}
else{
  if((int)packetBuffer[i]>= 48 && (int)packetBuffer[i] <=57){
usb_in.left_right_ax *= 10;
usb_in.left_right_ax += (int)packetBuffer[i]- 48;
  }

}

      
    }
    else{
    if(negative == 1){
usb_in.left_right_ax *= -1;
      negative = 0;
    }
value ++;
      
    }
  }


     else if(value == 2){
    if(packetBuffer[i] != ','){
if(packetBuffer[i] == '-'){
negative = 1;
  
}
else{
  if((int)packetBuffer[i]>= 48 && (int)packetBuffer[i] <=57){
usb_in.hor_rotate_ax *= 10;
usb_in.hor_rotate_ax += (int)packetBuffer[i]- 48;
  }

}

      
    }
    else{
    if(negative == 1){
usb_in.hor_rotate_ax *= -1;
      negative = 0;
    }
value = 3;
      
    }
  }
else if(value == 3){
    if(packetBuffer[i] != ','){


  if((int)packetBuffer[i]>= 48 && (int)packetBuffer[i] <=57){

usb_in.servo_pos += (int)packetBuffer[i]- 48;
  }

}

     else{
    
value = 0;
      
    }   
    }


  
}
Serial.println(usb_in.forward_back_ax);
Serial.println(usb_in.left_right_ax);
Serial.println(usb_in.hor_rotate_ax);
Serial.println(usb_in.servo_pos);
Serial.println();
     
   

 vel.front_l = usb_in.forward_back_ax - (usb_in.hor_rotate_ax)/2 -usb_in.left_right_ax;
  vel.front_r = usb_in.forward_back_ax + (usb_in.hor_rotate_ax)/2 + usb_in.left_right_ax;
  vel.rear_l = usb_in.forward_back_ax  - (usb_in.hor_rotate_ax)/2+usb_in.left_right_ax;
  vel.rear_r = usb_in.forward_back_ax + (usb_in.hor_rotate_ax)/2 -usb_in.left_right_ax; 
if(vel.front_l > 250){
vel.front_l = 250;


}
else if(vel.front_l < -250){
vel.front_l = -250;

}
if(vel.front_r > 250){
vel.front_r = 250;


}
else if(vel.front_r < -250){
vel.front_r = -250;


}
if(vel.rear_l > 250){
vel.rear_l = 250;


}
else if(vel.rear_l < -250){
vel.rear_l = -250;


}
if(vel.rear_r > 250){
vel.rear_r = 250;


}
else if(vel.rear_r < -250){
vel.rear_r = -250;


}


/*const int FL_F_pin = 16;
const int FL_R_pin = 17;
const int FR_F_pin = 18;
const int FR_R_pin = 19;
const int RL_F_pin = 20;
const int RL_R_pin = 21;
const int RR_F_pin = 22;
const int RR_R_pin = 23;*/


if(vel.front_l >= 0 && vel.front_l <= 255){
analogWrite(FL_F_pin,vel.front_l);
analogWrite(FL_R_pin,0);

}
 if(vel.front_l <= 0 && vel.front_l >= -255){
analogWrite(FL_R_pin,vel.front_l*(-1));
analogWrite(FL_F_pin,0);

} 
if(vel.front_r >= 0 && vel.front_r <= 255){
analogWrite(FR_F_pin,vel.front_r);
analogWrite(FR_R_pin,0);

}
 if(vel.front_r <= 0 && vel.front_r >= -255){
analogWrite(FR_R_pin,vel.front_r*(-1));
analogWrite(FR_F_pin,0);

} 


if(vel.rear_l >= 0 && vel.rear_l <= 255){
analogWrite(RL_F_pin,vel.rear_l);
analogWrite(RL_R_pin,0);

}
 if(vel.rear_l <= 0 && vel.rear_l >= -255){
analogWrite(RL_R_pin,vel.rear_l*(-1));
analogWrite(RL_F_pin,0);

} 
if(vel.rear_r >= 0 && vel.rear_r <= 255){
analogWrite(RR_F_pin,vel.rear_r);
analogWrite(RR_R_pin,0);

}
 if(vel.rear_r <= 0 && vel.rear_r >= -255){
analogWrite(RR_R_pin,vel.rear_r*(-1));
analogWrite(RR_F_pin,0);

}
if(usb_in.servo_pos == 1){
ledcWrite(pwmPin1, 150);
ledcWrite(pwmPin2, 106);
}
else
{
ledcWrite(pwmPin1, 112);
ledcWrite(pwmPin2, 144); 
}




}
delay(1);
}
