/*
 * joy.c
 *
 *  Created on: 21/8/2017
 *      Author: dany
 */

#include <string.h>
#include "Pin.h"
#include "PWM.h"


int dummy;
// This code goes on Kinetis Board (this is a Test Bench)
//// Importante activar la funcion Analog en el joystick

// Button functions

void fbot0(int value)			// Boton 1 verde
{
	//printf("B0->%d\n",value);
	 Pin_Write(PIN_LED_GREEN, value);

}

void fbot1(int value)			// Boton 2 Rojo
{
	//printf("B1->%d\n",value);
	 Pin_Write(PIN_LED_RED, value);
}

void fbot2(int value)			// Boton 3 Celeste
{
	//printf("B2->%d\n",value);
	 Pin_Write(PIN_LED_BLUE, value);
}

void fbot3(int value)			// Boton 4 Rosa
{
	//printf("B3->%d\n",value);
	 Pin_Write(PIN_LED_BLUE, value);
	 Pin_Write(PIN_LED_RED, value);
}

void fbot4(int value)			// Boton trasero izquierdo arriba
{
	//printf("B4->%d\n",value);
	 Pin_Write(PIN_LED_BLUE, value);
	 Pin_Write(PIN_LED_GREEN, value);
}

void fbot5(int value)			// Boton trasero derecho arriba
{
	//printf("B5->%d\n",value);
	 Pin_Write(PIN_LED_GREEN, value);
	 Pin_Write(PIN_LED_RED, value);
}

void fbot6(int value)			// Boton trasero izquierdo abajo
{
	//printf("B6->%d\n",value);
	 Pin_Write(PIN_LED_GREEN, value);
	 Pin_Write(PIN_LED_BLUE, value);
	 Pin_Write(PIN_LED_RED, value);
}

void fbot7(int value)			// Boton trasero derecho abajo
{
	//printf("B7->%d\n",value);
	 Pin_Write(PIN_LED_GREEN, value);
}

void fbot8(int value)
{
	//printf("B8->%d\n",value);	// boton select
	Pin_Write(PIN_LED_RED, value);
}

void fbot9(int value)			// boton start
{
	//printf("B9->%d\n",value);
	Pin_Write(PIN_LED_BLUE, value);
}

void fbot10(int value)			// Boton 10 Joystick derecho
{
	//printf("B10->%d\n",value);
	 Pin_Write(PIN_LED_BLUE, value);
	 Pin_Write(PIN_LED_RED, value);
}

void fbot11(int value)			// Boton 11 Joystick derecho
{
	//printf("B11->%d\n",value);
	 Pin_Write(PIN_LED_BLUE, value);
	 Pin_Write(PIN_LED_GREEN, value);
}



// Axis functions

void faxis0(int value)
{
	//printf("A0->%d\n",value);
	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, value);
}

void faxis1(int value)
{
	//printf("A1->%d\n",value);
	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, value);
}

void faxis2(int value)
{
	//printf("A2->%d\n",value);
	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, value);
}

void faxis3(int value)
{
	//printf("A3->%d\n",value);
	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, value);
}

void faxis4(int value)	// Flechas horizontales izq -32767 der +32767
{
	//printf("A4->%d\n",value);
	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, value);
}

void faxis5(int value)	// Flechas verticales  arriba -32767 abajo +32767
{
	//printf("A5->%d\n",value);
	PWM_SetDuty(PWMID_MOT_IZQ_PLUS, value);
}


void(*Button_Actions[])(int)={fbot0,fbot1,fbot2,fbot3,fbot4,fbot5, \
	                           fbot6,fbot7,fbot8,fbot9,fbot10,fbot11};
void(*Axis_Actions[])(int)={faxis0,faxis1,faxis2,faxis3,faxis4,faxis5};

void(*pf)(int);

void parse_joy_string(unsigned char *ptr_joy_message)
{
	const char *delimiter=":,";
	char *pt=(char *)0;

	unsigned char joy_control_type[2];
	unsigned int  joy_control_number;
	int joy_control_value;


	pt = strtok(ptr_joy_message,delimiter); //Control type: Axis or Button
	strcpy(joy_control_type,pt);

	pt = strtok(NULL,delimiter);	// Control Number
	joy_control_number=atoi(pt);

	pt = strtok(NULL,delimiter);	// Control Value
	joy_control_value=atoi(pt);

	////printf(">>>>>>\n");
	////printf("Control:%s,Number:%d,Value:%d",joy_control_type,joy_control_number,joy_control_value);
	////printf("\n<<<<<<<<\n");

	if(!strcmp(joy_control_type,"B"))  //Was a button?
	{


		Button_Actions[joy_control_number](joy_control_value);

	}


	if(!strcmp(joy_control_type,"A")) //Was an Axis?
	{


		Axis_Actions[joy_control_number](joy_control_value);

	}
}

