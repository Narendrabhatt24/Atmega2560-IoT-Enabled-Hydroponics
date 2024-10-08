#include <OneWire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"
#include <EEPROM.h>

#include <SoftwareSerial.h>                           //Software Serial library                                                      
SoftwareSerial espSerial(50, 49);                     //Pin 50 and 49 act as RX and TX. Connect them to TX and RX of ESP8266     
#define DEBUG true                                                                                                

//#define DEBUGGING_ON                               //Macro to turn ON/OFF the Serial print of data on computer screen via Serial communication

String HOST = "api.thingspeak.com";                                                                               //////
String PORT = "80"; 
struct{String ID;String PSSWD;String API; }IoT;     //Data from display store stored here 
String ID, PSSWD, API;                              //Data from EEPROM store here

unsigned int count_tx_time=0;                       //counting for pushing data to the cloud
boolean running_status=0;                           //It control the data transmission to the display (inside the timer interrupt) 
String esp_response;                                //to store the status if button is pressed in between the esp process
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EC_PIN A0                                  //Connect EC sensor at A0 PIN
#define PH_PIN A1                                  //Connect pH sensor at A1 PIN
#define ONE_WIRE_BUS 2                             //Connect Temperature Sensor at digital pin -2 

float  voltagePH, voltageEC, phValue, ecValue, temperature;
DFRobot_PH ph;
DFRobot_EC ec;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float Celcius = 0;
float Fahrenheit = 0;

////////////////////////////////////////Structure for holding the data for different crops//////
struct sensor_stat{
  float ec;
  float ph;
  unsigned int pump_on_time;
  unsigned int pump_off_time;
  };

sensor_stat auto_mode=    { 1.9,  6.2,  300,  900   };
sensor_stat lettuce=      { 1.5,  6.25, 300,  1200  };
sensor_stat corriender=   { 1.5,  6.1,  300,  1200  };
sensor_stat cauliflower=  { 1.9,  6.75, 300,  600   };
sensor_stat spanich=      { 2.05, 6.6,  300,  600   };
sensor_stat broccoli=     { 1.9,  6.4,  300,  900   };
sensor_stat chives=       { 1.7,  6.25, 300,  900   };
sensor_stat chilli=       { 2.3,  6.05, 300,  1200  };
sensor_stat strawberry=   { 2.15, 6.25, 300,  900   };
sensor_stat user_mode;
sensor_stat set_val;
sensor_stat current_val;
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
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

unsigned int count_interrupt=0;       //To count number of interrupts with TIMER overflow

