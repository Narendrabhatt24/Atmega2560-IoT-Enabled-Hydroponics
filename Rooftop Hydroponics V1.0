#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"
#include <EEPROM.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////Added By Narendra for IoT/////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>       //Software Serial library                                                       //////
SoftwareSerial espSerial(50, 49);   //Pin 50 and 49 act as RX and TX. Connect them to TX and RX of ESP8266        //////
#define DEBUG true                                                                                                //////  
String WIFI_SSID = "Narendra";       // WiFi SSID                                                                 //////
String WIFI_PWD = "CDAC@123"; // WiFi Password                                                                    //////
String API = "Y4JXAWOZRXLR1V5M";   // API Key                                                                     //////
String HOST = "api.thingspeak.com";                                                                               //////
String PORT = "80";                                                                                               //////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
sensor_stat strawberry=   {1.5,6.5};
sensor_stat user_mode;
sensor_stat set_val;
sensor_stat current_val;

String NutrientA_OFF = String('Nutrient_A_OFF');
String NutrientB_OFF = String('Nutrient_B_OFF');
String NutrientC_OFF = String('Nutrient_C_OFF');


int relay_pin1 = 4;           //Nutrient A
int relay_pin2 = 5;           //Nutrient B
int relay_pin3 = 6;           //Nutrient C
int relay_pin4 = 7;           //Nutrient Pump Relay

String data_from_display = "";
String a;
String readString = "";

char selection;
char selection1;

//String abc;
unsigned int count_interrupt=0;       //To count number of interrupts with TIMER overflow (Added By Narendra to control nutrient pump)
char TIMER_Start_flag=1;                //Flag to start TIMER1

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
//////////////////////////////////////////////////////////////Added By Narendra for IoT/////////////////////////////////////
  espSerial.begin(115200);                                                                                            //////  
  espData("AT+RST", 1000, DEBUG);                      //Reset the ESP8266 module                                     //////
  espData("AT+CWMODE=1", 1000, DEBUG);                 //Set the ESP mode as station mode                             //////
  espData("AT+CWJAP=\""+ WIFI_SSID +"\",\""+ WIFI_PWD +"\"", 1000, DEBUG);   //Connect to WiFi network                //////
  /*while(!esp.find("OK"))                                                                                            //////
  {                                                                                                                   //////
      //Wait for connection                                                                                           //////
  }*/                                                                                                                 //////
  delay(1000);                                                                                                        //////
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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


void loop() 
{
    while (Serial1.available()) {
    delay(3);  //delay to allow buffer to fill
    if (Serial1.available() > 0) {
      selection = Serial1.read();  //gets one byte from serial buffer
      readString += selection; //makes the string readString
    }
  }
  Serial.println(readString);
  readString = "";
  /// }
  cropselect();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////Added By Narendra for IoT///////////////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    String sendData = "GET /update?api_key="+ API +"&"+ myFIELD +"="+String(sendVal);     //Send data on single field                                                       //////
    String sendData = "GET /update?api_key="+ API +"&field1="+String(ecValue)+",&field2="+String(phValue)+",&field3="+String(Celcius);   //Send data on multiple fields       //////
    espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections                                                                                                   //////
    espData("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT, 1000, DEBUG);                                                                                                      //////
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);                                                                                                         //////
    espSerial.find(">");                                                                                                                                                      //////
    espSerial.println(sendData);                                                                                                                                              //////                            
    espData("AT+CIPCLOSE=0",1000,DEBUG);                                                                                                                                      //////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  delay(1000);
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
  TCCR1B=(1<<CS10)|(1<<CS12);             //1024 Prescaler                                                            ////
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
  if(count_interrupt==1)                                                                                                                                                      //////
  {                                                                                                                                                                           //////
    digitalWrite(relay_pin4,LOW);                 //Turn On                                                                                                                   //////
  }                                                                                                                                                                           //////
  else if(count_interrupt==72)                    //After 72 Cycles (5 Min approx)                                                                                            //////
  {                                                                                                                                                                           //////
    digitalWrite(relay_pin4,HIGH);                //Turn off                                                                                                                  //////
  }                                                                                                                                                                           //////
  else if(count_interrupt==215)                   //After 215 Cycles(Total 15 min and 10 min from turn off), again turn on with count reset                                   //////
  {                                                                                                                                                                           //////
    count_interrupt=0;                                                                                                                                                        //////
  }                                                                                                                                                                           //////
}                                                                                                                                                                             //////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
void ec_pump()
{
  if (current_val.ec > set_val.ec)
  {
    digitalWrite(relay_pin1, LOW); //EC PumpA ON
    digitalWrite(relay_pin2, LOW); //EC PumpB ON
    NutrientA_OFF = "Nutrient_A_ON";
    NutrientB_OFF = "Nutrient_B_ON";
    Serial.println(NutrientA_OFF);
    Serial.println(NutrientB_OFF);
  }
  else
  {
    digitalWrite(relay_pin1, HIGH); // EC PumpA OFF
    digitalWrite(relay_pin2, HIGH); //EC PumpB OFF
    NutrientA_OFF = "Nutrient_A_OFF";
    NutrientB_OFF = "Nutrient_B_OFF";
    Serial.println(NutrientA_OFF);
    Serial.println(NutrientB_OFF);
  }
}

