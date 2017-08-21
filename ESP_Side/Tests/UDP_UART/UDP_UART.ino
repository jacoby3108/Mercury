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

// WiFi credentials
char ssid[] = "IOT_WIFI";
char password[] = "GEDA2016";   // Set password to "" for open networks.


#define Board_LED D0          // AMICA OnBoadrd LED  (on board blue led same as D0 but inverted)
//#define Board_LED D4          // LOLIN OnBoadrd LED  (on board blue led same as D4 but inverted)
#define Board_LED_OFF  1
#define Board_LED_ON   0
int isledon=0;

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
char  replyPacekt[] = "Hi there! Got the message :-)\n";  // a reply string to send back
int Get_UDP_Status(void);
int Get_UDP_Data(void);

void setup()
{

 Serial.begin(9600, SERIAL_8N1);
 //Serial.swap();


 setup_gpios();         // initialize used GPIOS
 setup_wifi();          // initialize WIFI an connect to network
 setup_UDP();           // initialize UDP
 
 
 
}




void loop()
{
  
 int packetSize; 
  
    // receive incoming UDP packets

    if (packetSize=Get_UDP_Status())
    {
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
    

    Get_UDP_Data();
 
    
    debug_message("Received %d bytes from %s port %d\n",packetSize,Udp.remoteIP().toString().c_str(),Udp.remotePort());

    
    debug_message("UDP packet contents: %s\n",incomingPacket);

    Send_UDP_Data();
    
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

  int len = Udp.read(incomingPacket, 255);
  
  if (len > 0)
  {
      incomingPacket[len] = 0;
  }

     
}

int Send_UDP_Data(void)
{
    // send back a reply, to the IP address and port we got the packet from
   
    Udp.beginPacket(Udp.remoteIP(),remoteUdpPort);
    Udp.write(replyPacekt);
    Udp.endPacket();
  
    return(0);
  
}







void setup_gpios(void )
{
  // prepare GPIO's

  pinMode(Board_LED, OUTPUT);
  digitalWrite(Board_LED, Board_LED_OFF);
  isledon=0;
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
// https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/udp-class.rst







