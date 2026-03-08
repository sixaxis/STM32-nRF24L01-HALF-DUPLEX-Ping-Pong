/*
 * nrf24.h
 *
 *  Created on: Mar 7, 2026
 *      Author: sixaxis
 */

#ifndef NRF24_H_
#define NRF24_H_

#include "stm32f4xx_hal.h"

// Команды
#define NRF24_CMD_W_REGISTER    0x20
#define NRF24_CMD_R_RX_PAYLOAD  0x61
#define NRF24_CMD_W_TX_PAYLOAD  0xA0
#define NRF24_CMD_FLUSH_TX      0xE1
#define NRF24_CMD_FLUSH_RX      0xE2

// Регистры адресов
#define NRF24_REG_CONFIG      0x00
#define NRF24_REG_EN_AA       0x01
#define NRF24_REG_EN_RXADDR   0x02
#define NRF24_REG_SETUP_AW    0x03
#define NRF24_REG_SETUP_RETR  0x04
#define NRF24_REG_RF_CH       0x05
#define NRF24_REG_RF_SETUP    0x06
#define NRF24_REG_STATUS      0x07
#define NRF24_REG_RX_ADDR_P0  0x0A
#define NRF24_REG_TX_ADDR     0x10
#define NRF24_REG_RX_PW_P0    0x11
#define NRF24_REG_FIFO_STATUS 0x17


typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *CE_Port;
    uint16_t CE_Pin;
    GPIO_TypeDef *CSN_Port;
    uint16_t CSN_Pin;
} NRF24_t;

void NRF24_Init(NRF24_t *dev);
void NRF24_Mode_TX(NRF24_t *dev);
void NRF24_Mode_RX(NRF24_t *dev);
void NRF24_Transmit(NRF24_t *dev, uint8_t *data);
void NRF24_SetAddress(NRF24_t *dev, uint8_t *addr);
void NRF24_ReadPayload(NRF24_t *dev, uint8_t *data, uint8_t size);
uint8_t NRF24_Check(NRF24_t *dev);
uint8_t NRF24_ReadReg(NRF24_t *dev, uint8_t reg);
void NRF24_WriteReg(NRF24_t *dev, uint8_t reg, uint8_t data);
void CSN_Select(NRF24_t *dev);
void CSN_Unselect(NRF24_t *dev);
#endif /* INC_NRF24_H_ */
