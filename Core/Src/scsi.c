//
// Created by Ronan Gaillard on 30/11/2020.
//

#include "main.h"
#include "scsi.h"
#include "helpers.h"

uint8_t readBus() {
  return (~(RDB0_GPIO_Port->IDR)) & 0xff;
}

int getParity(unsigned int n) {
  int parity = 0;
  while (n) {
    parity = !parity;
    n = n & (n - 1);
  }
  return parity;
}

uint8_t readHandshake(void) {
  TREQ(GPIO_PIN_SET);
  while (RACK());
  uint8_t r = readBus();
  TREQ(GPIO_PIN_RESET);
  while (!RACK());
  return r;
}

void writeHandshake(uint8_t d) {
  TDB0_GPIO_Port->ODR = d;

  if (getParity(d)) {
    TDBP(GPIO_PIN_SET);
  } else {
    TDBP(GPIO_PIN_RESET);
  }

  TREQ(GPIO_PIN_SET);
  while (RACK());
  TREQ(GPIO_PIN_RESET);
  while (!RACK());
}

void writeDataPhase(int len, const uint8_t *p) {
  LOG("DATAIN PHASE\n");
  TCD(GPIO_PIN_RESET);
  TIO(GPIO_PIN_SET);
  for (int i = 0; i < len; i++) {
    writeHandshake(p[i]);
    LOG(".");
  }
  LOG("\n");
}

uint8_t onInquiryCommand(uint8_t len) {
  uint8_t buf[36] = {
          0x09,   //device type
          0x00,   //RMB = 0
          0x01,   //ISO, ECMA, ANSI version
          0x01,   //Response data format
          35 - 4, //Additional data length
          0,
          0,    //Reserve
          0x00, //Support function
          'R',
          'O',
          'N',
          'A',
          'N',
          ' ',
          ' ',
          ' ',
          '6',
          '8',
          'N',
          'E',
          'T',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          ' ',
          '0',
          '.',
          '1',
          ' ',
  };
  writeDataPhase(len < 36 ? len : 36, buf);
  return 0x00;
}