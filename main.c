//#include"bmp280.h"
//
///* ================= MAIN ================= */
//
//char buf[100];
//
//int main(void)
//{
//    FPU_Enable();
//    SysTick_Init();
//    UART2_Init();
//    I2C1_Init();
//
//    BMP_Init();
//
//    while(1)
//    {
//        float t = BMP_Read_Temp();
//        float p = BMP_Read_Press();
//
//        sprintf(buf,"T=%.2fC  P=%.2fPa\r\n",t,p);
//        UART_SendString(buf);
//
//        delay_ms(1000);
//    }
//}
#include"nrf_tx.h"
#include"stdio.h"
#include"bmp280.h"
char buf[100];
void UART2_Init(void)
{
    RCC->AHB1ENR |= (1<<0);
    RCC->APB1ENR |= (1<<17);

    GPIOA->MODER &= ~(3<<(2*2));
    GPIOA->MODER |=  (2<<(2*2));

    GPIOA->AFR[0] &= ~(0xF<<(2*4));
    GPIOA->AFR[0] |=  (7<<(2*4));

    USART2->BRR = 0x0683;
    USART2->CR1 |= (1<<3);
    USART2->CR1 |= (1<<13);
}

void UART2_SendString(char *str)
{
    while(*str)
    {
        while(!(USART2->SR & (1<<7)));
        USART2->DR = *str++;
    }
}
int main(void)
{
    SysTick_Init();
    FPU_Enable();
    UART2_Init();
    SPI1_Init();
    NRF_Init_TX();
    I2C1_Init();
    BMP_Init();
    typedef struct{
    	uint32_t value1;
    	uint32_t value2;
    }txdata;
    txdata r;
    while(1)
    {
    	  float t = BMP_Read_Temp();
    	  float p = BMP_Read_Press();
    	  r.value1=t*100;
    	  r.value2=p;
    	NRF_Clear_And_Flush_TX();
        NRF_Send((uint8_t*)&r, sizeof(r));
        NRF_Write_Reg(0x07, 0x70); // clear all flags
        delay_ms(1100);

    }
}
