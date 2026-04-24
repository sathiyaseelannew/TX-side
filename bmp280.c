#include"bmp280.h"
uint16_t dig_T1;
int16_t dig_T2, dig_T3;
uint16_t dig_P1;
int16_t dig_P2,dig_P3,dig_P4,dig_P5,dig_P6,dig_P7,dig_P8,dig_P9;
int32_t t_fine;
/* ================= FPU ENABLE ================= */
void FPU_Enable(void)
{
    SCB->CPACR |= (0xF << 20);
}

/* ================= SYSTICK ================= */
//void SysTick_Init(void)
//{
//    SysTick->LOAD = 16000 - 1; // 1ms @16MHz
//    SysTick->VAL = 0;
//    SysTick->CTRL = 5;
//}
//
//void delay_ms(uint32_t ms)
//{
//    for(uint32_t i=0;i<ms;i++)
//        while(!(SysTick->CTRL & (1<<16)));
//}

/* ================= UART ================= */
//void UART2_Init(void)
//{
//    RCC->APB1ENR |= (1<<17);
//    RCC->AHB1ENR |= (1<<0);
//
//    GPIOA->MODER |= (2<<(2*2));
//    GPIOA->AFR[0] |= (7<<(4*2));
//
//    USART2->BRR = 0x683; // 9600 @16MHz
//    USART2->CR1 |= (1<<13)|(1<<3);
//}
//
//void UART_SendString(char *s)
//{
//    while(*s)
//    {
//        while(!(USART2->SR & (1<<7)));
//        USART2->DR = *s++;
//    }
//}

/* ================= I2C ================= */
void I2C1_Init(void)
{
    RCC->APB1ENR |= (1<<21);
    RCC->AHB1ENR |= (1<<1);

    GPIOB->MODER |= (2<<(8*2)) | (2<<(9*2));
    GPIOB->OTYPER |= (1<<8)|(1<<9);
    GPIOB->PUPDR |= (1<<(8*2))|(1<<(9*2));
    GPIOB->AFR[1] |= (4<<(0*4)) | (4<<(1*4));

    I2C1->CR2 = 16;
    I2C1->CCR = 80;
    I2C1->TRISE = 17;

    I2C1->CR1 |= (1<<0);
}

/* I2C Write */
void I2C_Write(uint8_t reg, uint8_t data)
{
    I2C1->CR1 |= (1<<8);
    while(!(I2C1->SR1 & 1));

    I2C1->DR = BMP_ADDR<<1;
    while(!(I2C1->SR1 & (1<<1)));
    (void)I2C1->SR2;

    I2C1->DR = reg;
    while(!(I2C1->SR1 & (1<<7)));

    I2C1->DR = data;
    while(!(I2C1->SR1 & (1<<7)));

    I2C1->CR1 |= (1<<9);
}

/* I2C Read */
uint8_t I2C_Read(uint8_t reg)
{
    uint8_t data;

    I2C1->CR1 |= (1<<8);
    while(!(I2C1->SR1 & 1));

    I2C1->DR = BMP_ADDR<<1;
    while(!(I2C1->SR1 & (1<<1)));
    (void)I2C1->SR2;

    I2C1->DR = reg;
    while(!(I2C1->SR1 & (1<<7)));

    I2C1->CR1 |= (1<<8);
    while(!(I2C1->SR1 & 1));

    I2C1->DR = (BMP_ADDR<<1)|1;
    while(!(I2C1->SR1 & (1<<1)));
    (void)I2C1->SR2;

    I2C1->CR1 |= (1<<9);

    while(!(I2C1->SR1 & (1<<6)));
    data = I2C1->DR;

    return data;
}

/* ================= BMP280 ================= */



void BMP_Read_Calib(void)
{
    uint8_t calib[24];

    for(int i=0;i<24;i++)
        calib[i] = I2C_Read(0x88+i);

    dig_T1 = calib[1]<<8 | calib[0];
    dig_T2 = calib[3]<<8 | calib[2];
    dig_T3 = calib[5]<<8 | calib[4];

    dig_P1 = calib[7]<<8 | calib[6];
    dig_P2 = calib[9]<<8 | calib[8];
    dig_P3 = calib[11]<<8 | calib[10];
    dig_P4 = calib[13]<<8 | calib[12];
    dig_P5 = calib[15]<<8 | calib[14];
    dig_P6 = calib[17]<<8 | calib[16];
    dig_P7 = calib[19]<<8 | calib[18];
    dig_P8 = calib[21]<<8 | calib[20];
    dig_P9 = calib[23]<<8 | calib[22];
}

void BMP_Init(void)
{
    I2C_Write(0xF4, 0x27);
    BMP_Read_Calib();
}

float BMP_Read_Temp(void)
{
    int32_t adc_T = (I2C_Read(0xFA)<<12) |
                    (I2C_Read(0xFB)<<4) |
                    (I2C_Read(0xFC)>>4);

    int32_t var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * dig_T2)>>11;
    int32_t var2 = (((((adc_T>>4)-dig_T1)*((adc_T>>4)-dig_T1))>>12)*dig_T3)>>14;

    t_fine = var1 + var2;

    return ((t_fine * 5 + 128) >> 8) / 100.0f;
}

float BMP_Read_Press(void)
{
    int32_t adc_P = (I2C_Read(0xF7)<<12) |
                    (I2C_Read(0xF8)<<4) |
                    (I2C_Read(0xF9)>>4);

    int64_t var1 = ((int64_t)t_fine)-128000;
    int64_t var2 = var1*var1*dig_P6;
    var2 += (var1*dig_P5)<<17;
    var2 += ((int64_t)dig_P4)<<35;

    var1 = ((var1*var1*dig_P3)>>8) + ((var1*dig_P2)<<12);
    var1 = (((int64_t)1<<47)+var1)*dig_P1>>33;

    if(var1==0) return 0;

    int64_t p = 1048576 - adc_P;
    p = ((p<<31)-var2)*3125/var1;

    var1 = (dig_P9*(p>>13)*(p>>13))>>25;
    var2 = (dig_P8*p)>>19;

    p = ((p+var1+var2)>>8) + ((int64_t)dig_P7<<4);

    return (float)p/256;
}
