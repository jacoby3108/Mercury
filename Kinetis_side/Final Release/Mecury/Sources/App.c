
#include "App.h"
#include "UART.h"

#include "PORT.h"
#include "PWM.h"
#include "Pin.h"
#include "SysTick.h"
#include "UART.h"
#include "joy.h"

#include <string.h>
#define STX 0x02
#define ETX 0x03
#define ENQ 0x05
#define ACK 0x06
#define NACK 0x15

void parse_incomming_data(void);

void App_Init (void)
{

	Pin_Init();
	PWM_Init ();
	SysTick_Init();
	UART_Init();



	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, 128);
	PWM_SetDuty(PWMID_MOT_IZQ_MINUS,128);
	PWM_SetDuty(PWMID_MOT_DER_PLUS, 128);
	PWM_SetDuty(PWMID_MOT_DER_MINUS,128);


	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, 128);

}

#define MAXMES 5
enum status {TRUE,FALSE};
unsigned char joy_message[20];
unsigned char mensajes[MAXMES][10];
int mes_index=0;
enum status message_arrived=FALSE;
int j=0;

void App_Run (void)
{

	while(1) // Test code
	{
		if(UART_Get_Status())			//if data vas received parse it
			parse_incomming_data();

		if(message_arrived==TRUE)		//Send an answer
		{
			 UART_Send_Data(STX);		//Frame [STX] [Ascii Data] [ETX]
			 UART_Send_Data(0x41);		// Sequential Answer AB,AC,AD,....AZ
			 UART_Send_Data(0x42+j++);
			 UART_Send_Data(ETX);
			 if(j>24)
			 j=0;

			 if (mes_index<MAXMES){
			 strcpy(mensajes[mes_index],joy_message);
			 mes_index++;
			 }
			 else
				 mes_index=0;

			 parse_joy_string(joy_message);
			 message_arrived=FALSE;


		}

	}
}


void parse_incomming_data(void)		// tiny FSM
{
enum STATE {S_ILDE,S_GET_DATA};
static enum STATE rx_state=S_ILDE;
unsigned char data;
static int i=0;


				data=UART_Get_Data();

				if(rx_state==S_ILDE)
				{
					if(data==STX)			//Start of Frame
					{
						rx_state=S_GET_DATA;
						i=0;

					}

					return;
				}

				if(rx_state==S_GET_DATA)
				{
					if(data==ETX)			//End of Frame
					{
						rx_state=S_ILDE;
						joy_message[i++]=0x00;		//Payload Terminator
						message_arrived=TRUE;

						return;
					}

					if(data==STX)  //if by chance frame gets unsynced reset FSM
					{
						rx_state=S_ILDE;
					}

					joy_message[i++]=data;			//Store Payload


				}



}
