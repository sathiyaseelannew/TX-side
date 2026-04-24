/*
 * NRF_TX.c
 *
 *  Created on: 21-Apr-2026
 *      Author: sathi
 */



/* ================= SYSTICK ================= */
#include"nrf_tx.h"
void SysTick_Init(void)
{
    SysTick->LOAD = 16000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = (1<<0) | (1<<2);
}

void delay_ms(uint32_t ms)
{
    for(uint32_t i=0;i<ms;i++)
    {
        SysTick->VAL = 0;
        while(!(SysTick->CTRL & (1<<16)));
    }
}

void delay_us(uint32_t us)
{
    uint32_t temp = SysTick->LOAD;

    SysTick->LOAD = 16 - 1;

    for(uint32_t i=0;i<us;i++)
    {
        SysTick->VAL = 0;
        while(!(SysTick->CTRL & (1<<16)));
    }

    SysTick->LOAD = temp;
}

/* ================= SPI ================= */

void SPI1_Init(void)
{
    RCC->AHB1ENR |= (1<<0);
    RCC->APB2ENR |= (1<<12);

    GPIOA->MODER &= ~(0x3F << (5*2));
    GPIOA->MODER |=  (0x2A << (5*2));

    GPIOA->AFR[0] &= ~((0xF<<(5*4)) | (0xF<<(6*4)) | (0xF<<(7*4)));
    GPIOA->AFR[0] |=  ((5<<(5*4)) | (5<<(6*4)) | (5<<(7*4)));

    GPIOA->MODER &= ~((3<<(4*2)) | (3<<(8*2)));
    GPIOA->MODER |=  ((1<<(4*2)) | (1<<(8*2)));
    GPIOA->OSPEEDR |=((3<<8)|(3<<10)|(3<<12)|(3<<14));//HIGH SPEED

    CE_LOW();
    CSN_HIGH();

    SPI1->CR1 = 0;
    SPI1->CR1 |= (1<<2);
    SPI1->CR1 |= (1<<9);
    SPI1->CR1 |= (1<<8);
    SPI1->CR1 |= (1<<3);
    SPI1->CR1 |= (1<<6);
}

uint8_t SPI_Transfer(uint8_t data)
{
    while(!(SPI1->SR & (1<<1)));
    SPI1->DR = data;
    while(!(SPI1->SR & (1<<0)));
    return SPI1->DR;
}

/* ================= NRF ================= */


void NRF_Write_Reg(uint8_t reg, uint8_t val)
{
    CSN_LOW();
    SPI_Transfer(W_REGISTER | reg);
    SPI_Transfer(val);
    CSN_HIGH();
}

void NRF_Write_Buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    CSN_LOW();
    SPI_Transfer(W_REGISTER | reg);
    for(int i=0;i<len;i++)
        SPI_Transfer(buf[i]);
    CSN_HIGH();
}

void NRF_Send(uint8_t *buf, uint8_t len)
{
    CSN_LOW();
    SPI_Transfer(0xA0);

    for(int i=0;i<len;i++)
        SPI_Transfer(buf[i]);

    CSN_HIGH();

    CE_HIGH();
    delay_us(10);
    CE_LOW();
}

/* ================= NRF TX INIT ================= */
void NRF_Flush_TX(void){
	CSN_LOW();
	SPI_Transfer(0xE1);//Flush_TX
	CSN_HIGH();
}
void NRF_Flush_RX(void){
	CSN_LOW();
	SPI_Transfer(0xE2);//Flush_RX
	CSN_HIGH();
}
void NRF_Init_TX(void)
{
    delay_ms(200);

    uint8_t addr[5] = {'N','O','D','E','1'};
    NRF_Write_Reg(0x07,0x70);//clear status flags
    NRF_Flush_TX();
    NRF_Flush_RX();
    NRF_Write_Reg(0x00, 0x0E); // TX mode

    NRF_Write_Reg(0x01, 0x00); // no auto ack
    NRF_Write_Reg(0x02, 0x01); // pipe0
    NRF_Write_Reg(0x03, 0x03); // 5 byte addr
    NRF_Write_Reg(0x05, 76);   // channel
    NRF_Write_Reg(0x06, 0x06); // RF setup

    NRF_Write_Buf(0x10, addr, 5); // TX address
    NRF_Write_Buf(0x0A, addr, 5); // RX addr (for ACK)

    CE_LOW();
}
void NRF_Clear_And_Flush_TX(void)
{
    NRF_Write_Reg(0x07, 0x70); // Clear RX_DR, TX_DS, MAX_RT

    CSN_LOW();
    SPI_Transfer(0xE1);        // FLUSH_TX
    CSN_HIGH();
}
