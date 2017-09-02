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
#include <stdarg.h>

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <unistd.h> // close();
#include <arpa/inet.h>
#include <sys/socket.h>


// joystick
#include "joyapi.h"
#include <linux/input.h>
#include <linux/joystick.h>


//#define SERVER "192.168.0.100"     // esp8266 address
#define SERVER "192.168.43.6"        // esp8266 address (using my phone)
#define PORT 5000   				// The port on which to send data


#define  BUFLEN 512  //Max length of buffer
#define  ONE_MS    	1000	   // One mseg
#define  TIME_BASE  50         // in mseg 
#define  time_delay(x)  (x/TIME_BASE)    // x in ms , minvalue 100mseg



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
void create_output_message(void);
int map_axis(__s16 value );
void init_axes_state(void);

//--------------------
pthread_mutex_t printf_mutex;
int sync_printf(const char *format, ...);

void * thread1();
void * thread2();
void * thread3();
void * thread4();

__u32 delaylp=50*ONE_MS;
__u32 last_time;
unsigned int TimerTick;

typedef enum {
	JOY_ID_AXIS_LX,
	JOY_ID_AXIS_LY,
	JOY_ID_AXIS_RX,
	JOY_ID_AXIS_RY,
	JOY_ID_AXIS_PADX,
	JOY_ID_AXIS_PADY,
	JOY_ID_BUT_GREEN1,
	JOY_ID_BUT_RED2,
	JOY_ID_BUT_BLUE3,
	JOY_ID_BUT_PINK4,
	JOY_ID_BUT_L1,
	JOY_ID_BUT_R1,
	JOY_ID_BUT_L2,
	JOY_ID_BUT_R2,
	JOY_ID_BUT_SELECT,
	JOY_ID_BUT_START,
	JOY_ID_BUTT10,
	JOY_ID_BUTT11,

	JOY_CANT_IDS
}joyCtrId_t;


__s16 Control_Values[JOY_CANT_IDS];  // actual event values

//(type,number)  --> JOY_ID

int main(void)
{

		int status;
        pthread_t tid1,tid2,tid3,tid4;
        pthread_mutex_init(&printf_mutex, NULL);
        
        printf("Release 1.0 \n");
        joy_init();
        
        srand(time(NULL));
		

        pthread_create(&tid1,NULL,thread1,NULL);
        pthread_create(&tid2,NULL,thread2,NULL);
        pthread_create(&tid3,NULL,thread3,NULL);
        pthread_create(&tid4,NULL,thread4,NULL);
        
        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        pthread_join(tid3,NULL);
        pthread_join(tid4,NULL);
        return 0;

	
}




void * thread1()  // Time base Thread
{
    while(1)
    {
               
		usleep(TIME_BASE*ONE_MS); // TIME_BASE in ms * 

		if (TimerTick)
			TimerTick--;
	
    }
}

void * thread2() // Periodic Task Thread

{
	static int r=0;
	
	while (1) 
	{

		if (!TimerTick)			// Wait for Thread1
		{
	   	  
	        TimerTick=time_delay(100);             // 100ms 
	       // sync_printf("Thread 2 r=%d\n",r++);  // Test Print
	        main_joy();
	                
	    }

	}

}




void * thread3() // The APP

{
	while (1)
	{
	
				sync_printf(YELLOW_TEXT "The" " App\n" WHITE_TEXT );
				sleep(1);
	
	}
}

void * thread4() // This thread reads joystick events 

