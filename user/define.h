
#ifndef __DEFINE_H
#define __DEFINE_H

//常用宏定义
//位宏定义
#define	b1		0x01
#define	b2		0x02
#define	b3		0x04
#define	b4		0x08
#define	b5		0x10
#define	b6		0x20
#define	b7		0x40
#define	b8		0x80
#define	b9		0x100
#define	b10		0x200
#define	b11		0x400
#define	b12		0x800
#define	b13		0x1000
#define	b14		0x2000
#define	b15		0x4000
#define	b16		0x8000

//EEPROM地址的定义
#define Flash_Base0 0x0000		//本机地址起始地址
#define Flash_Base1 0x1000		//本机类型起始地址
#define SYS_Space   0x2000		//系统数据相关
#define Flash_Base3 0x3000
#define Flash_Base4 0x4000


//管脚宏定义
#define PIN0		GPIO_Pin_0
#define PIN1		GPIO_Pin_1
#define PIN2		GPIO_Pin_2
#define PIN3		GPIO_Pin_3
#define PIN4		GPIO_Pin_4
#define PIN5		GPIO_Pin_5
#define PIN6		GPIO_Pin_6
#define PIN7		GPIO_Pin_7
#define PIN8		GPIO_Pin_8
#define PIN9		GPIO_Pin_9
#define PIN10		GPIO_Pin_10
#define PIN11		GPIO_Pin_11
#define PIN12		GPIO_Pin_12
#define PIN13		GPIO_Pin_13
#define PIN14		GPIO_Pin_14
#define PIN15		GPIO_Pin_15

#define PINAB		GPIO_PIN_15
#define PINCS		GPIO_PIN_XY
#define	PINXY		GPIO_PIN_YZ

#define LED1		GPIOC, GPIO_Pin_6
#define LED2		GPIOC, GPIO_Pin_7
#define LED3		GPIOC, GPIO_Pin_8
#define LED4		GPIOC, GPIO_Pin_9
#define LED5 		GPIOA, GPIO_Pin_8

#define OUTPUT8		GPIOC, GPIO_Pin_2
#define OUTPUT7		GPIOC, GPIO_Pin_1
#define OUTPUT6		GPIOC, GPIO_Pin_0
#define OUTPUT5		GPIOE, GPIO_Pin_6
#define OUTPUT4		GPIOE, GPIO_Pin_5
#define OUTPUT3		GPIOE, GPIO_Pin_4
#define OUTPUT2		GPIOE, GPIO_Pin_3
#define OUTPUT1		GPIOE, GPIO_Pin_2

//在这里定义多一个管脚，用来作为以太网模块的重启功能，PE1
#define RESET_PIN	  GPIOE, GPIO_Pin_1

#define INPUT1		GPIOC, GPIO_Pin_3
#define INPUT2		GPIOA, GPIO_Pin_1
#define INPUT3		GPIOA, GPIO_Pin_2
#define INPUT4		GPIOA, GPIO_Pin_3
#define INPUT5		GPIOC, GPIO_Pin_4
#define INPUT6		GPIOC, GPIO_Pin_5
#define INPUT7		GPIOB, GPIO_Pin_0
#define INPUT8		GPIOB, GPIO_Pin_1
#define INPUT9		GPIOE, GPIO_Pin_7
#define INPUT10		GPIOE, GPIO_Pin_8
#define INPUT11		GPIOE, GPIO_Pin_9
#define INPUT12		GPIOE, GPIO_Pin_10
#define INPUT13		GPIOE, GPIO_Pin_11
#define INPUT14		GPIOE, GPIO_Pin_12
#define INPUT15		GPIOE, GPIO_Pin_13
#define INPUT16		GPIOE, GPIO_Pin_14

#include "serial1.h"
#include "serial3.h"
#include "serial4.h"
#include "serial5.h"
#include "printf.h"
#include "usart.h"
#include "communication.h"
#include "user.h"

#endif
