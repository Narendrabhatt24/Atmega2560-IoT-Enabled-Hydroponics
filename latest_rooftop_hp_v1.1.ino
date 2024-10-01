#include <EEPROM.h>

String readString;
char selection, selection1;

struct{String ID;String PSSWD;String API; }IoT; //Data from display store stored here
String ID, PSSWD, API;          //Data from EEPROM store here

void setup() {
  Serial1.begin(9600);    //Connect Display
  Serial.begin(9600);   
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
  
  if(selection=='A' || selection== 'M' || selection=='U' || selection=='I' || selection =='S')      //A = Auto | M = Manual | U = User | I = IoT Configuration | S = Back to Home
  {
  selection1=selection;
  selection="";
  }

 switch (selection1)
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    case 'A':                             //Auto mode
      //set_val.ec=auto_mode.ec;
      //set_val.ph=auto_mode.ph;
      //set pump on and off time
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
            selection1='R';
            break;
        case 'B':     //Cauliflower
            Serial.println(" Cauliflower selected");      
            selection1='R';
            break;
        case 'C':     //Chilli
            Serial.println(" Chilli selected");     
            selection1='R';
            break;
        case 'D':     //Chives
            Serial.println(" Chives selected");   
            selection1='R';
            break;
        case 'E':     //Corriender
            Serial.println(" Corriender selected");      
            selection1='R';
            break;
        case 'F':     //Lettuce
            Serial.println(" Lettuce selected");  
            selection1='R';
            break;
        case 'G':     //Spanich
            Serial.println(" Spanich selected");      
            selection1='R';
            break;
        case 'H':     //Strawberry
            Serial.println(" Strawberry selected"); 
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
      input2="";
    }

    else if (input2=='D')                                       //If Done button Pressed
    {
      //set ec and ph values entered by the user as set value
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
      else if (input3=='S')                               //Back to home page
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
        get_iot_param();
      }        
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////      
    case 'R':                                           //Running mode (run for  Auto, user and manual modes)
      Serial.println("Inside Running Mode");
      delay(2000);
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    case 'S':                                         //for home screen (Reach via back button)
      //selection1="";
      Serial.println("At Home");
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    default:
      selection1="";
      //Serial.println("At Home");
      break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////  

}
}



void send_ssid()       //Data from display to EEPROM
{ 
  for(int i=0;i<20;i++)
  {
    if(i<IoT.ID.length())
    {
    EEPROM.write(i,IoT.ID[i]);
    }
    else
    {
      EEPROM.write(i,'\n');
      }
    }  
}

void send_psswd()       //Data from display to EEPROM
{ 
  for(int i=20;i<40;i++)
  {
    if((i-20)<IoT.PSSWD.length())
    {
    EEPROM.write(i,IoT.PSSWD[i-20]);
    }
    else
    {
      EEPROM.write(i,'\n');
      }
    }  
}

void send_api()       //Data from display to EEPROM
{
  for(int i=40;i<70;i++)
  {
    if((i-40)<IoT.API.length())
    {
    EEPROM.write(i,IoT.API[i-40]);
    }
    else
    {
      EEPROM.write(i,'\n');
      }
    }   
}

/////////////////////////////////////////////////////Get Network Data Stored in EEPROM/////////////////////////////////
void get_iot_param()
{
  ID="";
  char read_ch;
  for(int i=0;i<20;i++)
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
  for(int i=20;i<40;i++)
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
  for(int i=40;i<70;i++)
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
