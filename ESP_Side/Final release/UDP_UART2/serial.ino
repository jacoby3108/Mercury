

// =============Serial =========================
//https://www.arduino.cc/en/Reference/Serial
void setup_serial(void)
{

 Serial.begin(9600, SERIAL_8N1);  // Serial Port Parameters
 Serial.setTimeout(20);           // Recieve Serial port Timeout in miliseg
 Serial.swap();                   // Use alternate serial pins (disconect from USB-uart pins and connect to alternate pins) 
  
}


//==================Send UDP data to Kinetis=====================================
int Send_Serial(void)
{

int i;
char data;

      i=0;
      
      Serial.write(STX);               // Start of Frame
      while(data=incomingPacket[i++])  // send recieved Udp Data to Kinetis via UART
         Serial.write(data);
      Serial.write(ETX);               // End of frame 
  
}

//==================Parse UART incomming Data from Kinetis=====================================

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
