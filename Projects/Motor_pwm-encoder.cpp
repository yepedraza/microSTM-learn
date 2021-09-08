/*	PWM and encoder for a motor application
		Created by: yepedraza */
#include <stdio.h>
#include "STM32F7xx.h"

int notches = 0;
int time=50000,i=0;
int cont=0;
int conta=0;
int band;
int RPM;
int cont_received = 0;
int F_UART=16000000;			//UART 16Mhz
int F_DES=9600;						//Bps
int calc =0;
char received_data=0, vec_CU[3]={0,0,0};
int to_send[5] = {0, 0, 46, 0, 0};
int aux;
int percent_CU, CU;


char num[]={"0123456789"};


int adc_data;
int grades=0;

int Prescaler;
int frec_TIM=16000000; 		//16 Mhz
int frec_Servo=350; 			// 50 hz = 20 ms
int frec_D=1000000; 			// 1Mhz = 1us
int data1_serv=0;
bool aument=true;
int y=0;
//**********************************

void Send_data (char data_send)
{
	UART4->TDR=data_send;
	while((UART4->ISR &=0x80)==0);
}

extern "C"
{
	void SysTick_Handler()
	{
		RPM = (notches/60)*100;

			to_send[0] = (RPM/1000) + 48;
			aux = (RPM/100);
			to_send[1] = (aux % 10) + 48;
			aux = (RPM/10);
			to_send[3] = (aux % 10) + 48;
			to_send[4] = (RPM % 10) + 48;

			for (int i = 0; i<5;i++)
			{
				Send_data(to_send[i]);
			}

			Send_data('-');
			Send_data('-');

	}

	void EXTI15_10_IRQHandler() {
	EXTI->PR |=(1UL<<13);
			notches++;
	}

	void UART4_IRQHandler()
	{
		if (UART4->ISR &= 0x20)
		{
			received_data= (UART4->RDR)-48;
			vec_CU [cont_received] = received_data;
			cont_received++;

			if (cont_received >= 2)
			{
				cont_received = 0;
			}

			percent_CU = vec_CU[0]*100+vec_CU[1]*10+vec_CU[2];

			CU = 40.95*percent_CU;

		}
	}
}



int main(void)
{

	//CLOCK
	RCC->AHB1ENR =0x6;


	GPIOB->MODER = 0x55555555;
  GPIOB->OTYPER = 0x0;
	GPIOB->OSPEEDR = 0x55555555;
	GPIOB->PUPDR = 0x55555555;
	GPIOB->MODER =0X55555555;
	GPIOB->OTYPER =0;
	GPIOB->OSPEEDR =0X55555555;
	GPIOB->PUPDR =0X55555555;

	//SYSTICK
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock);

	SYSCFG ->EXTICR [3] &=0xFF0F;
	SYSCFG ->EXTICR [3] |=0x0020;

	EXTI ->IMR |=(1UL<<13);
	EXTI ->FTSR |=(1UL<<13);		//FALLING
	EXTI ->PR |=(1UL<<13);

	NVIC_EnableIRQ(EXTI15_10_IRQn);


	GPIOC->MODER &=~(3UL<<2*13);

	GPIOA->MODER &=~(3UL<<2*0);
	GPIOA->MODER |=(2UL<<2*0);

	GPIOC->MODER &=~(3UL<<2*11);
	GPIOC->MODER |=(2UL<<2*11);


	//UART
	RCC->APB1ENR |=(1UL<<19);
	calc=F_UART/F_DES;
	UART4->BRR = calc;

	UART4->CR1 |=(1UL<<0);
	UART4->CR1 |=(1UL<<2);
	UART4->CR1 |=(1UL<<3);
	UART4->CR1 |=(1UL<<5);

	GPIOA->AFR[0]|=0x8;
	GPIOC->AFR[1]|=0x8000;

	NVIC_EnableIRQ(UART4_IRQn);

	GPIOC->MODER &=~(3UL<<2*13);

	GPIOC->MODER &=~(3UL<<2*13);

	GPIOA->MODER &=~(3UL<<2*0);
	GPIOA->MODER |=(2UL<<2*0);

	GPIOC->MODER &=~(3UL<<2*11);
	GPIOC->MODER |=(2UL<<2*11);
	RCC->APB1ENR |=(1UL<<3);

	GPIOA->MODER |=(2UL<<2*0);
	GPIOA->PUPDR |=(1UL<<2*0);

	GPIOA->AFR[0] |=0X2000000;
	GPIOA->AFR[0] |=0X2;
	GPIOB->AFR[0] |=0X2000000;
	GPIOA->AFR[0] |=0X100000;


	//Timer 5
	Prescaler=((frec_TIM/frec_D)-1);
	TIM5->EGR |= (1UL<<0);
	TIM5->PSC = Prescaler;
	TIM5->ARR = (frec_D/frec_Servo);
	TIM5->DIER |= (1UL<<0);
	TIM5->CR1 |= (1UL<<0);
	TIM5->CCER |=TIM_CCER_CC1E;
	TIM5->CCMR1 = 0X60;
	TIM5->CCR1 = 700;

	NVIC_EnableIRQ(UART4_IRQn);


while(true){

		y = CU;//y=PA0
		data1_serv=y; //0 y 4096
		TIM5->CCR1 = data1_serv;

		}
}
