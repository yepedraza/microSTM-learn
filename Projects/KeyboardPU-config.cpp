/*	PullUp Keyboard configuration
	Created by: yepedraza*/
#include <stdio.h>
#include "STM32F7xx.h"

int tMatrix [4][4] ={{0x5C,0x3F,0x49,0x5E}, 
					 {0x07,0x7F,0x67,0x39},
					 {0x66,0x6D,0x7D,0x7C},
					 {0x06,0x5B,0x4F,0x77}};

void InOut(void){
	GPIOB->MODER = 0X00000055; //IN PB0-PB3 & OUT PB12-PB15
	GPIOB->OSPEEDR = 0xAA0000AA; //High Speed
	GPIOB->OTYPER = 0x0; //Push Pull
	GPIOB->PUPDR = 0xAA000055; //No Pull Up, Pull Down
	
	GPIOD->MODER = 0x1;
	GPIOD->OSPEEDR = 0x1;
	GPIOD->OTYPER =0;
	GPIOD->PUPDR =0;
	
	GPIOE->MODER = 0x1555;
	GPIOE->OSPEEDR = 1;
	GPIOE->OTYPER =0;
	GPIOE->PUPDR =0;
	
	GPIOD->ODR = 0x1;
	GPIOE->ODR = 0x7F;
}

void IEXTI(void){
	SYSCFG->EXTICR[3]=0x1111;
	EXTI->IMR |=0xF000;
	EXTI->RTSR |=0xF000;
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void Keyboard(){
	for(int i=0;i<4;i++){
		GPIOB->ODR = (1UL<<i);
		for(int j=0;j<4;j++){
			if((GPIOB->IDR & 0xF000) == (1UL<<(j+12))){
			GPIOE->ODR = ~tMatrix[i][j];
			}
		}
	}
}

extern "C"{
	void EXTI15_10_IRQHandler(void){
		EXTI->PR = 0xF000; // 1111|1111|0000|0000
		Keyboard();
	}
}

int main(){
	RCC->AHB1ENR =0xFF;
	RCC->APB2ENR =0x4000;
	
	InOut();	
	IEXTI();
	
	while(1){
		GPIOB->ODR = 0xF;
	}
}