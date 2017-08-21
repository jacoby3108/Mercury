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


#define SERVER "192.168.0.102"     // esp8266 address
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

char joy_message[50];

void mainTB1(void);
void main_joy(void);

void parse_joy_string(void);

void main(void)
{
	
//	mainTB1();
	main_joy();
	
	
}

void main_joy(void)
{
	joy_init();
	
	
	while (1) 
	{		
		if (joy_read(&js)==True){
		
		//	printf("<<<<<<<< EVENT <<<<<<<<<<<<<\n");
		//	printf("Event: type %d, time %d, number %d, value %d\n",js.type, js.time, js.number, js.value); 
		//  sprintf(joy_message,"Event: type %d,number %d, value %d",js.type, js.number, js.value);

			// Parse and re-encode 
			if(js.type==JS_EVENT_BUTTON)
				sprintf(joy_message,"B:%d,%d",js.number, js.value);
			if(js.type==JS_EVENT_AXIS)
				sprintf(joy_message,"A:%d,%d",js.number,js.value);
			
			
			printf("%s\n",joy_message);
			
		//	parse_joy_string();  //  this function goes in kinetis code
			
			

	
			send_UDP(joy_message); 
			
			usleep(50*ONE_MS); // 50ms * Retry poll time
			
			receive_UDP(rcv_buf);
			printf("Data: %s\n" ,rcv_buf);
			

		}	
		
		usleep(50*ONE_MS);
	}	
}

//// Importante activar la funcion Analog en el joystick

// Button functions

void fbot0(int value)			// Boton 1 verde
{
	printf("B0->%d\n",value);
}

void fbot1(int value)			// Boton 2 Rojo
{
	printf("B1->%d\n",value);
}

void fbot2(int value)			// Boton 3 Celeste
{
	printf("B2->%d\n",value);
}

void fbot3(int value)			// Boton 4 Rosa
{
	printf("B3->%d\n",value);
}

void fbot4(int value)			// Boton trasero izquierdo arriba
{
	printf("B4->%d\n",value);	
}

void fbot5(int value)			// Boton trasero derecho arriba
{
	printf("B5->%d\n",value);
}

void fbot6(int value)			// Boton trasero izquierdo abajo
{
	printf("B6->%d\n",value);
}

void fbot7(int value)			// Boton trasero derecho abajo
{
	printf("B7->%d\n",value);
}

void fbot8(int value)
{
	printf("B8->%d\n",value);	// boton select
}

void fbot9(int value)			// boton start
{
	printf("B9->%d\n",value);
}

void fbot10(int value)			// Boton 10 Joystick derecho
{
	printf("B10->%d\n",value);
}

void fbot11(int value)			// Boton 11 Joystick derecho
{
	printf("B11->%d\n",value);
}



// Axis functions

void faxis0(int value)
{
	printf("A0->%d\n",value);
}

void faxis1(int value)
{
	printf("A1->%d\n",value);
}

void faxis2(int value)
{
	printf("A2->%d\n",value);
}

void faxis3(int value)
{
	printf("A3->%d\n",value);
}

void faxis4(int value)	// Flechas horizontales izq -32767 der +32767
{
	printf("A4->%d\n",value);
}

void faxis5(int value)	// Flechas verticales  arriba -32767 abajo +32767
{
	printf("A5->%d\n",value);
}


void(*Button_Actions[])(int)={fbot0,fbot1,fbot2,fbot3,fbot4,fbot5, \
	                           fbot6,fbot7,fbot8,fbot9,fbot10,fbot11};
void(*Axis_Actions[])(int)={faxis0,faxis1,faxis2,faxis3,faxis4,faxis5};

void(*pf)(int);




void parse_joy_string(void)
{
	const char *delimiter=":,";
	char *pt=NULL;
	
	unsigned char joy_control_type[2];
	unsigned int  joy_control_number;
	int joy_control_value;
	
	
	pt = strtok(joy_message,delimiter); //Control type: Axis or Button
	strcpy(joy_control_type,pt);
	
	pt = strtok(NULL,delimiter);	// Control Number
	joy_control_number=atoi(pt);
		
	pt = strtok(NULL,delimiter);	// Control Value
	joy_control_value=atoi(pt);
	
	//printf(">>>>>>\n");
	//printf("Control:%s,Number:%d,Value:%d",joy_control_type,joy_control_number,joy_control_value);
	//printf("\n<<<<<<<<\n");

	if(!strcmp(joy_control_type,"B"))  //Was a button?
	{
		printf("ES BOTTON\n");
			
		Button_Actions[joy_control_number](joy_control_value);	
		
	}	
	
		
	if(!strcmp(joy_control_type,"A")) //Was an Axis?
	{
		printf("ES AXIS\n");
		
		Axis_Actions[joy_control_number](joy_control_value);

	}
}


void mainTB1(void)
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