/////////////////////////////////////////////////////////////////////////////////////////////////// Void Setup Function//////////////////////////////
void setup() {
  Serial1.begin(9600);    //Connect Display

#ifdef DEBUGGING_ON
  Serial.begin(9600);     //Serial monitor
#endif

  get_iot_param();        //Get Wifi name, password and Thingspeak API from EEPROM 
  init_esp();             //start esp communication
  ////////////////////////////////////////////////////////////////////////////////
  pinMode(relay_pin1, OUTPUT);
  pinMode(relay_pin2, OUTPUT);
  pinMode(relay_pin3, OUTPUT);
  pinMode(relay_pin4, OUTPUT);
  digitalWrite(relay_pin1, HIGH);
  digitalWrite(relay_pin2, HIGH);
  digitalWrite(relay_pin3, HIGH);
  digitalWrite(relay_pin4, HIGH);

  TCCR1A=0X00;              //Turn TIMER off and all relays off
  TCCR1B=0x00;
  count_interrupt=0;

  ph.begin();
  ec.begin();   
  selection1='S';
  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  
    while (Serial1.available()) 
    {
      delay(3);  //delay to allow buffer to fill
      if (Serial1.available() > 0) 
      {
        selection = Serial1.read();  //gets one byte from serial buffer
        readString += selection; //makes the string readString
        }
      }
    #ifdef DEBUGGING_ON
      Serial.println(readString);
      Serial.println(selection1);
    #endif
    readString = "";
    if(selection=='A' || selection== 'M' || selection=='U' || selection=='I' || selection =='S')      //A = Auto | M = Manual | U = User | I = IoT Configuration | S = Back to Home
    { 
      running_status=0;
      selection1=selection;
      selection="";
      }
    switch (selection1)
    {
      case 'A':                             //Auto mode
        set_val.ec=auto_mode.ec;
        set_val.ph=auto_mode.ph;
        set_val.pump_on_time=auto_mode.pump_on_time;
        set_val.pump_off_time=auto_mode.pump_off_time;     
        init_timer1();                             // Initialize TIMER 
        
        #ifdef DEBUGGING_ON     
          Serial.println("Auto Mode selected");      // Automatic Mode Selected
        #endif     
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
        #ifdef DEBUGGING_ON  
          Serial.println(readString);
          Serial.println(input);
        #endif
        switch (input)
        {
          case 'A':                                                     //Broccoli
            #ifdef DEBUGGING_ON  
              Serial.println(" Broccoli selected");   
            #endif
            set_val.ec=broccoli.ec;
            set_val.ph=broccoli.ph;
            set_val.pump_on_time=broccoli.pump_on_time;
            set_val.pump_off_time=broccoli.pump_off_time;      
            init_timer1();
            selection1='R';
            break;
            
          case 'B':                                                     //Cauliflower
            #ifdef DEBUGGING_ON  
              Serial.println(" Cauliflower selected"); 
            #endif  
            set_val.ec=cauliflower.ec;
            set_val.ph=cauliflower.ph;  
            set_val.pump_on_time=cauliflower.pump_on_time;
            set_val.pump_off_time=cauliflower.pump_off_time;     
            init_timer1();
            selection1='R';
            break;
            
          case 'C':                                                       //Chilli
            #ifdef DEBUGGING_ON  
              Serial.println(" Chilli selected"); 
            #endif  
            set_val.ec=chilli.ec;
            set_val.ph=chilli.ph; 
            set_val.pump_on_time=chilli.pump_on_time;
            set_val.pump_off_time=chilli.pump_off_time;     
            init_timer1();
            selection1='R';
            break;
            
          case 'D':                                                        //Chives
            #ifdef DEBUGGING_ON  
              Serial.println(" Chives selected");   
            #endif    
            set_val.ec=chives.ec;
            set_val.ph=chives.ph;
            set_val.pump_on_time=chives.pump_on_time;
            set_val.pump_off_time=chives.pump_off_time;     
            init_timer1();
            selection1='R';
            break;
            
          case 'E':     //Corriender
            #ifdef DEBUGGING_ON  
              Serial.println(" Corriender selected"); 
            #endif
            set_val.ec=corriender.ec;
            set_val.ph=corriender.ph;
            set_val.pump_on_time=corriender.pump_on_time;
            set_val.pump_off_time=corriender.pump_off_time;     
            init_timer1(); 
            selection1='R';
            break;
            
          case 'F':     //Lettuce
            #ifdef DEBUGGING_ON  
              Serial.println(" Lettuce selected");  
            #endif
            set_val.ec=lettuce.ec;
            set_val.ph=lettuce.ph;
            set_val.pump_on_time=lettuce.pump_on_time;
            set_val.pump_off_time=lettuce.pump_off_time; 
            init_timer1();
            selection1='R';
            break;
            
          case 'G':     //Spanich
            #ifdef DEBUGGING_ON  
              Serial.println(" Spanich selected");  
            #endif
            set_val.ec=spanich.ec;
            set_val.ph=spanich.ph; 
            set_val.pump_on_time=spanich.pump_on_time;
            set_val.pump_off_time=spanich.pump_off_time;  
            init_timer1();
            selection1='R';
            break;
            
          case 'H':     //Strawberry
            #ifdef DEBUGGING_ON  
              Serial.println(" Strawberry selected"); 
            #endif
            set_val.ec=strawberry.ec;
            set_val.ph=strawberry.ph;
            set_val.pump_on_time=strawberry.pump_on_time;
            set_val.pump_off_time=strawberry.pump_off_time;  
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
          #ifdef DEBUGGING_ON  
            Serial.println("Inside User Mode");
          #endif
          char input2;  
          while(!Serial1.available());                //Wait for input from display  
          if (Serial1.available() > 0) 
          {
            input2 = Serial1.read();  //gets one byte from serial buffer
            }
          if(input2=='E')                                       //If button for EC Pressed
          {
            #ifdef DEBUGGING_ON  
              Serial.println("Inside EC Case");
            #endif
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
            #ifdef DEBUGGING_ON  
              Serial.println(ec_f);
            #endif
            user_mode.ec=ec_f;        
            input2="";
            }
          else if (input2=='P')                                       //If button for PH Pressed
          { 
            #ifdef DEBUGGING_ON  
              Serial.println("Inside PH Case");
            #endif
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
            #ifdef DEBUGGING_ON  
              Serial.println(ph_f);
            #endif
            user_mode.ph=ph_f;
            input2="";
            }
          else if (input2=='N')                                       //If button "Pump ON Time" is pressed
          { 
            #ifdef DEBUGGING_ON  
              Serial.println("Inside Pump ON case");
            #endif
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
            #ifdef DEBUGGING_ON  
              Serial.println(on_time_i);
            #endif
            user_mode.pump_on_time=on_time_i;
            input2="";
            }
          else if (input2=='F')                                       //If button "Pump OFF Time" is pressed
          { 
            #ifdef DEBUGGING_ON  
              Serial.println("Inside Pump OFF case");
            #endif
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
            #ifdef DEBUGGING_ON    
              Serial.println(off_time_i);
            #endif
            user_mode.pump_off_time=off_time_i;
            input2="";
            }
          else if (input2=='D')                                       //If "Done button" Pressed
          {
            set_val.ec=user_mode.ec;
            set_val.ph=user_mode.ph;
            set_val.pump_on_time=user_mode.pump_on_time;
            set_val.pump_off_time=user_mode.pump_off_time;       
            init_timer1();
            selection1='R';
            }
          else if(input2=='S')                                       //If Back button Pressed
          {
            selection1='S';
            }    
          break; 
////////////////////////////////////////////////////////////////////IoT Config//////////////////////////////////////////      
        case 'I': 
          #ifdef DEBUGGING_ON  
            Serial.println("Inside IoT Configuration");
          #endif
          char input3;  
          while(!Serial1.available());      
          if (Serial1.available() > 0) 
          {
            input3 = Serial1.read();  //gets one byte from serial buffer
            }
        ///////////////////////////////////////////////////////////////////
          if(input3=='N')                                     //Set User Name
          {
            #ifdef DEBUGGING_ON  
              Serial.println("Set Network Name");
            #endif
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
            #ifdef DEBUGGING_ON             
              Serial.println(Network_ID);
            #endif
            IoT.ID=Network_ID;                      //data from display saved to variable in mcu
            Network_ID="";
            input3="";        
            }
      /////////////////////////////////////////////////////////////////// 
          else if(input3=='P')                                //Set Password
          {
            #ifdef DEBUGGING_ON  
              Serial.println("Set Network Password");
            #endif
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
            #ifdef DEBUGGING_ON         
              Serial.println(Password);
            #endif
            IoT.PSSWD=Password;
            Password="";
            input3="";
            }
      ///////////////////////////////////////////////////////////////////
          else if (input3=='W')                               //Set Write API
          {
            #ifdef DEBUGGING_ON  
              Serial.println("Set Write API");
            #endif
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
            #ifdef DEBUGGING_ON        
              Serial.println(W_API);
            #endif
            IoT.API=W_API;
            W_API="";
            input3="";
            }
       ///////////////////////////////////////////////////////////////////
          else if (input3=='D')                               //Done
          {
            #ifdef DEBUGGING_ON  
              Serial.print("SSID=");
              Serial.println(IoT.ID);        
              Serial.print("PSSWD=");
              Serial.println(IoT.PSSWD);
              Serial.print("API=");
              Serial.println(IoT.API);     
            #endif
            //Store ssid password and api to the eeprom and try to connect with the network
            send_ssid();
            send_psswd();
            send_api();
            get_iot_param();                  //Read from EEPROM (Newly set configuration)
            init_esp();                       //Connect esp with cloud
            selection1='S';
            }
          else if (input3=='R')
          {
            get_iot_param();                  //Read from EEPROM (Newly set configuration)   
            iot_data_to_disp();               //Write to screen
            selection1='I';                   //Keep case of IoT configuration
            }
      ///////////////////////////////////////////////////////////////////     
          else if(input3=='S')
          {
            selection1='S';  
            }        
          break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
        case 'S':                                         //for home screen (Reach via back button)
          Serial1.print("page 1");
          Serial1.write(0xff);
          Serial1.write(0xff);
          Serial1.write(0xff); 
          selection1="";
          //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          #ifdef DEBUGGING_ON  
            Serial.println("At Home");
          #endif
          digitalWrite(relay_pin1, HIGH);
          digitalWrite(relay_pin2, HIGH);
          digitalWrite(relay_pin3, HIGH);
          TCCR1A=0X00;              //Turn TIMER off and all relays off
          TCCR1B=0x00;
          count_interrupt=0;
          digitalWrite(relay_pin4, HIGH);
          //Pump_status="OFF";   
          break;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////     
        case 'R':                                           //Running mode (run for  Auto, user and manual modes)
          running_status=1;
          current_val.ec=get_ec_val();
          current_val.ph=get_ph_val();
          ec_pump();
          ph_pump();  
          //main_pump();   
          //data_to_disp();
          #ifdef DEBUGGING_ON  
            Serial.println("Inside Running Mode");
          #endif
          if (Serial1.available() > 0) 
              if(Serial1.read()=='S')
              {
                selection1='S';
                } 
          count_tx_time++;
          if(count_tx_time==12)
          {   
            count_tx_time=0;
            String sendData = "GET /update?api_key="+ API +"&field1="+String(ecValue)+",&field2="+String(phValue)+",&field3="+String(Celcius);   //Send data on multiple fields       //////
            esp_response= espData("AT+CIPMUX=1", 1000, DEBUG);       //Allow multiple connections                                                                                                   //////
            #ifdef DEBUGGING_ON  
              Serial.print("ESp response = ");
              Serial.println(esp_response);
            #endif
            if(esp_response=="0") 
            {
              break;
              }
            esp_response= espData("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT, 1000, DEBUG);                                                                                                      //////
            #ifdef DEBUGGING_ON  
              Serial.print("ESp response = ");
              Serial.println(esp_response);
            #endif
            if(esp_response=="0") 
            {
              break;
              }
            esp_response= espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);                                                                                                         //////
            #ifdef DEBUGGING_ON  
              Serial.print("ESp response = ");
              Serial.println(esp_response);
            #endif
            if(esp_response=="0") 
            {
              break;
              }
            espSerial.find(">");                                                                                                                                                      //////
            espSerial.println(sendData);                                                                                                                                              //////                            
            esp_response= espData("AT+CIPCLOSE=0",1000,DEBUG);                                                                                                                                      //////
            #ifdef DEBUGGING_ON  
              Serial.print("ESp response = ");
              Serial.println(esp_response);
            #endif
            if(esp_response=="0") 
            {
              break;
              }
            } 
            break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
        default:
          running_status=0;
          #ifdef DEBUGGING_ON  
            Serial.println("Default Case");
          #endif
          selection1="";
          //Serial.println("At Home");
          break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    }       //End of Selection1 Switch
}         //End of void loop


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
    #ifdef DEBUGGING_ON  
      Serial.print("SSID from EEPROM=");
      Serial.println(ID);
    #endif
  PSSWD="";
  for(int i=20;i<40;i++)                         //Get wifi password from 20 to 39th location of EEPROM
  {
    read_ch=EEPROM.read(i);
    if(read_ch!='\n')
    {
      PSSWD +=read_ch;
      }
    }  
   #ifdef DEBUGGING_ON    
     Serial.print("PSSWD from EEPROM=");
     Serial.println(PSSWD);
   #endif
   API="";
   for(int i=40;i<70;i++)                         //Get ThingSpeak Write API from 40 to 69th location of EEPROM
   {
    read_ch=EEPROM.read(i);
    if(read_ch!='\n')
    {
      API +=read_ch;
      }
    }    
   #ifdef DEBUGGING_ON  
    Serial.print("API from EEPROM=");
    Serial.println(API);   
   #endif
 }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
