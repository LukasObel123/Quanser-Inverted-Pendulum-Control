#include "..\func_lib\MRE6183_usart.h"


void UART_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	GPIO_StructInit(&GPIO_InitStructure);
	USART_StructInit(&USART_InitStructure);
	
	//Part f
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //Enable clock for GPIO A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //Enable clock for AFIO
	
	//part g-enable clock on usart2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//part h - 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2 is TX for USART2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Alternate function push-pull
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 50 MHz output speed
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Part i 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Alternate function push-pull
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//Part j
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_Init(USART2, &USART_InitStructure);
	
	//part k
	USART_Cmd(USART2, ENABLE);
	
}

void UART_CharOut(char serial_char_out)
{
// delay until the transmit data register is ready
while(USART_GetFlagStatus(USART2, USART_FLAG_TXE)==RESET);
// send the character
USART_SendData(USART2, serial_char_out);
// delay until the transmission is done
//while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
}

void UART_StringOut(char* str) {
    while (*str) {
        UART_CharOut(*str++);
    }
}
