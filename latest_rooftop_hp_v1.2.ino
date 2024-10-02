#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"
#include <EEPROM.h>

#include <SoftwareSerial.h>       //Software Serial library                                                       //////
SoftwareSerial espSerial(50, 49);   //Pin 50 and 49 act as RX and TX. Connect them to TX and RX of ESP8266        //////
#define DEBUG true                                                                                                //////  
String HOST = "api.thingspeak.com";                                                                               //////
String PORT = "80"; 
struct{String ID;String PSSWD;String API; }IoT;     //Data from display store stored here 
String ID, PSSWD, API;                              //Data from EEPROM store here

unsigned int count_tx_time=0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EC_PIN A0
#define PH_PIN A1

#define ONE_WIRE_BUS 2

float  voltagePH, voltageEC, phValue, ecValue, temperature;
DFRobot_PH ph;
DFRobot_EC ec;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float Celcius = 0;
float Fahrenheit = 0;

struct sensor_stat{
  float ec;
  float ph;
  };

sensor_stat auto_mode=    {1.5,6.5};
sensor_stat lettuce=      {1.5,6.5};
sensor_stat corriender=   {1.5,6.5};
sensor_stat cauliflower=  {1.5,6.5};
sensor_stat spanich=      {1.5,6.5};
sensor_stat broccoli=     {1.5,6.5};
sensor_stat chives=       {1.5,6.5};
sensor_stat chilli=       {1.5,6.5};
sensor_stat strawberry=   {1.5,3.5};
sensor_stat user_mode;
sensor_stat set_val;
sensor_stat current_val;

String NutrientA = "OFF";
String NutrientB = "OFF";
String NutrientC = "OFF";
String Pump_status= "OFF";


int relay_pin1 = 4;           //Nutrient A
int relay_pin2 = 5;           //Nutrient B
int relay_pin3 = 6;           //Nutrient C
int relay_pin4 = 7;           //Nutrient Pump Relay


String readString;
char selection, selection1;

unsigned int count_interrupt=0;       //To count number of interrupts with TIMER overflow (Added By Narendra to control nutrient pump)
//char TIMER_Start_flag=1;                //Flag to start TIMER1

void setup() {
  Serial1.begin(9600);    //Connect Display
  Serial.begin(9600); 
  get_iot_param();        //Get Wifi name, password and Write API 
  init_esp();             //start esp communication

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(relay_pin1, OUTPUT);
  digitalWrite(relay_pin1, HIGH);
  pinMode(relay_pin2, OUTPUT);
  digitalWrite(relay_pin2, HIGH);
  pinMode(relay_pin3, OUTPUT);
  digitalWrite(relay_pin3, HIGH);
  pinMode(relay_pin4, OUTPUT);
  digitalWrite(relay_pin4, HIGH);


  ph.begin();
  ec.begin();   
}

