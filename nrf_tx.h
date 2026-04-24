/*
 * NRF_TX.h
 *
 *  Created on: 21-Apr-2026
 *      Author: sathi
 */

#ifndef NRF_TX_H_
#define NRF_TX_H_
#include"stm32f4xx.h"
#include"stdint.h"

/* ================= GPIO ================= */

#define CE_HIGH()   (GPIOA->BSRR = (1<<8))
#define CE_LOW()    (GPIOA->BSRR = (1<<8)<<16)

#define CSN_HIGH()  (GPIOA->BSRR = (1<<4))
#define CSN_LOW()   (GPIOA->BSRR = (1<<4)<<16)

#define W_REGISTER  0x20
#define R_REGISTER  0x00

void SysTick_Init(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void SPI1_Init(void);
uint8_t SPI_Transfer(uint8_t data);
void NRF_Write_Reg(uint8_t reg, uint8_t val);
void NRF_Write_Buf(uint8_t reg, uint8_t *buf, uint8_t len);
void NRF_Send(uint8_t *buf, uint8_t len);
void NRF_Flush_TX(void);
void NRF_Flush_RX(void);
void NRF_Init_TX(void);
void NRF_Clear_And_Flush_TX(void);

#endif /* NRF_TX_H_ */
