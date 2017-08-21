
#ifndef SOURCES_UART_H_
#define SOURCES_UART_H_

#include "Template/LDM.h"
void UART_Init (void);
void UART_SetBaudRate (UART_Type *uart, uint32_t baudrate);
unsigned char UART_Get_Status(void);
unsigned char UART_Get_Data(void);
void UART_Send_Data(unsigned char txdata);


#endif /* SOURCES_UART_H_ */
