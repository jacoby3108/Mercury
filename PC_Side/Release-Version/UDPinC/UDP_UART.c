/* TEST OF THREADS */

//Compile  gcc -o th2 th2.c -lpthread

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

#define SERVER "192.168.0.101"     // esp8266 address
#define BUFLEN 512  //Max length of buffer
//#define PORT 4210   //The port on which to send data
#define PORT 5000   //The port on which to send data 

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

int receive_UDP(void);
int send_UDP(char *message);

unsigned int TimerTick;


void * thread1()  // Time base Thread
{
    while(1)
    {
               
		usleep(100*ONE_MS); // 100ms * 

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
	   	  
	        TimerTick=time_delay(500);       // 500ms 
	       // printf("Thread 2 r=%d\n",r++);  // Call SPI
	                
	    }

	}

}


char *message2send="hola24";

void * thread3() // The APP

{
int	retry_count;
	while (1)
	{
		
		retry_count=50;
			
		//send_UDP(message2send);   
		//usleep(800*ONE_MS); // 100ms * Retry poll time
		
		
		while(retry_count)
		{
		
			if(receive_UDP()!=0)
			{
				printf(RED_TEXT "Retying %d",retry_count);
				printf("message \n" WHITE_TEXT );
				
				usleep(100*ONE_MS); // 100ms * Retry poll time
				send_UDP(message2send);
				retry_count--;
				if(retry_count==0)
					exit(1);
			}
			else
			retry_count=0;
			
		}
	
	while(1)
		usleep(20*ONE_MS); // 20ms * Poll Time 
		
		
	}
}


int main()
{
        int status;
        pthread_t tid1,tid2,tid3;
        srand(time(NULL));

        pthread_create(&tid1,NULL,thread1,NULL);
        pthread_create(&tid2,NULL,thread2,NULL);
        pthread_create(&tid3,NULL,thread3,NULL);
        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        pthread_join(tid3,NULL);
        return 0;
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








int receive_UDP(void)
{
    struct sockaddr_in si_me, si_other;
     
    int s, i, slen = sizeof(si_other) , recv_len;
    char buf[BUFLEN];
     
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


    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Data: %s\n" , buf);
     
 
    close(s);
    return 0;
}

