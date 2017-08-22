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

//===In serial.ino file===
void setup_serial(void);



//========================



long lastMillis;
long currentMillis;
 int  timeout=0;

enum status {TRUE,FALSE};
///////////unsigned char arr[50];
enum status message_arrived=FALSE;

void setup()
{

 setup_serial();        // initialize Serial port
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

      // IMPORTTANTE !!! no reducir este delay es critico // 
      delay(80);                    // Avoid reading serial port status immediately after message was send to kinetis
                                    // It seems that Serial.available() works too slow so false readings can happen 

                                   

      // Wait data from kinetis

        
        if (Serial.available()>0)
        {
          
            timeout=0;
            currentMillis = millis();
            lastMillis=currentMillis; 
            
      
            do{
                parse_incomming_data();
                
                         
            }while( message_arrived!=TRUE && timeout!=1);

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
    
    //delay(100); //ok
    delay(5); //pruebas
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


