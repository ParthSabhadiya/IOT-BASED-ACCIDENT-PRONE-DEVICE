/* 

19-02-2019
 
The A. D. Patel Institute of Technology Hackathon Team from computer engineering 2nd year dept.


TEAM ID : TG001068

Definition Problem :IOT Based Accident Prone Device


*/
#include <Wire.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include<string.h>
SoftwareSerial mySerial(3,2);

#define DEBUG true

// twelve servo objects can be created on most boards

const int rs = 10, en = 9, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int pos = 0;    // variable to store the servo position

#define call_direct 13

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

char latitude[15];
char longitude[15];
char altitude[6];
char date[16];
char time[7];
char satellites[3];
char speedOTG[10];
char course[10];

int buttonpin=11;
int bz=12;
int stbus; // To save the last logic state of the button
       
  

char data[100];
int data_size;

char aux_str[30];
char aux;
int x = 0;
char N_S,W_E;


char frame[200];
int8_t answer,answer1;
String response1 = "";    
int onModulePin= 8;
char aux_string[30];
int flag = 0;

char phone_number[]="+917046429193"; //enter country code in XX and Mobile number to whome you want to send SMS over YYYYYYYYYY
char phone_number1[]="+919664919955";
void setup() {
    pinMode(buttonpin,INPUT);
    pinMode(call_direct, INPUT_PULLUP);

    pinMode(bz, OUTPUT);
    digitalWrite(bz,LOW);     
 
  pinMode(10,OUTPUT);
  
  lcd.begin(16, 2);
  
  // Print a message to the LCD.
  lcd.setCursor(3, 0);
  
 
  mySerial.begin(9600);
  Serial.begin(9600);
   lcd.setCursor(0,0);
    lcd.print("Accident Alert");
  lcd.setCursor(0,1);
  
  lcd.print("     System     ");
  delay(2000);
  lcd.clear();
  lcd.print("Initializing");
  lcd.setCursor(0,1);
  lcd.print("Please Wait...");
  lcd.clear();
   lcd.println("bike ready to go");
  Wire.begin();
  setupMPU();
  getgps();
  
  delay(1000);
}


void loop() {
  recordAccelRegisters();
  recordGyroRegisters();
  printData();
  digitalWrite(stbus,LOW);
  stbus = digitalRead(buttonpin); //Put the reading value of the switch on botao
      Serial.println(stbus); //Shows the logic state of the input on Serial Monitor

    
   if(stbus== 1 && digitalRead(bz)==1 )
  {
    
    Serial.println("button pressed");
    lcd.clear();
    lcd.print("Button Pressed");
    
    SendMessage1(); 
    Serial.println("sent 1");
   
    SendMessage2();
    Serial.println("sent 2");
    digitalWrite(bz,LOW);
     
    
  }  
}

void SendMessage1()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(100);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+917046429193\"\r"); //enter country code in XX and Mobile number to whome you want to send SMS over YYYYYYYYYY
  delay(100);
  mySerial.println("I am Fine");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(100);
}  

void SendMessage2()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(100);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+919664919955\"\r"); //enter country code in XX and Mobile number to whome you want to send SMS over YYYYYYYYYY
  delay(100);
  mySerial.println("I am Fine");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(100);
}  


void setupMPU(){
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
}

void recordAccelRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}

void recordGyroRegisters() {
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}

void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}

void printData() {
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print(" Z=");
  Serial.print(rotZ);
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);

  
    if((gForceX>=0.20 || gForceX <= (-0.25))&&((rotZ >=80)||rotY>=50 ||rotZ<=(-100)))
  {

    digitalWrite(10,HIGH);
    digitalWrite(5,HIGH);
    Serial.println("accident");
    lcd.setCursor(1, 0);
    lcd.clear();
    lcd.print("Accident occur");
    
      digitalWrite(bz,HIGH);
      
      // callNumber();
     // delay(1000);
      
    // sendSMS1();
    // sendSMS2();
      
      lcd.clear();
    lcd.print("Message sent"); 
    
      
  }

  else 
  {
    digitalWrite(5,LOW);
    

      RecieveMessage(); 
      Serial.println("Bike Steady");
      lcd.setCursor(3, 0);
      lcd.clear();
      
      lcd.print("Bike Steady");
     
  }

   if(gForceZ>=50 || gForceZ<= -50 )
  {

    digitalWrite(10,HIGH);
    digitalWrite(5,HIGH);
    Serial.println("accident");
    lcd.setCursor(1, 0);
    lcd.clear();
    lcd.print("Accident occur");
    
      digitalWrite(bz,HIGH);
    
     // callNumber();
     // delay(1000);
      
    // sendSMS1();
      
    // sendSMS2();
      
      lcd.clear();
    lcd.print("Message sent");

  }
}   

 void callNumber()
 {
  
  mySerial.println("ATD7046429193;");
  delay(1000);
 }



 void RecieveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
 }
 int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialice the string
    
    delay(100);
    
    while( mySerial.available() > 0) mySerial.read();    // Clean the input buffer
    
    if (ATcommand[0] != '\0')
    {
        mySerial.println(ATcommand);    // Send the AT command 
    }


    x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(mySerial.available() != 0){    // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = mySerial.read();
            //Serial.print(response[x]);
            x++;
            if (strstr(response, expected_answer) != NULL)    // check if the desired answer (OK) is in the response of the module
            {
                answer = 1;
            }
        }
    }while((answer == 0) && ((millis() - previous) < timeout));    // Waits for the asnwer with time out

    return answer;
}

