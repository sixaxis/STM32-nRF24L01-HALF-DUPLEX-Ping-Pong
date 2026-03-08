# STM32 nRF24L01+ Ping-Pong Driver
!!!!*****ПЕРЕЗАГРУЗИТЕ ПЛАТУ ПЕРЕДАТЧИКА ПОСЛЕ ПРОШИВКИ*****!!!!

!!!!REBOOT THE TRANSMITTER BOARD AFTER FLASHING!!!!

[RU] Проект реализации приемопередатчика на базе STM32 и nRF24L01 с использованием конечного автомата (FSM) и прерываний.

[EN] A project implementing a transceiver based on STM32 and nRF24L01 using a State Machine (FSM) and interrupts.

---

## 🇷🇺 

Данный проект демонстрирует надежный способ работы с радиомодулем **nRF24L01+** через библиотеку **HAL**. Основная логика построена на переключении состояний между передачей (TX) и ожиданием ответа (RX).

### Основные фичи:
- **Обработка IRQ:** Использование внешнего прерывания (EXTI) для мгновенной реакции на события `Data Ready`, `Data Sent` и `Max Retransmissions`.
- **FSM (Конечный автомат):** Четкое разделение логики работы на состояния `STATE_TX` и `STATE_WAIT_RESPONSE`.
- **Retarget printf:** Весь лог работы (статус инициализации, успех передачи, входящие данные) выводится в UART.
- **Безопасность:** Очистка TX FIFO при достижении максимального количества попыток отправки (`MAX_RT`).

### Схема подключения :

| nRF24 Pin | STM32 Pin (Пример) |
|-----------|-------------------|
| VCC       | 3.3V              |
| GND       | GND               |
| CSN       | PB7  (GPIO_Output)|
| CE        | PB6  (GPIO_Output)|
| SCK       | PB3 (SPI1_SCK)    |
| MISO      | PB4 (SPI1_MISO)   |
| MOSI      | PB5 (SPI1_MOSI)   |
| IRQ       | PB8 (EXTI_IT)     |

---

## 🇺🇸 

This project demonstrates a robust way to interface with the **nRF24L01+** radio module using the **STM32 HAL** library. The core logic is built around a state machine that toggles between transmission (TX) and waiting for a response (RX).

### Key Features:
- **IRQ Handling:** Utilizes external interrupts (EXTI) to respond instantly to `Data Ready`, `Data Sent`, and `Max Retransmissions` events.
- **FSM (State Machine):** Clean logic separation into `STATE_TX` and `STATE_WAIT_RESPONSE`.
- **Retarget printf:** Real-time logging of initialization status, transmission success, and incoming payloads via UART.
- **Error Handling:** Automatic TX FIFO flushing upon reaching the maximum retry limit (`MAX_RT`).

### Logic Flow:
1. **Init:** Checks if the NRF module is alive.
2. **TX State:** Sends "Ping" and waits for hardware confirmation.
3. **RX State:** Switches to receiver mode with a 200ms timeout to catch the response.
4. **Callback:** The `HAL_GPIO_EXTI_Callback` handles status register clearing and event signaling.

---

## 🛠 Tech Stack
- **MCU:** STM32 (Cortex-M4/M1/M0)
- **Library:** STM32Cube HAL
- **Interface:** SPI + UART (Debug)
- **Toolchain:** STM32CubeIDE / Keil
