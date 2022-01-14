#ifndef SCSI_PROTOCOL_H
#define SCSI_PROTOCOL_H

#include <stdint.h>

void handle_scsi_com();
void reselect();

#define EnableTim()               (TIM6->CR1 |= TIM_CR1_CEN)
#define DisableTim()              (TIM6->CR1 &= (~((uint16_t)TIM_CR1_CEN)))

#define ClearTimCount()           (TIM6->CNT = 0)
#define ClearTimeFlag()          __HAL_TIM_CLEAR_FLAG(&htim6, TIM_SR_UIF);

#define WAIT()  \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop");

#define CYCLE_SLEEP(cycle_count) DWT->CYCCNT = 0; \
  while (1) \
  { \
    if (stopwatch_getticks() >= cycle_count) \
      break; \
  }

#define BUS_FREE() TDB0_GPIO_Port->ODR &= 0xff00; \
            TBSY(GPIO_PIN_RESET); \
            TDBP(GPIO_PIN_RESET); \
            TSEL(GPIO_PIN_RESET); \
            TMSG(GPIO_PIN_RESET); \
            TIO(GPIO_PIN_RESET); \
            TCD(GPIO_PIN_RESET); \
            TREQ(GPIO_PIN_RESET); \
            HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET)

#define MESSAGE_OUT() TCD(GPIO_PIN_SET); \
  TIO(GPIO_PIN_RESET); \
  TMSG(GPIO_PIN_SET)

#endif