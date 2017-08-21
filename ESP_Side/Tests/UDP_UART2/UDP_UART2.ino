/**************************************************************
* UDP Test code
* This code listen on PORT (5000)
* Any time a message is receieved the on board led is toggled
* and the received messagge is print on terminal
* afterwards a reply message is sent to host on same port
*
**************************************************************/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <string.h>


// WiFi credentials
char ssid[] = "IOT_WIFI";
char password[] = "GEDA2016";   // Set password to "" for open networks.


#define Board_LED D0          // AMICA OnBoadrd LED  (on board blue led same as D0 but inverted)
//#define Board_LED D4          // LOLIN OnBoadrd LED  (on board blue led same as D4 but inverted)
#define Board_LED_OFF  1
#define Board_LED_ON   0


//////  #define eprintf(format,...) Serial.printf (format, ##__VA_ARGS__)
// more info see Variadic Macros  https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

#define DEBUG_OFF  0
#define DEBUG_ON   1

#define debug DEBUG_OFF

#define debug_message(fmt,...)          \
  do {              \
    if (debug)          \
       Serial.printf (fmt, ##__VA_ARGS__);     \
  } while (0)


#define debug_message_2(fmt)          \
  do {              \
      if (debug)          \
      Serial.print(">>> "); \
        Serial.printf (fmt, ##__VA_ARGS__);     \
      if (debug)          \
        Serial.print(" <<<<\n"); \
  } while (0)



// ================================== UDP Stuff =============================================
WiFiUDP Udp;
unsigned int localUdpPort = 5000;  // local port to listen on
unsigned int remoteUdpPort= 5000;  // Remote port to wite to
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[20];  // a reply string to send back
int Get_UDP_Status(void);
int Get_UDP_Data(void);
int Send_UDP_Data(void);
void setup_UDP(void);

#define STX 0x02
#define ETX 0x03
#define ENQ 0x05
#define ACK 0x06
#define NACK 0x15

void setup_gpios(void);
void setup_wifi(void);
void toggle_led(void);
int Send_Serial(void);


long lastMillis;
long currentMillis;
 int  timeout=0;

enum status {TRUE,FALSE};
///////////unsigned char arr[50];
enum status message_arrived=FALSE;

void setup()
{

 Serial.begin(9600, SERIAL_8N1);  // Serial Port Parameters
 Serial.setTimeout(20);         // Recieve Serial port Timeout in miliseg
 Serial.swap();                   // Use alternate serial pins (disconect from USB-uart pins and connect to alternate pins) 


 setup_gpios();         // initialize used GPIOS
 setup_wifi();          // initialize WIFI an connect to network
 setup_UDP();           // initialize UDP
 
 
 
}




void loop()
{
  
 int packetSize; 
 byte serin=0;

  
   

    if (packetSize=Get_UDP_Status())  // if UDP message was received 
    {
          
     
      if(Get_UDP_Data()>0)                 // get received data
      {
        Send_Serial();                         //and sent it to kinetis
      
      
      
      //debug_message("Received %d bytes from %s port %d\n",packetSize,Udp.remoteIP().toString().c_str(),Udp.remotePort());
      //debug_message("UDP packet contents: %s\n",incomingPacket);
   
      // preload response with NAK in case that no response

      strcpy(replyPacekt,"NACK");

      



     
      delay(50);                    // Avoid reading serial port status immediately after message was send to kinetis
                                    // It sems that Serial.available() works too slow so false readings can happen 

      // Wait data from kinetis

        
        if (Serial.available()>0)
        {
          
            timeout=0;
            currentMillis = millis();
            lastMillis=currentMillis; 
            
      
            do{
                parse_incomming_data();
                
                         
            }while( message_arrived!=TRUE && timeout!=1);
//while( message_arrived!=TRUE && timeout!=1);


            if(timeout==1)
            {
                strcpy(replyPacekt,"Timeout0");
                toggle_led();
                timeout==0;  
                        
            }
         
        }
      
     Send_UDP_Data();

    } //end if UDP message was received 

   }// 
    
    delay(100);
 
}

//==================Parse UART incomming Data=====================================

void parse_incomming_data(void)
{
enum STATE {S_ILDE,S_GET_DATA};
static enum STATE rx_state=S_ILDE;
unsigned char data;

static int i=0;

        currentMillis = millis();
        if ((currentMillis - lastMillis) > 20  )
        {
                       
               timeout=1;
               rx_state=S_ILDE;
 
               return;         
        } 

       //   data=Serial.read();
        Serial.readBytes(&data,1);
        
    
        if(rx_state==S_ILDE)
        {
          if(data==STX)
          {
            rx_state=S_GET_DATA;
            i=0;
            message_arrived=FALSE;

          }

          return;
        }

        if(rx_state==S_GET_DATA)
        {
          if(data==ETX)
          {
            rx_state=S_ILDE;
            replyPacekt[i++]=0x00;
            message_arrived=TRUE;

            return;
          }

          replyPacekt[i++]=data;


        }



}


//=================== UDP =============================

int Get_UDP_Status(void)
{

  int packetSize = Udp.parsePacket();
  
    return(packetSize);

  
}

int Get_UDP_Data(void)
{
  int i;
  
  int len = Udp.read(incomingPacket, 255);
  
  if (len > 0)
  {
      incomingPacket[len] = 0;
  }

  return len;     
}

int Send_Serial(void)
{

int i;
char data;

      i=0;
      
      Serial.write(STX);
      while(data=incomingPacket[i++])
         Serial.write(data);
      Serial.write(ETX);
  
}



int Send_UDP_Data(void)
{
    // send back a reply, to the IP address and port we got the packet from
   
    Udp.beginPacket(Udp.remoteIP(),remoteUdpPort);
    Udp.write(replyPacekt,strlen(replyPacekt)+1);  //no usar la version Udp:write(string) No anda bien me hizo perder horas 
    Udp.endPacket();
  
    return(0);
  
}







void setup_gpios(void)
{
  // prepare GPIO's

  pinMode(Board_LED, OUTPUT);
  digitalWrite(Board_LED, Board_LED_OFF);
  
}


void setup_wifi(void) {

// Start connecting to a WiFi network
 
  debug_message("\n\nConnecting to %s\n",ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    debug_message(".");
  }

  debug_message("\nWiFi Connected!!  IP Address:%s \n\n",WiFi.localIP().toString().c_str() );

}

void setup_UDP(void)
{

  Udp.begin(localUdpPort);
  debug_message("Now listening at IP %s UDP port %d\n", WiFi.localIP().toString().c_str(),localUdpPort);  
}

// References:
// ================ UDP Sample ==============
// https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/udp-examples.rst
// ================ UDP API =================
// https://www.arduino.cc/en/Reference/EthernetUDPWrite

void toggle_led(void)
{
  static int isledon=0;
  
      if(isledon)
      {
         digitalWrite(Board_LED, Board_LED_OFF);
        isledon=0;
      }
      else
      {
         digitalWrite(Board_LED, Board_LED_ON);
        isledon=1;
      }

}

// =============Serial =========================
//https://www.arduino.cc/en/Reference/Serial

