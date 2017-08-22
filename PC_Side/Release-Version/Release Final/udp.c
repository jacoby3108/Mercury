/* Joystick over UDP
 * 
 * by dany
 * 
 *  */

//  NO olvidar poner el JoyStick en ANALOG !!!!!!!!!!!!!!!!!!!!!!!

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <unistd.h> // close();
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>

// joystick
#include "joyapi.h"
#include <linux/input.h>
#include <linux/joystick.h>


#define SERVER "192.168.0.100"     // esp8266 address
#define PORT 5000   				//The port on which to send data


#define BUFLEN 512  //Max length of buffer
#define  ONE_MS    	1000	   // One mseg
#define  time_delay(x)  (x/100)    // x in ms , minvalue 100mseg



// ============ Colored Text ===========

#define BLACK_TEXT		   "\e[30;1m"
#define RED_TEXT		   "\e[31;1m"
#define GREEN_TEXT 		   "\e[32;1m"
#define YELLOW_TEXT 	   "\e[33;1m"
#define BLUE_TEXT 		   "\e[34;1m"
#define MAGENTA_TEXT 	   "\e[35;1m"
#define CYAN_TEXT 	   	   "\e[36;1m"
#define WHITE_TEXT 		   "\e[37m"

// ============ Colored Text End ===========

	
//http://linuxprograms.wordpress.com/2007/12/29/threads-programming-in-linux-examples/

int receive_UDP(char *);
int send_UDP(char *);

char *message2send="hola24";

char send_buf[BUFLEN];
char rcv_buf[BUFLEN];

static js_event_t js;

char joy_message[100];

void mainTB1(void);
void main_joy(void);

void parse_joy_string(void);

void main(void)
{
	
//	mainTB1();
	main_joy();
	
	
}

__u32 delaylp=50*ONE_MS;
__u32 last_time;
void main_joy(void)
{
	joy_init();
	
	
	while (1) 
	{		
			if (joy_read(&js)==True){
		
				
				//	printf("<<<<<<<< EVENT <<<<<<<<<<<<<\n");
				//	printf("Event: type %d, time %d, number %d, value %d\n",js.type, js.time, js.number, js.value); 
				//  sprintf(joy_message,"Event: type %d,number %d, value %d",js.type, js.number, js.value);

				printf("Delta_Time: %d\n",js.time-last_time);

				if(js.time-last_time>7)
				{

					// Parse and re-encode 
					if(js.type==JS_EVENT_BUTTON)
						sprintf(joy_message,"B:%d,%d",js.number, js.value);
			
			
					if(js.type==JS_EVENT_AXIS)
					{
			
						int x=(int)((32767+js.value)/256.0);   //Remap  -32767 < js.value <32767 --> 0 < x < 255
						
						sprintf(joy_message,"A:%d,%d",js.number,x); // Create message to kinetis
			
						
					}
			
			
					printf("%s\n",joy_message); // show message on screen and send it 
				
					send_UDP(joy_message);
						
					receive_UDP(rcv_buf);
			
					printf("Data: %s\n" ,rcv_buf); // show received data (if nack was received ESP works Kinetis no)
			
				}	
				else
				{
					printf("Noooooooooo \n");
					delaylp=5*ONE_MS;
				}
				
				last_time=js.time;
	
			}
			
			usleep(delaylp);
			delaylp=50*ONE_MS;
	}	
}




void mainTB1(void) // old Test Bench
{

	int	retry_count;


		
		retry_count=5;
		
		while(retry_count--)
		{	
			
			send_UDP(message2send);   
			usleep(50*ONE_MS); // 50ms * Retry poll time
			receive_UDP(rcv_buf);
			printf("Data: %s\n" ,rcv_buf);
	    }
	     
	  
        
	    
		exit(1);
		
		
		
		
	

}






void die(char *s)
{
    perror(s);
  //  exit(1);
}
 



int send_UDP(char *message)
{
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
   // char message[BUFLEN];
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
   
    if (inet_aton(SERVER , &si_other.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
 
    //send the message
    if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
            die("sendto()");
    }
       
    close(s);
    return 0;
}








int receive_UDP(char *buf)
{
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
   // char buf[BUFLEN];
     
    struct timeval read_timeout;
    
     
     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }
   
//  Set Timeout     
//  Total time is tv_sec + (1.0/1000000) * tv_usec seconds.
//  That's why when you need times under a second you set tv_usec,
//  when you need times over 1sec you set both 
//  (but usually end up setting only tv_sec)

   
    read_timeout.tv_sec = 0;	   // 0 seg
	read_timeout.tv_usec = 400000; // 400ms
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout); 
     
//       printf("Waiting for data...");
//       fflush(stdout);
         
//try to receive some data, this is ussually a blocking call but we introduced a timeout
//given by the variable read_timeout.
 
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
         close(s);
         return(-1);
    }
         
//print details of the client/peer and the data received


    //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    //printf("Data: %s\n" , buf);
     
 
    close(s);
    return 0;
}