void loop() {
  
    while (Serial1.available()) {
    delay(3);  //delay to allow buffer to fill
    if (Serial1.available() > 0) {
      selection = Serial1.read();  //gets one byte from serial buffer
      readString += selection; //makes the string readString
    }
  }
  Serial.println(readString);
  readString = "";

  Serial.println(selection1);
  if(selection=='A' || selection== 'M' || selection=='U' || selection=='I' || selection =='S')      //A = Auto | M = Manual | U = User | I = IoT Configuration | S = Back to Home
  {
   
  selection1=selection;
  selection="";
  }

 switch (selection1)
  {
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    case 'S':                                         //for home screen (Reach via back button)
      //selection1="";
      Serial.println("At Home");
      digitalWrite(relay_pin1, HIGH);
      digitalWrite(relay_pin2, HIGH);
      digitalWrite(relay_pin3, HIGH);
      //digitalWrite(relay_pin4, HIGH);
      TCCR1A=0X00;
      TCCR1B=0x00;
      count_interrupt=0;
      digitalWrite(relay_pin4, HIGH);    
      break;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    case 'A':                             //Auto mode
      set_val.ec=auto_mode.ec;
      set_val.ph=auto_mode.ph;
      //set on and off time also
      init_timer1();
      Serial.println("Auto Mode selected");      // Automatic Mode Selected
      selection1='R';
      break;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    case 'M':                              //Manual Mode
      char input;  
      while(!Serial1.available());        //Wait for input from display
      if (Serial1.available() > 0) 
      {
        input = Serial1.read();  //gets one byte from serial buffer
      }
      Serial.println(readString);
      Serial.println(input);
      switch (input)
      {
        case 'A':     //Broccoli
            Serial.println(" Broccoli selected");   
            set_val.ec=broccoli.ec;
            set_val.ph=broccoli.ph; 
            //Also set on off time for pump
            init_timer1();
            selection1='R';
            break;
        case 'B':     //Cauliflower
            Serial.println(" Cauliflower selected");   
            set_val.ec=cauliflower.ec;
            set_val.ph=cauliflower.ph;  
            //Also set on off time for pump 
            init_timer1();
            selection1='R';
            break;
        case 'C':     //Chilli
            Serial.println(" Chilli selected");   
            set_val.ec=chilli.ec;
            set_val.ph=chilli.ph; 
            //Also set on off time for pump  
            init_timer1();
            selection1='R';
            break;
        case 'D':     //Chives
            Serial.println(" Chives selected");   
            set_val.ec=chives.ec;
            set_val.ph=chives.ph;
            //Also set on off time for pump
            init_timer1();
            selection1='R';
            break;
        case 'E':     //Corriender
            Serial.println(" Corriender selected"); 
            set_val.ec=corriender.ec;
            set_val.ph=corriender.ph;
            //Also set on off time for pump    
            init_timer1(); 
            selection1='R';
            break;
        case 'F':     //Lettuce
            Serial.println(" Lettuce selected");  
            set_val.ec=lettuce.ec;
            set_val.ph=lettuce.ph;
            //Also set on off time for pump 
            init_timer1();
            selection1='R';
            break;
        case 'G':     //Spanich
            Serial.println(" Spanich selected");  
            set_val.ec=spanich.ec;
            set_val.ph=spanich.ph; 
            //Also set on off time for pump   
            init_timer1();
            selection1='R';
            break;
        case 'H':     //Strawberry
            Serial.println(" Strawberry selected"); 
            set_val.ec=strawberry.ec;
            set_val.ph=strawberry.ph;
            //Also set on off time for pump 
            init_timer1();
            selection1='R';
            break;
        case 'S':     //Back 
            selection1='S';
            break;
        }
        input="";
        break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////      
    case 'U':
      Serial.println("Inside User Mode");
      char input2;  
      while(!Serial1.available());                //Wait for input from display  
      if (Serial1.available() > 0) 
      {
        input2 = Serial1.read();  //gets one byte from serial buffer
      }
      if(input2=='E')                                       //If button for EC Pressed
      {
        Serial.println("Inside EC Case");
        String EC;
        char EC_ch;
        while(!Serial1.available());
        while (Serial1.available()) 
        {
          delay(3);  //delay to allow buffer to fill
          if (Serial1.available() > 0) 
          {
            EC_ch = Serial1.read();  //gets one byte from serial buffer
            EC += EC_ch; //makes the string readString
          }
        }   
        unsigned int ec_i=EC.toInt();     
        float ec_f=float(ec_i)/1000;
        Serial.println(ec_f);
        user_mode.ec=ec_f;        
        input2="";
     }
     else if (input2=='P')                                       //If button for PH Pressed
     { 
      Serial.println("Inside PH Case");
      String PH;
      char PH_ch;
      while(!Serial1.available());
      while (Serial1.available()) 
      {
        delay(3);  //delay to allow buffer to fill
        if (Serial1.available() > 0) 
        {
          PH_ch = Serial1.read();  //gets one byte from serial buffer
          PH += PH_ch; //makes the string readString
        }
      }   
      unsigned int ph_i=PH.toInt();     
      float ph_f=float(ph_i)/1000;
      Serial.println(ph_f);
      user_mode.ph=ph_f;
      input2="";
    }
    else if (input2=='N')                                       //If button Pump ON Time is pressed
    { 
      Serial.println("Inside Pump ON case");
      String ON_Time;
      char ON_Time_ch;
      while(!Serial1.available());
      while (Serial1.available()) 
      {
        delay(3);  //delay to allow buffer to fill
        if (Serial1.available() > 0) 
        {
          ON_Time_ch = Serial1.read();  //gets one byte from serial buffer
          ON_Time += ON_Time_ch; //makes the string readString
        }
      }   
      unsigned int on_time_i=ON_Time.toInt();     
      Serial.println(on_time_i);
      //Set on time here
      input2="";
    }
    else if (input2=='F')                                       //If button Pump OFF Time is pressed
    { 
      Serial.println("Inside Pump OFF case");
      String OFF_Time;
      char OFF_Time_ch;
      while(!Serial1.available());
      while (Serial1.available()) 
      {
        delay(3);  //delay to allow buffer to fill
        if (Serial1.available() > 0) 
        {
          OFF_Time_ch = Serial1.read();  //gets one byte from serial buffer
          OFF_Time += OFF_Time_ch; //makes the string readString
        }
      }   
      unsigned int off_time_i=OFF_Time.toInt();     
      Serial.println(off_time_i);
      //Set off time here
      input2="";
    }

    else if (input2=='D')                                       //If Done button Pressed
    {

      set_val.ec=user_mode.ec;
      set_val.ph=user_mode.ph;
      //set_val.pump_on_time=;
      //set_val.pump_off_time=;
      selection1='R';
    }
    else if(input2=='S')                                       //If Back button Pressed
    {
      selection1='S';
    }    
    break; 
////////////////////////////////////////////////////////////////////IoT Config//////////////////////////////////////////      
    case 'I': 
      Serial.println("Inside IoT Configuration");
      char input3;  
      while(!Serial1.available());      
      if (Serial1.available() > 0) 
      {
        input3 = Serial1.read();  //gets one byte from serial buffer
      }
      //iot_data_to_disp();
        ///////////////////////////////////////////////////////////////////
      if(input3=='N')                                     //Set User Name
      {
        Serial.println("Set Network Name");
        String Network_ID;
        char Network_ch;
        while(!Serial1.available());
        while (Serial1.available()) 
        {
          delay(3);  //delay to allow buffer to fill
          if (Serial1.available() > 0) 
          {
            Network_ch = Serial1.read();  //gets one byte from serial buffer
            Network_ID += Network_ch; //makes the string readString
          }
        }       
        
        Serial.println(Network_ID);
        IoT.ID=Network_ID;
        Network_ID="";
        input3="";        
      }
      /////////////////////////////////////////////////////////////////// 
      else if(input3=='P')                                //Set Password
      {
        Serial.println("Set Network Password");
        String Password;
        char Password_ch;
        while(!Serial1.available());
        while (Serial1.available()) 
        {
          delay(3);  //delay to allow buffer to fill
          if (Serial1.available() > 0) 
          {
            Password_ch = Serial1.read();  //gets one byte from serial buffer
            Password += Password_ch; //makes the string readString
          }
        }       
        Serial.println(Password);
        IoT.PSSWD=Password;
        Password="";
        input3="";
      }
      ///////////////////////////////////////////////////////////////////
      else if (input3=='W')                               //Set Write API
      {
        Serial.println("Set Write API");
        String W_API;
        char W_API_ch;
        while(!Serial1.available());
        while (Serial1.available()) 
        {
          delay(3);  //delay to allow buffer to fill
          if (Serial1.available() > 0) 
          {
            W_API_ch = Serial1.read();  //gets one byte from serial buffer
            W_API += W_API_ch; //makes the string readString
          }
        }       
        Serial.println(W_API);
        IoT.API=W_API;
        W_API="";
        input3="";
      }
       ///////////////////////////////////////////////////////////////////
      else if (input3=='D')                               //Done
      {
        Serial.print("SSID=");
        Serial.println(IoT.ID);        
        Serial.print("PSSWD=");
        Serial.println(IoT.PSSWD);
        Serial.print("API=");
        Serial.println(IoT.API);     
        //Store ssid password and api to the eeprom and try to connect with the network
        selection1='S';
        send_ssid();
        send_psswd();
        send_api();
        get_iot_param();                  //Read from EEPROM (Newly set configuration)
        init_esp();                       //Connect esp with cloud
      }
      else if(input3=='S')
      {
        selection1='S';  
        }        
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////      
    case 'R':                                           //Running mode (run for  Auto, user and manual modes)
      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      ec_pump();
      ph_pump();     
      data_to_disp();
      
      Serial.println("Inside Running Mode");
      if (Serial1.available() > 0) 
          if(Serial1.read()=='S')
          {selection1='S';} 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////Added for IoT///////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    String sendData = "GET /update?api_key="+ API +"&"+ myFIELD +"="+String(sendVal);     //Send data on single field                                                       //////
    count_tx_time++;
    if(count_tx_time==6)
    {
      
    count_tx_time=0;
    String sendData = "GET /update?api_key="+ API +"&field1="+String(ecValue)+",&field2="+String(phValue)+",&field3="+String(Celcius);   //Send data on multiple fields       //////
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections                                                                                                   //////
    espData("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT, 1000, DEBUG);                                                                                                      //////
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);                                                                                                         //////
    espSerial.find(">");                                                                                                                                                      //////
    espSerial.println(sendData);                                                                                                                                              //////                            
    espData("AT+CIPCLOSE=0",1000,DEBUG);                                                                                                                                      //////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    } //delay(1000);
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    default:
      Serial.println("Default Case");
      selection1="";
      //Serial.println("At Home");
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_ssid()       //Data from display to EEPROM
{ 
  for(int i=0;i<20;i++)     //Store wifi ID into EEPROM location starting from 0 to 19 location
  {
    if(i<IoT.ID.length())
    {
    EEPROM.write(i,IoT.ID[i]);    //wifi name to EEPROM
    }
    else                          
    {
      EEPROM.write(i,'\n');      //Name store in starting location, rest location (till 19th) filled with \n escape 
      }
    }  
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_psswd()       //Data from display to EEPROM
{ 
  for(int i=20;i<40;i++)        //Store wifi password into EEPROM starting from 20 to 39 location
  {
    if((i-20)<IoT.PSSWD.length())   
    {
    EEPROM.write(i,IoT.PSSWD[i-20]);    //wifi password to EEPROM
    }
    else
    {
      EEPROM.write(i,'\n');      //Password store in starting location, rest location (till 39th) filled with \n escape 
      }
    }  
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void send_api()       //Data from display to EEPROM
{
  for(int i=40;i<70;i++)            //Store Thingspeak Write API into EEPROM starting from 40 to 69th location
  {
    if((i-40)<IoT.API.length())
    {
    EEPROM.write(i,IoT.API[i-40]);  //Write API to EEPROM
    }
    else
    {
      EEPROM.write(i,'\n');        //Write API store in starting location, rest location (till 69th) filled with \n escape 
      }
    }   
}

/////////////////////////////////////////////////////Get Network Data Stored in EEPROM/////////////////////////////////
void get_iot_param()
{
  ID="";
  char read_ch;
  for(int i=0;i<20;i++)                         //Get wifi name from 0 to 19th location of EEPROM
  {
    read_ch=EEPROM.read(i);
    if(read_ch!='\n')
    {
      ID +=read_ch;
      }
    }
    Serial.print("SSID from EEPROM=");
    Serial.println(ID);

  PSSWD="";
  for(int i=20;i<40;i++)                         //Get wifi password from 20 to 39th location of EEPROM
  {
    read_ch=EEPROM.read(i);
    if(read_ch!='\n')
    {
      PSSWD +=read_ch;
      }
    }    
    Serial.print("PSSWD from EEPROM=");
    Serial.println(PSSWD);


   API="";
  for(int i=40;i<70;i++)                         //Get ThingSpeak Write API from 40 to 69th location of EEPROM
  {
    read_ch=EEPROM.read(i);
    if(read_ch!='\n')
    {
      API +=read_ch;
      }
    }    
    Serial.print("API from EEPROM=");
    Serial.println(API);   
  
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
  
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
    //Serial.print(response);
  }
  return response;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_ec_val()
{
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    voltageEC = analogRead(EC_PIN) / 1024.0 * 5000; // read the voltage
    temperature = readTemperature();          // read your temperature sensor to execute temperature compensation
    ecValue =  ec.readEC(voltageEC, temperature); // convert voltage to EC with temperature compensation
    //      Serial.print("temperature:");
    //      Serial.print(Celcius,1);
    Serial.print(" EC:");
    Serial.print(ecValue, 2);
    Serial.println("ms/cm");
  }
  ec.calibration(voltageEC, temperature);
  return(ecValue);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_ph_val()
{
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) {            //time interval: 1s
    timepoint = millis();
    temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
    voltagePH = analogRead(PH_PIN) / 1024.0 * 5000; // read the voltage
    phValue = ph.readPH(voltagePH, temperature); // convert voltage to pH with temperature compensation
    //        Serial.print("temperature:");
    //        Serial.print(Celcius,1);
    Serial.print(" pH:");
    Serial.println(phValue, 2);
  }
  ph.calibration(voltagePH, temperature);
  return(phValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
  sensors.requestTemperatures();
  Celcius = sensors.getTempCByIndex(0);
  //Fahrenheit=sensors.toFahrenheit(Celcius);
  Serial.print("Temp  ");
  Serial.print(Celcius);
  Serial.print(":°C  ");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ec_pump()
{
  if (current_val.ec > set_val.ec)
  {
    digitalWrite(relay_pin1, LOW); //EC PumpA ON
    digitalWrite(relay_pin2, LOW); //EC PumpB ON
    NutrientA = "ON";
    NutrientB = "ON";
    Serial.println(NutrientA);
    Serial.println(NutrientB);
  }
  else
  {
    digitalWrite(relay_pin1, HIGH); // EC PumpA OFF
    digitalWrite(relay_pin2, HIGH); //EC PumpB OFF
    NutrientA = "OFF";
    NutrientB = "OFF";
    Serial.println(NutrientA);
    Serial.println(NutrientB);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ph_pump()
{
  if (current_val.ph > set_val.ph)
  {
    digitalWrite(relay_pin3, LOW); //EC Pump ON
    NutrientC = "ON";
    Serial.println(NutrientC);
  }
  else
  {
    digitalWrite(relay_pin3, HIGH); // EC Pump OFF
    NutrientC = "OFF";
    Serial.println(NutrientC);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void data_to_disp()
{

  String command = "temp.txt=\"" + String(Celcius) + "\"";
  Serial1.print(command);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  String command3 = "ec.txt=\"" + String(ecValue, 2) + "\"";
  Serial1.print(command3);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command4 = "ph.txt=\"" + String(phValue, 2) + "\"";
  Serial1.print(command4);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command5 = "nutA.txt=\"" + String(NutrientA) + "\"";
  Serial1.print(command5);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command6 = "nutB.txt=\"" + String(NutrientB) + "\"";
  Serial1.print(command6);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command7 = "nutC.txt=\"" + String(NutrientC) + "\"";
  Serial1.print(command7);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  String command8 = "pump.txt=\"" + String(Pump_status) + "\"";
  Serial1.print(command8);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);




}

//
//void iot_data_to_disp()
//{
//  String command9 = "page5.t3.txt=\"" + String(ID) + "\"";
//  Serial1.print(command9);
//  Serial1.write(0xff);
//  Serial1.write(0xff);
//  Serial1.write(0xff);
//  
//  String command10 = "page5.t5.txt=\"" + String(PSSWD) + "\"";
//  Serial1.print(command10);
//  Serial1.write(0xff);
//  Serial1.write(0xff);
//  Serial1.write(0xff);
//
//  String command11 = "page5.t6.txt=\"" + String(API) + "\"";
//  Serial1.print(command11);
//  Serial1.write(0xff);
//  Serial1.write(0xff);
//  Serial1.write(0xff);  
//}

void init_esp()
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
//////////////////////////////////////////////////////////////Added By Narendra for IoT/////////////////////////////////////
  espSerial.begin(115200);                                                                                            //////  
  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module                                     //////
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode                             //////
  espData("AT+CWJAP=\""+ ID +"\",\""+ PSSWD +"\"", 1000, DEBUG);   //Connect to WiFi network                          //////
  /*while(!esp.find("OK"))                                                                                            //////
  {                                                                                                                   //////
      //Wait for connection                                                                                           //////
  }*/                                                                                                                 //////
  delay(1000);                                                                                                        //////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////TIMER-1 Initialization (Added By Narendra)/////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
void init_timer1()                                                                                                    ////  
{                                                                                                                     ////
//Freq 16Mhz or 16000000Hz                                                                                            ////
//Prescaler 1024                                                                                                      ////
//Effective freq 15625Hz                                                                                              ////
//Effective time 0.000064 Sec                                                                                         ////
//Max delay with TIMER normal mode 4.194304                                                                           ////
  TCCR1A=0x00;                                                                                                        ////
  TCCR1B|=(1<<CS10)|(1<<CS12);             //1024 Prescaler                                                            ////
  TCNT1=0x0000;                           //Initial Value for TIMER set at 0                                          ////
  //sei();                                //Global interrupt enable                                                   ////
  SREG=0b10000000;                      //Global interrupt enable                                                     ////
  TIMSK1=(1<<TOIE1);                     //TIMER Overflow Interrupt Enabled                                           ////
}                                                                                                                     ////  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////TIMER-1 Overflow ISR (Added By Narendra )/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(TIMER1_OVF_vect)                              //ISR executed after every 4.20 Seconds                                                                                     //////
{                                                                                                                                                                             //////
  count_interrupt++;                                                                                                                                                          //////
//  Serial.print("Counter value");
//  Serial.println(count_interrupt);
  
  if(count_interrupt==1)                                                                                                                                                      //////
  {                                                                                                                                                                           //////
    digitalWrite(relay_pin4,LOW);                 //Turn On                                                                                                                   //////
    Pump_status="ON";
  }                                                                                                                                                                           //////
  else if(count_interrupt==72)                    //After 72 Cycles (5 Min approx)                                                                                            //////
  {                                                                                                                                                                           //////
    digitalWrite(relay_pin4,HIGH);                //Turn off                                                                                                                  //////
    Pump_status="OFF";
  }                                                                                                                                                                           //////
  else if(count_interrupt==215)                   //After 215 Cycles(Total 15 min and 10 min from turn off), again turn on with count reset                                   //////
  {                                                                                                                                                                           //////
    count_interrupt=0;                                                                                                                                                        //////
  }                                                                                                                                                                           //////
}                                                                                                                                                                             //////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
