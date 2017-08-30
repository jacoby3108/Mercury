
#include "UART.h"
#include "PORT.h"

#define UART_HAL_DEFAULT_BAUDRATE 9600

/*
#define UART0_TX_PIN 	17   //PTB17
#define UART0_RX_PIN 	16   //PTB16
#define PORT_UART0 PORTB
#define GPIO_UART0 PTB
*/
#define UART UART3
#define UART_TX_PIN 	17   //PTC17
#define UART_RX_PIN 	16   //PTC16
#define PORT_UART 		PORTC
#define GPIO_UART 		PTC

static unsigned char rx_flag=false;
static unsigned char rx_data;


void UART_Init (void)
{
	PCRstr UserPCR;
	UserPCR.PCR=false;

// Note: 5.6 Clock Gating page 192
// Any bus access to a peripheral that has its clock disabled generates an error termination.

	    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
		SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
		SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
		SIM_SCGC4 |= SIM_SCGC4_UART3_MASK;
		SIM_SCGC1 |= SIM_SCGC1_UART4_MASK;
		SIM_SCGC1 |= SIM_SCGC1_UART5_MASK;

		NVIC_EnableIRQ(UART0_RX_TX_IRQn);
		NVIC_EnableIRQ(UART1_RX_TX_IRQn);
		NVIC_EnableIRQ(UART2_RX_TX_IRQn);
		NVIC_EnableIRQ(UART3_RX_TX_IRQn);
		NVIC_EnableIRQ(UART4_RX_TX_IRQn);
		NVIC_EnableIRQ(UART5_RX_TX_IRQn);


		UART_SetBaudRate(UART, UART_HAL_DEFAULT_BAUDRATE);

	//UART0 PINS setup

		UserPCR.FIELD.MUX=PORT_mAlt3;		// UART3
		UserPCR.FIELD.IRQC=PORT_eDisabled;  // No Irqs from port

		PORT_Configure2(PORT_UART,UART_TX_PIN,UserPCR); //Setup Tx and Rx pins
		PORT_Configure2(PORT_UART,UART_RX_PIN,UserPCR);

	//UART0 Baudrate Setup

		UART_SetBaudRate (UART, 9600);

	//UART0 Setup

		//UART0->C2=UART_C2_TE_MASK | UART_C2_RE_MASK;
		UART->C2=UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK;




}


void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate)
{
	uint16_t sbr, brfa;
	uint32_t clock;

	clock = ((uart == UART0) || (uart == UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);

	baudrate = ((baudrate == 0)?(UART_HAL_DEFAULT_BAUDRATE):
			((baudrate > 0x1FFF)?(UART_HAL_DEFAULT_BAUDRATE):(baudrate)));

	sbr = clock / (baudrate << 4);               // sbr = clock/(Baudrate x 16)
	brfa = (clock << 1) / baudrate - (sbr << 5); // brfa = 2*Clock/baudrate - 32*sbr

	uart->BDH = UART_BDH_SBR(sbr >> 8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}


__ISR__ UART3_RX_TX_IRQHandler (void)
{
	unsigned char tmp;

	tmp=UART->S1;


	rx_flag=true;

	rx_data=UART->D;

}


unsigned char UART_Get_Status(void)
{
	return(rx_flag);
}

unsigned char UART_Get_Data(void)
{
	rx_flag=false;
	return(rx_data);
}

void UART_Send_Data(unsigned char tx_data)
{
	while(((UART->S1)& UART_S1_TDRE_MASK) ==0);

				UART->D = tx_data;
}


