/*	Multichannel ADC for three temperature sensors
		Created by: yepedraza */
#include <stdio.h>
#include "STM32F7xx.h"

int hab1=false, hab2=false, hab3=false;
int cont1=0, cont2=0, cont3=0, save_channel=0;
int channels[8]={0x4,0x5,0x6,0x7,0x8,0x9,0xE,0xF};

int adc_value[3][5]={0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0,
										 0, 0, 0, 0, 0};

int cont_save1 = 0,cont_save2 = 0,cont_save3 = 0;
int pos_save=0;
int prom1=0, prom2=0, prom3=0;
int volts1 = 0,volts2 = 0,volts3 = 0;

int cent1=0, dec1 =0, uni1=0;
int cent2=0, dec2 =0, uni2=0;
int cent3=0, dec3 =0, uni3=0;


int F_UART=16000000;	//UART 16Mhz
int F_DES=9600;				//Bps
int calcu =0;
int time=0;

int conver_adc(int channel){
	ADC3->SQR3 = channel;
	ADC3->CR2 |= (1UL << 30);        //BIT SWSTART
	while ((ADC3->SR &= 0x2)==1);
}

void send_data (char data_send)
{
	UART4->TDR=data_send;
	while((UART4->ISR &=0x80)==0);
}

extern "C"
{
	void SysTick_Handler()
	{
		if (cont1 == 3)
		{
			hab1=true;
			cont1=0;
		}
		if (cont2 == 3)
		{
			hab2=true;
			cont2=0;
		}
		if (cont3 == 2)			//500Hz
		{
			hab3=true;
			cont3=0;
		}
		cont1++;
		cont2++;
		cont3++;
	}
	void ADC_IRQHandler(void){
		switch(save_channel)
		{
			case 0x0:
				pos_save=cont_save1;
			break;

			case 0x1:
				pos_save=cont_save2;
			break;

			case 0x2:
				pos_save=cont_save3;
			break;
		}
		adc_value[save_channel][pos_save] = ADC3->DR;
	}
}



int main(void)
{

	RCC->AHB1ENR = 0xFFFF;

	RCC->APB2ENR |=(1UL<<10);

	GPIOF->MODER |=(3UL<<2*6);
	GPIOF->MODER |=(3UL<<2*7);
	GPIOF->MODER |=(3UL<<2*8);


	ADC3->CR1 |= (1UL << 5);
	ADC3->CR2 |= (1UL << 0);
	ADC3->CR2 |= (1UL << 10);
  NVIC_EnableIRQ(ADC_IRQn);

	GPIOB->MODER =0X55555555;
	GPIOB->OTYPER =0;
	GPIOB->OSPEEDR =0X55555555;
	GPIOB->PUPDR =0X55555555;

	GPIOC->MODER &=~(3UL<<2*13);

	GPIOA->MODER &=~(3UL<<2*0);
	GPIOA->MODER |=(2UL<<2*0);

	RCC->APB1ENR |=(1UL<<19);
	calcu=F_UART/F_DES;
	UART4->BRR = calcu;
	UART4->CR1 |=(1UL<<0);
	UART4->CR1 |=(1UL<<2);
	UART4->CR1 |=(1UL<<3);
	GPIOA->AFR[0]|=0x8;

	NVIC_EnableIRQ(UART4_IRQn);

	//SYSTICK
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/1000);


while(true){

			if (hab1 == true)
			{
				save_channel=0x0;
				conver_adc(0x4);
				cont_save1++;
				hab1=false;
			}
			if (hab2 == true)
			{
				save_channel = 0x1;
				conver_adc(0x5);
				cont_save2++;
				hab2=false;
			}
			if (hab3 == true)
			{
				save_channel = 0x2;
				conver_adc(0x6);
				cont_save3++;
				hab3=false;
			}

		for (int i = 0; i<3; i++)
			{
				int acumulador=0;
				for (int j = 0; j<5; j++)
				{
					acumulador = acumulador + adc_value[i][j];
				}
				switch (i)
				{
					case 0:
						prom1 = acumulador/5;
					break;
					case 1:
						prom2 = acumulador/5;
					break;
					case 2:
						prom3 = acumulador/5;
					break;
				}
			}

		volts1=(prom1*0.000805801772)*100;
		volts2=(prom2*0.000805801772)*100;
		volts3=(prom3*0.000805801772)*100;

		cent1=volts1/100;
		dec1=(volts1/10)%10;
		uni1=volts1%10;

		cent2=volts2/100;
		dec2=(volts2/10)%10;
		uni2=volts2%10;

		cent3=volts3/100;
		dec3=(volts3/10)%10;
		uni3=volts3%10;

		send_data('T');
		send_data('e');
		send_data('m');
		send_data('p');
		send_data('1');
		send_data(':');
		send_data(cent1+48);
		send_data('.');
		send_data(dec1 +48);
		send_data(uni1 +48);
		send_data('\n');
		send_data('T');
		send_data('e');
		send_data('m');
		send_data('p');
		send_data('2');
		send_data(':');
		send_data(cent2+48);
		send_data('.');
		send_data(dec2 +48);
		send_data(uni2 +48);
		send_data('\n');
		send_data('T');
		send_data('e');
		send_data('m');
		send_data('p');
		send_data('3');
		send_data(':');
		send_data(cent3+48);
		send_data('.');
		send_data(dec3 +48);
		send_data(uni3 +48);
		send_data('\n');

	}
}