void ph_pump()
{
  if (current_val.ph > set_val.ph)
  {
    digitalWrite(relay_pin3, LOW); //EC Pump ON
    NutrientC_OFF = "Nutrient_C_ON";
    Serial.println(NutrientC_OFF);
  }
  else
  {
    digitalWrite(relay_pin3, HIGH); // EC Pump OFF
    NutrientC_OFF = "Nutrient_C_OFF";
    Serial.println(NutrientC_OFF);
  }
}


void automatic_mode()
{

  String command = "temp.txt=\"" + String(Celcius) + "\"";
  Serial1.print(command);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  String command3 = "EC.txt=\"" + String(ecValue, 2) + "\"";
  Serial1.print(command3);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command4 = "pH.txt=\"" + String(phValue, 2) + "\"";
  Serial1.print(command4);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command5 = "nutA.txt=\"" + String(NutrientA_OFF) + "\"";
  Serial1.print(command5);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command6 = "nutB.txt=\"" + String(NutrientB_OFF) + "\"";
  Serial1.print(command6);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  String command7 = "nutC.txt=\"" + String(NutrientC_OFF) + "\"";
  Serial1.print(command7);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
}

void cropselect()
{
  if (selection == 'A' || selection == 'M' || selection == 'C' || selection == 'D' || selection == 'E' || selection == 'F' || selection == 'G' || selection == 'H' || selection == 'I' || selection == 'J' || selection == 'K' || selection == 'O')
  {
    TIMER_Start_flag++;         //Get inremented to 2, initially 1
    if(TIMER_Start_flag==2)     //Condition to initialize TIMER1
    {
    init_timer1();            //Initialize TIMER1
    TIMER_Start_flag=0;       
    }
    else
    {
      TIMER_Start_flag=0;
    }
    
    selection1 = selection;
  }


  switch (selection1)
  {
    case 'A':                             //Auto mode
      set_val.ec=auto_mode.ec;
      set_val.ph=auto_mode.ph;
      Serial.println("Auto Mode selected");      // Automatic Mode Selected
      selection1='R';
      break;

    case 'M':                              //Manual Mode

      char input;  
      while(!Serial.available());
      
      while (Serial1.available()) {
        delay(3);  //delay to allow buffer to fill
        if (Serial1.available() > 0) {
          input = Serial1.read();  //gets one byte from serial buffer
          readString += selection; //makes the string readString
        }
      }
      Serial.println(readString);
      readString = "";

      switch (input)
      {
        case 'A':     //Broccoli
        case 'B':     //Cauliflower
        case 'C':     //Chilli
        case 'D':     //Chives
        case 'E':     //Corriender
        case 'F':     //Lettuce
        case 'G':     //Spanich
        case 'H':     //Strawberry
        case 'S':     //Back
        
        }

          
    case 'R':
      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();  
      ec_pump();
      ph_pump();     
      automatic_mode();
      break;

    case 'B':
      Serial.println("Mode1 selected");      // Manual Mode Selected
      break;

    case 'C':
      Serial.println("Mode3 selected");       // user Mode Selected
      break;

    case 'D':
      set_val.ec=lettuce.ec;
      set_val.ph=lettuce.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("lettuce selected");
      break;

    case 'E':
      set_val.ec=corriender.ec;
      set_val.ph=corriender.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("corriender selected");
      break;

    case 'F':
      set_val.ec=cauliflower.ec;
      set_val.ph=cauliflower.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("cauliflower selected");
      break;

    case 'G':
      set_val.ec=spanich.ec;
      set_val.ph=spanich.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("spanich selected");
      break;

    case 'H':
      set_val.ec=broccoli.ec;
      set_val.ph=broccoli.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("broccoli selected");
      break;

    case 'I':
      set_val.ec=chives.ec;
      set_val.ph=chives.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("chives selected");
      break;

    case 'J':
      set_val.ec=chilli.ec;
      set_val.ph=chilli.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("chilli selected");
      break;

    case 'K':
      set_val.ec=strawberry.ec;
      set_val.ph=strawberry.ph;

      current_val.ec=get_ec_val();
      current_val.ph=get_ph_val();
      
      ec_pump();
      ph_pump();
      automatic_mode();
      Serial.println("strawberry selected");
      break;

    case 'O':
      Serial.println("set pumpA selected");
      break;

    case 'M':
      Serial.println("set pumpB selected");
      break;

    case 'N':
      Serial.println("set pH selected");
      break;

    default:
      Serial.println("Mode10 selected");
      break;
  }
}


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
