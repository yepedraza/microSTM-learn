/*	Analog to digital converter (ADC)
		Created by: yepedraza */
#include <stdio.h>
#include "STM32F7xx.h"

int adc_data, volts;
int mil_h, cen_h, dec_h, uni_h;
int final_data=0;

void conv_adc(){

	ADC3->CR2 |= (1UL << 30);       //BIT SWSTART
	while ((ADC3->SR &= 0x2)==1);   //WAIT UNTIL CHANNEL 1 CONVERSION IS FINISHED

}

extern "C"{
	void SysTick_Handler()
	{
		conv_adc();
	}

	void ADC_IRQHandler(void){    //INTERRUPTION FOR END OF CONVERSION
		adc_data = ADC3->DR;				//DATA READING
	}
}

int main(void){
	//CLOCK CONFIGURATION
	RCC->AHB1ENR = 0xFFFF;

	//ADC CONFIGURATION
	RCC->APB2ENR |=(1UL<<10);			//Enable clock of ADC ///APB2

	ADC3->CR1 &= ~(3UL<<24);			//Resolution of 12 bits
	ADC3->CR2 |= (1UL<<11);				//Left justification
	ADC3->CR2 |= (1UL << 0);   		//Turn on ADC
	ADC3->CR2 |=(1UL<<10);     		//Set on final of convertion
	ADC3->CR1 |= (1UL << 5);   	 //Active the interruption of final convertion
	ADC3->CR2  &=~(1UL<<1);    		//Configure that the ADC does not do continuous configuration, it converts at will (simple conversion)


  NVIC_EnableIRQ(ADC_IRQn);   	//ENABLE ADC INTERRUPTION

	ADC3->SQR3 = 0x4;

	GPIOF->MODER |=(3UL<<2*6);

	//SYSTICK
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock/750);

	while(true){

		mil_h= adc_data/4096;
    cen_h= (adc_data-(mil_h*4096))/256;
    dec_h= ((adc_data-(mil_h*4096)-(cen_h*256)))/16;
    uni_h= ((adc_data-(mil_h*4096))-(cen_h*256))-(dec_h*16);

		final_data= (mil_h*256)+(cen_h*16)+dec_h;


		volts=(adc_data*0.000805801772)*100; //Voltage value in debugger visualitation

	}
}
