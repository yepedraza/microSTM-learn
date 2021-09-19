/*		Analog signal processing
			Created by: yepedraza */
#include <stdio.h>
#include "STM32F7xx.h"

// ADC Variables
int channel=0;                   //Channel
int position=0;                //Channel counter
int channels[3]={0x9,0xE,0xF};  //Channel 9,14,15
int SensorVal[3][5];       //Sensor values
//Systick
int FrecHz=50;
float t1=0,t2=0,t3=0;
int p=0;
//Functions
float med1=0,med2=0,med3=0,avg1=0,avg2=0,avg3=0;
float med[3];                //Sensor average value
bool sensor=true;
//Uart
int val=-1;
int F_UART=16000000;//frecuencia del UART 16Mhz
int F_DES=9600;//Bps
int calc =0;
int send1=0,send2=0,send3=0;
char NUM[]={"0123456789"};
int u1=0,u2=0,u3=0,d1=0,d2=0,d3=0,c1=0,c2=0,c3=0,umil1=0,umil2=0,umil3=0;
char sen1[14]={'S','e','n','s','o','r','#','1','=',umil1+48,c1+48,d1+48,u1+48,'.'};
char sen2[14]={'S','e','n','s','o','r','#','2','=',umil2+48,c2+48,d2+48,u2+48,'.'};
char sen3[14]={'S','e','n','s','o','r','#','3','=',umil3+48,c3+48,d3+48,u3+48,'.'};
char alarm [15] = {'S','e','n','s','o','r','#',' ',' ','A','L','E','R','T'};

void send_dato (char data_send)
{
	UART4->TDR=data_send;
	while((UART4->ISR &=0x80)==0);
}
void send_phrase()
{
	for(int i=0;send1!='.';i++)
	{
		send1=sen1[i];
		UART4->TDR=send1;
		while((UART4->ISR &=0x80)==0);
	}
	send1=' ';
	for(int i=0;send2!='.';i++)
	{
		send2=sen2[i];
		UART4->TDR=send2;
		while((UART4->ISR &=0x80)==0);
	}
	send2=' ';
	for(int i=0;send3!='.';i++)
	{
		send3=sen3[i];
		UART4->TDR=send3;
		while((UART4->ISR &=0x80)==0);
	}
	send3=' ';
}

void alarm(int sensor)
{
	alarm[7]=sensor+48;
	send_phrase();
	for (int i = 0; i<16; i++)
	{
		UART4->TDR=alarm[i];
		while((UART4->ISR &=0x80)==0);
	}
}

extern "C"
{
	 void SysTick_Handler(void)
	 {
			t1++;  //Sensor 1
			t2++;  //Sensor 2
		  t3++;  //Sensor 3
		      if(sensor==true)
		        {
			       if(t1==1)
		           {
				         p++;
								SensorVal[0][p] = ADC3->DR;
								 if (SensorVal[0][p] > (med[0]+(med[0]*0.1)) || SensorVal[0][p] < (med[0]-(med[0]*0.1)))
								 {
									 alarm(1);
								 }
                t1=0;
								 med1=med1+SensorVal[0][p];
								 for(int delat=0;delat<1000000;delat++){}
			         }
			          if(t2==1+41/10000)
		              {
		               SensorVal[1][p] = ADC3->DR;
                   med2+=SensorVal[1][p];
									  if (SensorVal[1][p] > (med[1]+(med[1]*0.1)) || SensorVal[1][p] < (med[1]-(med[1]*0.1)))
								 {
									 alarm(2);
								 }
                   t2=0;
									for(int delat=0;delat<100000;delat++){}
			            }
									if(t3==1+3/80)
		              {
		               SensorVal[2][p] = ADC3->DR;
                   med3+=SensorVal[2][p];
								 if (SensorVal[2][p] > (med[2]+(med[2]*0.1)) || SensorVal[2][p] < (med[2]-(med[2]*0.1)))
								 {
									 alarm(3);
								 }
                   t3=0;
										for(int delat=0;delat<100000;delat++){}
			            }
		        }
						       if(p>10)
				          {
					         p=0;
				        	 sensor=false;
				          }
	  }


}



int conver_adc(int channel)
{
	ADC3->SQR3 = channels[];
	ADC3->CR2 |= (1UL << 30);
	while ((ADC3->SR &= 0x2)==1);
}
void avgedio()
{

	 med[0]=med1/10;
	 med[1]=med2/10;
	 med[2]=med3/10;

	 umil1=med[0]/1000;
	 c1=(med[0]-(umil1*1000))/100;
	 d1=(med[0]-(umil1*1000)-(c1*100))/10;
	 u1=med[0]-(umil1*1000)-(c1*100)-(d1*10);
	 umil2=med[1]/1000;
	 c2=(med[1]-(umil2*1000))/100;
	 d2=(med[1]-(umil2*1000)-(c2*100))/10;
	 u2=med[1]-(umil2*1000)-(c2*100)-(d2*10);
	 umil3=med[2]/1000;
	 c3=(med[2]-(umil3*1000))/100;
	 d3=(med[2]-(umil3*1000)-(c3*100))/10;
	 u3=med[2]-(umil3*1000)-(c3*100)-(d3*10);

}

int main(void)
{


	RCC->AHB1ENR = 0xFFFF;


	RCC->APB2ENR |=(1UL<<10);
	GPIOF->MODER |=(3UL<<2*3);
	GPIOF->MODER |=(3UL<<2*4);
	GPIOF->MODER |=(3UL<<2*5);
	ADC3->CR2 |= (1UL << 0);
	ADC3->CR2 &=~(1UL<<1);
	ADC3->CR1 |= (1UL << 5);
	ADC3->CR2 |= (1UL << 10);


	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock*FrecHz/800);


	GPIOC->MODER &=~(3UL<<2*13);
	GPIOA->MODER &=~(3UL<<2*0);
	GPIOA->MODER |=(2UL<<2*0);
	GPIOC->MODER &=~(3UL<<2*11);
	GPIOC->MODER |=(2UL<<2*11);
	RCC->APB1ENR |=(1UL<<19);
	calc=F_UART/F_DES;
	UART4->BRR = calc;
	UART4->CR1 |=(1UL<<0);
	UART4->CR1 |=(1UL<<2);
	UART4->CR1 |=(1UL<<3);
	UART4->CR1 |=(1UL<<5);
	GPIOA->AFR[0]|=0x8;
	GPIOC->AFR[1]|=0x8000;

	while(true)
	{
		for(position=0;position<3;position++)
		{
			conver_adc(position);
		}
	}
}
