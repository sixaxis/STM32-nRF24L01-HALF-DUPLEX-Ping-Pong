#include "nrf24.h"


void CSN_Select(NRF24_t *dev)   { HAL_GPIO_WritePin(dev->CSN_Port, dev->CSN_Pin, GPIO_PIN_RESET); }
void CSN_Unselect(NRF24_t *dev) { HAL_GPIO_WritePin(dev->CSN_Port, dev->CSN_Pin, GPIO_PIN_SET);   }

void NRF24_Init(NRF24_t *dev)
{
	HAL_GPIO_WritePin(dev->CSN_Port, dev->CSN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(dev->CE_Port, dev->CE_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);

	NRF24_WriteReg(dev, NRF24_REG_CONFIG,    0x0C);       // Power Down, CRC 2-byte
	NRF24_WriteReg(dev, NRF24_REG_EN_AA,     0x01);       // Auto-ACK только на Pipe 0
	NRF24_WriteReg(dev, NRF24_REG_EN_RXADDR, 0x01);       // Включить только Pipe 0
	NRF24_WriteReg(dev, NRF24_REG_SETUP_AW,  0x03);       // Адрес 5 байт
	NRF24_WriteReg(dev, NRF24_REG_SETUP_RETR,0x2F);       // 750us задержка, 15 повторов
	NRF24_WriteReg(dev, NRF24_REG_RF_CH,     76);         // Канал 76
	NRF24_WriteReg(dev, NRF24_REG_RF_SETUP,  0x06);       // 1Mbps, 0dBm (Самый стабильный режим)
	NRF24_WriteReg(dev, NRF24_REG_RX_PW_P0,  32);         // Статичный пакет 32 байта
	NRF24_WriteReg(dev, NRF24_REG_STATUS,    0x70);       // Сброс флагов

	// Очистка FIFO
	uint8_t cmd = NRF24_CMD_FLUSH_TX;
	CSN_Select(dev);
	HAL_SPI_Transmit(dev->hspi, &cmd, 1, 10);
	CSN_Unselect(dev);
	cmd = NRF24_CMD_FLUSH_RX;
	CSN_Select(dev);
	HAL_SPI_Transmit(dev->hspi, &cmd, 1, 10);
	CSN_Unselect(dev);

	// По умолчанию включаем режим TX (Power UP)
	NRF24_Mode_TX(dev);
}


uint8_t NRF24_ReadReg(NRF24_t *dev, uint8_t reg)
{
    uint8_t command = reg & 0x1F; // Маска для команды чтения регистра
    uint8_t res;
    CSN_Select(dev);
    HAL_SPI_Transmit(dev->hspi, &command, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(dev->hspi, &res, 1, HAL_MAX_DELAY);
    CSN_Unselect(dev);
    return res;
}

uint8_t NRF24_Check(NRF24_t *dev)
{
    uint8_t check_val = 0xAB;
    uint8_t reg_addr = NRF24_REG_SETUP_RETR; // Используем как временное хранилище

    // Сохраняем старое значение, пишем новое, проверяем, возвращаем старое
    uint8_t initial_val = NRF24_ReadReg(dev, reg_addr);
    NRF24_WriteReg(dev, reg_addr, check_val);
    uint8_t read_val = NRF24_ReadReg(dev, reg_addr);
    NRF24_WriteReg(dev, reg_addr, initial_val);

    return (read_val == check_val); // Вернет 1, если связь есть
}

void NRF24_ReadPayload(NRF24_t *dev, uint8_t *data, uint8_t size)
{
    uint8_t cmd = NRF24_CMD_R_RX_PAYLOAD;

    CSN_Select(dev);
    HAL_SPI_Transmit(dev->hspi, &cmd, 1, HAL_MAX_DELAY);      // Отправляем команду на чтение
    HAL_SPI_Receive(dev->hspi, data, size, HAL_MAX_DELAY);    // Забираем данные
    CSN_Unselect(dev);

    // После чтения полезно очистить FIFO, если не используем динамическую длину
    uint8_t flush_cmd = NRF24_CMD_FLUSH_RX;
    CSN_Select(dev);
    HAL_SPI_Transmit(dev->hspi, &flush_cmd, 1, HAL_MAX_DELAY);
    CSN_Unselect(dev);
}

void NRF24_SetAddress(NRF24_t *dev, uint8_t *addr)
{
    // Записываем адрес для передачи
    CSN_Select(dev);
    uint8_t cmd = NRF24_CMD_W_REGISTER | NRF24_REG_TX_ADDR;
    HAL_SPI_Transmit(dev->hspi, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(dev->hspi, addr, 5, HAL_MAX_DELAY);
    CSN_Unselect(dev);
    // Записываем такой же адрес для приема (Pipe 0)
    CSN_Select(dev);
    cmd = NRF24_CMD_W_REGISTER | NRF24_REG_RX_ADDR_P0;
    HAL_SPI_Transmit(dev->hspi, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(dev->hspi, addr, 5, HAL_MAX_DELAY);
    CSN_Unselect(dev);
}
void NRF24_WriteReg(NRF24_t *dev, uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {NRF24_CMD_W_REGISTER | reg, data};
    CSN_Select(dev);
    HAL_SPI_Transmit(dev->hspi, buf, 2, HAL_MAX_DELAY);
    CSN_Unselect(dev);
}

void NRF24_Mode_TX(NRF24_t *dev)
{
    HAL_GPIO_WritePin(dev->CE_Port, dev->CE_Pin, GPIO_PIN_RESET);
    NRF24_WriteReg(dev, NRF24_REG_CONFIG, 0x0E);
    HAL_Delay(1);
}

void NRF24_Mode_RX(NRF24_t *dev)
{
    NRF24_WriteReg(dev, NRF24_REG_CONFIG, 0x0F);
    HAL_GPIO_WritePin(dev->CE_Port, dev->CE_Pin, GPIO_PIN_SET);
}

void NRF24_Transmit(NRF24_t *dev, uint8_t *data)
{
	uint8_t cmd_flush = NRF24_CMD_FLUSH_TX;
	CSN_Select(dev);
	HAL_SPI_Transmit(dev->hspi, &cmd_flush, 1, HAL_MAX_DELAY);
	CSN_Unselect(dev);

	uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD;
    CSN_Select(dev);
    HAL_SPI_Transmit(dev->hspi, &cmd, 1, HAL_MAX_DELAY);
    HAL_SPI_Transmit(dev->hspi, data, 32, HAL_MAX_DELAY);
    CSN_Unselect(dev);

    HAL_GPIO_WritePin(dev->CE_Port, dev->CE_Pin, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(dev->CE_Port, dev->CE_Pin, GPIO_PIN_RESET);
}


