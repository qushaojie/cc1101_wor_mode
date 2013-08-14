#ifndef _MCU_CONFIG_H__
#define _MCU_CONFIG_H__

#include "stc_new_8051.h"
#include <intrins.h>
#define		INT8U	unsigned char
#define		INT16U	unsigned int
#define		INT32U	unsigned long
#define		PD_ON	0x02
#define		PD_OFF	0xFD

sbit    MOSI    =   P0^0;   
sbit    SCK     =   P0^1;   
sbit    MISO    =   P0^2;
sbit    GDO2    =   P0^3;   
sbit    GDO0    =   P3^3;						// INT1外部中断INT1 低电平触发
sbit    CSN     =   P0^4;

// 此处需要将GDO0管脚连接到外部中断上，并配置为低电平中断。
//sbit	GDO0	=	P3^2;		// INT0

//sbit    LED_R   =   P1^7;

sbit LED_R  =   P3^7;
sbit LED_D1 =   P2^0;
sbit LED_D2	=	P2^1;
sbit LED_D3 =   P2^2;
sbit LED_D4	=	P2^3;

extern INT8U _1s_counter,leng,count; 
extern INT8U flag_rx;

extern INT8U TxBuf[64];	 			// 11字节, 如果需要更长的数据包,请正确设置
extern INT8U RxBuf[64];

void ExterInterrupt();
void SpiInit(void);
void CpuInit(void);
INT8U SpiTxRxByte(INT8U dat);
void UART_init();
void Log_printf(INT8U *p_log);
void Usart_printf(INT8U *p_uart,INT8U num);
void delay(unsigned int s);
void ser();

#endif