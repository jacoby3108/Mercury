/**************************************************************
* UART Test code
* This code waits for a 0x04 from Kinetis and replies
* with the following bytes 0x00,0x01,0x02,0x03.
*
**************************************************************/



void setup()
{

 Serial.begin(9600, SERIAL_8N1);
 Serial.swap();                   // Use alternate serial pins (disconect from USB-uart pins and connect to alternate pins) 
 
}

void loop()
{
  
 byte serin=0;

  
  if (Serial.available()>0)
  {
    serin=Serial.read();
  
  }
  
  if(serin==0x04)
  {
    Serial.write(0);
    Serial.write(1);
    Serial.write(2);
    Serial.write(3);
    serin=0;

   
  }

  delay(100);
  

 
}