void getgps(void){
   sendData( "AT+CGNSPWR=1",1000,DEBUG); 
   sendData( "AT+CGPSINF=0",1000,DEBUG); 
}

int8_t convert2Degrees(char* input){

    double deg;
    double minutes;
    
    boolean neg = false;    

    //auxiliar variable
    char aux[10];

    if (input[0] == '-')
    {
        neg = true;
        strcpy(aux, strtok(input+1, "\0"));

    }
    else
    {
        strcpy(aux, strtok(input, "\0"));
    }

    // convert string to integer and add it to final float variable
    double act=atof(aux);
    deg = atof(aux);
    Serial.println(deg);

    strcpy(aux, strtok(NULL, '\0'));
    
    minutes=atof(aux);
    Serial.println(minutes);

     minutes/=1000000;
   if (deg < 100)
    {
        minutes += deg;
        deg = 0;
    }
    else
    {
        minutes += int(deg) % 100;
        deg = int(deg) / 100;    
    }
    
    deg=floor(act/100);
    minutes=act-(100*deg);

    // add minutes to degrees 
    minutes/=60;
    deg+=minutes;


    if (neg == true)
    {
        deg*=-1.0;
    }

    neg = false;

    if( deg < 0 ){
        neg = true;
        deg*=-1;
    }
    
    float numberFloat=deg; 
    int intPart[10];
    int digit; 
    long newNumber=(long)numberFloat;  
    int size=0;
    
    while(1){
        size=size+1;
        digit=newNumber%10;
        newNumber=newNumber/10;
        intPart[size-1]=digit; 
        if (newNumber==0){
            break;
        }
    }
   
    int index=0;
    if( neg ){
        index++;
        input[0]='-';
    }
    for (int i=size-1; i >= 0; i--)
    {
        input[index]=intPart[i]+'0'; 
        index++;
    }

    input[index]='.';
    index++;

    numberFloat=(numberFloat-(int)numberFloat);
    for (int i=1; i<=6 ; i++)
    {
        numberFloat=numberFloat*10;
        digit= (long)numberFloat;          
        numberFloat=numberFloat-digit;
        input[index]=char(digit)+48;
        index++;
    }
    input[index]='\0';


}

void sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
int8_t counter=0, answer=0;
    
    memset(frame, '\0', 200);    // Initialize the string
    mySerial.println(command); 
    delay(5);
    if(debug){
    long int time = millis();   
    while( (time+timeout) > millis()){
      
      while(mySerial.available()){       
        frame[counter] += char(mySerial.read());
        counter++;
      }  
    }    
      Serial.println(frame);
    
    strtok(frame, ",");
    strcpy(longitude,strtok(NULL, ",")); // Gets longitude
    strcpy(latitude,strtok(NULL, ",")); // Gets latitude
    strcpy(altitude,strtok(NULL, ".")); // Gets altitude 
    strtok(NULL, ",");    
    strcpy(date,strtok(NULL, ".")); // Gets date
    strtok(NULL, ",");
    strtok(NULL, ",");  
    strcpy(satellites,strtok(NULL, ",")); // Gets satellites
    strcpy(speedOTG,strtok(NULL, ",")); // Gets speed over ground. Unit is knots.
    strcpy(course,strtok(NULL, "\r")); // Gets course
  
    convert2Degrees(latitude);
    convert2Degrees(longitude);  
    Serial.println(latitude);
    Serial.println(longitude);
}

}

void sendSMS1(){
  char str1[100];
  sendATcommand("AT+CPIN=**", "OK", 2000);
    
    Serial.println("Connecting to the network...");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
  
    answer1 = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer1 == 1)
    {   mySerial.println("Your Family member has met with an Accident");// The SMS text you want to send
        mySerial.println("Find me Here:-");
      
              mySerial.println();
              mySerial.print("link :-");
              sprintf(str1,"http://maps.google.com/?q=23.155524,72.667104",longitude,latitude);
              
        mySerial.print(str1);      
        mySerial.write(0x1A);
        answer1 = sendATcommand("", "OK", 20000);
        if (answer1 == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }

}

void sendSMS2(){
  char str1[100];
  sendATcommand("AT+CPIN=**", "OK", 2000);
   
    
    Serial.println("Connecting to the network...");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.print("Setting SMS mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    Serial.println("Sending SMS");
 
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number1);
    answer1 = sendATcommand(aux_string, ">", 2000);    // send the SMS number
    if (answer1 == 1)
    {   mySerial.println("Your Family member has met with an Accident");// The SMS text you want to send
        mySerial.println("Find me Here:-");
        
              mySerial.println();
              mySerial.print("link :-");
              sprintf(str1,"http://maps.google.com/?q=23.155524,72.667104",longitude,latitude);
              
        mySerial.print(str1);      
        mySerial.write(0x1A);
        answer1 = sendATcommand("", "OK", 20000);
        if (answer1 == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    }

}