String espData(String command, const int timeout, boolean debug)
{
  #ifdef DEBUGGING_ON  
    Serial.print("AT Command ==> ");
    Serial.print(command);
    Serial.println("     ");
  #endif
  
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
      
      if (Serial1.available() > 0) 
      if(Serial1.read()=='S')
      {
        selection1='S';
        return("0");
        } 
    
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
    #ifdef DEBUGGING_ON  
      Serial.print(" EC:");
      Serial.print(ecValue, 2);
      Serial.println("ms/cm");
    #endif
    }
  ec.calibration(voltageEC, temperature);
  return(ecValue);
  }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float get_ph_val()
{
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) 
  {            //time interval: 1s
    timepoint = millis();
    temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
    voltagePH = analogRead(PH_PIN) / 1024.0 * 5000; // read the voltage
    phValue = ph.readPH(voltagePH, temperature); // convert voltage to pH with temperature compensation
    //        Serial.print("temperature:");
    //        Serial.print(Celcius,1);
    #ifdef DEBUGGING_ON  
      Serial.print(" pH:");
      Serial.println(phValue, 2);
    #endif
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
  #ifdef DEBUGGING_ON  
    Serial.print("Temp  ");
    Serial.print(Celcius);
    Serial.print(":°C  ");
  #endif
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
    #ifdef DEBUGGING_ON  
      Serial.println(NutrientA);
      Serial.println(NutrientB);
    #endif
    }
  else
  {
    digitalWrite(relay_pin1, HIGH); // EC PumpA OFF
    digitalWrite(relay_pin2, HIGH); //EC PumpB OFF
    NutrientA = "OFF";
    NutrientB = "OFF";
    #ifdef DEBUGGING_ON  
      Serial.println(NutrientA);
      Serial.println(NutrientB);
    #endif
    }
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ph_pump()
{
  if (current_val.ph > set_val.ph)
  {
    digitalWrite(relay_pin3, LOW); //PH Pump ON
    NutrientC = "ON";
    #ifdef DEBUGGING_ON  
      Serial.println(NutrientC);
    #endif
    }
  else
  {
    digitalWrite(relay_pin3, HIGH); // PH Pump OFF
    NutrientC = "OFF";
    #ifdef DEBUGGING_ON  
      Serial.println(NutrientC);
    #endif
    }
  }

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