{    
	
	init_axes_state();
	create_output_message();  // This Updates Output Vector
	
	while (1)
	{
		
		if (joy_read(&js)==True){
	
				//sync_printf(RED_TEXT "<<<<<<<< EVENT <<<<<<<<<<<<<\n" WHITE_TEXT );
				
				if(js.type==JS_EVENT_INIT)
				   sync_printf(RED_TEXT "<<<<<<<< INIT <<<<<<<<<<<<<\n" WHITE_TEXT );
				
				
				if(js.type==JS_EVENT_BUTTON )
				{		
						switch(js.number)
						{
							case 0:
							Control_Values[JOY_ID_BUT_GREEN1]=js.value;
							break;
						
							case 1:
							Control_Values[JOY_ID_BUT_RED2]=js.value;
							break;
						
							case 2:
							Control_Values[JOY_ID_BUT_BLUE3]=js.value;
							break;
						
							case 3:
							Control_Values[JOY_ID_BUT_PINK4]=js.value;
							break;
						
							case 4:
							Control_Values[JOY_ID_BUT_L1]=js.value;
							break;
						
							case 5:
							Control_Values[JOY_ID_BUT_R1]=js.value;
							break;
							case 6:
							Control_Values[JOY_ID_BUT_L2]=js.value;
							break;
							case 7:
							Control_Values[JOY_ID_BUT_R2]=js.value;
							break;
							case 8:
							Control_Values[JOY_ID_BUT_SELECT]=js.value;
							break;
							case 9:
							Control_Values[JOY_ID_BUT_START]=js.value;
							break;
							case 10:
							Control_Values[JOY_ID_BUTT10]=js.value;
							break;
							case 11:
							Control_Values[JOY_ID_BUTT11]=js.value;
							break;
														
							default:
							sync_printf(RED_TEXT "Boton Desconocido\n" WHITE_TEXT );
						
						
						} // Button Switch
				
				
				
				} // Button If
				
				if(js.type==JS_EVENT_AXIS)
				{		
						switch(js.number)
						{
							case 0:
							Control_Values[JOY_ID_AXIS_LX]=map_axis(js.value);
							break;
						
							case 1:
							Control_Values[JOY_ID_AXIS_LY]=map_axis(js.value);
							break;
						
							case 2:
							Control_Values[JOY_ID_AXIS_RX]=map_axis(js.value);
							break;
						
							case 3:
							Control_Values[JOY_ID_AXIS_RY]=map_axis(js.value);
							break;
						
							case 4:
							Control_Values[JOY_ID_AXIS_PADX]=map_axis(js.value);
							break;
						
							case 5:
							Control_Values[JOY_ID_AXIS_PADY]=map_axis(js.value);
							break;
							
														
							default:
							sync_printf(RED_TEXT "Eje Desconocido\n" WHITE_TEXT );
						
						
						} // Button Switch
				
				
				
				} // Axes If
				
				create_output_message();  // This Updates Output Vector
			    
			    //Just for Debug 
				//sync_printf("%s",joy_message);	
				//sync_printf("\n");
				
				
				
				
		} // Event  if		
	
	} // thread while
}


int map_axis( __s16 value )
{
	return( (int)((32767+value)/256.0) );
}



//"J;valCtrl0;valCtrl1;...;valCtrlN

void create_output_message(void)
{
	int id;
	char temp[10];
	//-- Build Output String --
	memset(joy_message ,'\0', sizeof(joy_message));
	strcpy(joy_message,"J");
	for (id=0;id<=JOY_CANT_IDS-1;id++)
	{
		sprintf(temp,"%d",Control_Values[id]);
		strcat(joy_message,";");
		strcat(joy_message,temp);
	}		
}

void init_axes_state(void)
{

	Control_Values[JOY_ID_AXIS_LX]=127;
	Control_Values[JOY_ID_AXIS_LY]=127;
	Control_Values[JOY_ID_AXIS_RX]=127;
	Control_Values[JOY_ID_AXIS_RY]=127;
	Control_Values[JOY_ID_AXIS_PADX]=127;
	Control_Values[JOY_ID_AXIS_PADY]=127;

}


void main_joy(void)
{
	
	
		//printf("Event: type %d, time %d, number %d, value %d\n",js.type, js.time, js.number, js.value); 
		
	
		// show message on screen and send it   
		
		sync_printf("%s",joy_message);	
		sync_printf("\n");
				

				
		send_UDP(joy_message);
						
		//	receive_UDP(rcv_buf);
			
				
		
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


int sync_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&printf_mutex);
    vprintf(format, args);
    pthread_mutex_unlock(&printf_mutex);

    va_end(args);
}

