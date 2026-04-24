/*
 * bmp280.h
 *
 *  Created on: 21-Apr-2026
 *      Author: sathi
 */


#ifndef BMP280_H_
#define BMP280_H_
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>

#define BMP_ADDR 0x76
/* Declare variables (NOT define) */
extern uint16_t dig_T1;
extern int16_t dig_T2, dig_T3;
extern uint16_t dig_P1;
extern int16_t dig_P2,dig_P3,dig_P4,dig_P5,dig_P6,dig_P7,dig_P8,dig_P9;
extern int32_t t_fine;



void FPU_Enable(void);
//void SysTick_Init(void);
//void delay_ms(uint32_t ms);
//void UART2_Init(void);
//void UART_SendString(char *s);
void I2C1_Init(void);
void I2C_Write(uint8_t reg, uint8_t data);
uint8_t I2C_Read(uint8_t reg);
void BMP_Read_Calib(void);
void BMP_Init(void);
float BMP_Read_Temp(void);
float BMP_Read_Press(void);


#endif /* BMP280_H_ */