void iot_data_to_disp()
{
  String command9 = "page5.t3.txt=\"" + String(ID) + "\"";
  Serial1.print(command9);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);
  
  String command10 = "page5.t5.txt=\"" + String(PSSWD) + "\"";
  Serial1.print(command10);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);

  String command11 = "page5.t6.txt=\"" + String(API) + "\"";
  Serial1.print(command11);
  Serial1.write(0xff);
  Serial1.write(0xff);
  Serial1.write(0xff);  
  }

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
/////////////////////////////////////////////TIMER-1 Initialization (Added By Narendra)/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
//void init_timer1()                                                                                                    ////  
//{                                                                                                                     ////
////Freq 16Mhz or 16000000Hz                                                                                            ////
////Prescaler 1024                              | 256                                                                   ////
////Effective freq 15625Hz                      |                                                                       ////
////Effective time 0.000064 Sec                 |0.000016                                                               ////
////Max delay with TIMER normal mode 4.194304   |1.04856                                                                ////
//  TCCR1A=0x00;                                                                                                        ////
////  TCCR1B|=(1<<CS10)|(1<<CS12);             //1024 Prescaler                                                         ////
//  TCCR1B|=(1<<CS12);                         //256 Prescaler                                                          ////
//  TCNT1=0x0000;                             //Initial Value for TIMER set at 0                                        ////
//  //sei();                                  //Global interrupt enable                                                 ////
//  SREG=0b10000000;                          //Global interrupt enable                                                 ////
//  TIMSK1=(1<<TOIE1);                        //TIMER Overflow Interrupt Enabled                                        ////
//}                                                                                                                     ////  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////TIMER-1 CTC for 1 Sec Initialization //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
void init_timer1()                                                                                                    ////  
{                                                                                                                     ////
//Freq 16Mhz or 16000000Hz                                                                                            ////
//Prescaler 1024                              | 256                                                                   ////
//Effective freq 15625Hz                      |                                                                       ////
//Effective time 0.000064 Sec                 |0.000016                                                               ////
//Max delay with TIMER normal mode 4.194304   |1.04856                                                                ////
  TCCR1A=0x00;                                                                                                        ////
  TCCR1B|=(1<<WGM12)|(1<<CS10)|(1<<CS12);             //1024 Prescaler and CTC mode                                   ////
//  TCCR1B|=(1<<CS12);                         //256 Prescaler                                                          ////
  TCNT1=0x0000;                             //Initial Value for TIMER set at 0                                        ////
  OCR1A=15624;                              //Value to compare with TIMER
  //sei();                                  //Global interrupt enable                                                 ////
  SREG=0b10000000;                          //Global interrupt enable                                                 ////
  //TIMSK1=(1<<TOIE1);                        //TIMER Overflow Interrupt Enabled                                        ////
  TIMSK1=(1<<OCIE1A);
  }                                                                                                                     ////  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////TIMER-1 Overflow ISR (Added By Narendra )/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ISR(TIMER1_OVF_vect)                              //ISR executed after every 4.20 Seconds                                                                                   //////

ISR(TIMER1_COMPA_vect) 
{                                                                                                                                                                             //////                                                                                                    //////
  count_interrupt++;                                                                                                                                                          //////
//////////////////////////////////////////////////////////new function main_pump is added instead of executing it in ISR (lagging in showing status on screen)
  #ifdef DEBUGGING_ON  
    Serial.println(count_interrupt);
  #endif
  if(count_interrupt==1)                                                                                                                                                      //////
  {                                                                                                                                                                           //////
    digitalWrite(relay_pin4,LOW);                 //Turn On                                                                                                                   //////
    Pump_status="ON";
    }                                                                                                                                                                         //////
  else if(count_interrupt==(int(set_val.pump_on_time)+1))                    //After 72 Cycles (5 Min approx)                                                                 //////
  {                                                                                                                                                                           //////
    digitalWrite(relay_pin4,HIGH);                //Turn off                                                                                                                  //////
    Pump_status="OFF";
    }
  else if(count_interrupt==(set_val.pump_on_time+set_val.pump_off_time))                                                                                                      //////
  {                                                                                                                                                                           //////
    count_interrupt=0;                                                                                                                                                        //////
    }
  if(running_status==1)
  {  
    data_to_disp();
    }
  }                                                                                                                                                                           //////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
