//
// Created by Ronan Gaillard on 03/12/2020.
//

#include "enc.h"
#include "main.h"
#include "helpers.h"

extern SPI_HandleTypeDef hspi2;

#define ENABLE_CHIP()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define DISABLE_CHIP()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)


uint8_t encSendByte(uint8_t buffer) {
  uint8_t result;

  uint8_t status = HAL_SPI_TransmitReceive(&hspi2, &buffer, &result,
                                           1, HAL_MAX_DELAY);

  if (status != HAL_OK)
    LOG("Error on SPI");

  return result;
}

uint8_t encReadOp(uint8_t op, uint8_t address) {
  uint8_t tmp;

  ENABLE_CHIP();

  encSendByte(op | (address & ADDR_MASK));
  tmp = encSendByte(0xff);

  if (address & 0x80)
    tmp = encSendByte(0xff);

  DISABLE_CHIP();

  return tmp;
}
