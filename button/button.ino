#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,9);
int inputPin = 2;               // choose the input pin (for a pushbutton)
int val = 0;                     // variable for reading the pin status
int out1;

void setup() {
     
 pinMode(inputPin, INPUT);     // declare pushbutton as input
 Serial.begin(9600);



  //Set Exact Baud rate of the GSM/GPRS Module.
  
  Serial.print("\r");
  delay(1000);           
  
}






void loop(){
 val = digitalRead(inputPin);  // read input value
 if (val == HIGH) {            // check if the input is HIGH

   mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+919664919955\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("Your Family member has met with an Accident");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
   
   out1 = 1;
}
    else {
    
   
  out1 = 0;
 }
  Serial.println(out1);
  delay(225);
} 
