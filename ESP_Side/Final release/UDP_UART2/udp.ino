void setup_UDP(void)
{

  Udp.begin(localUdpPort);
  debug_message("Now listening at IP %s UDP port %d\n", WiFi.localIP().toString().c_str(),localUdpPort);  
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


int Send_UDP_Data(void)
{
    // send back a reply, to the IP address and port we got the packet from
   
    Udp.beginPacket(Udp.remoteIP(),remoteUdpPort);
    Udp.write(replyPacekt,strlen(replyPacekt)+1);  //no usar la version Udp:write(string) No anda bien me hizo perder horas 
    Udp.endPacket();
  
    return(0);
  
}



