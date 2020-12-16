//
// Created by Ronan Gaillard on 30/11/2020.
//

#include "main.h"
#include "scsi.h"
#include "helpers.h"
#include <stdio.h>
#include <inttypes.h>
#include "enc.h"
#include <stdlib.h>

// the selector for the TX buffer space
static uint8_t txbuf;

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
  TDB0_GPIO_Port->ODR &= 0xff00;
  TDB0_GPIO_Port->ODR |= d;

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
  char buf[50];
  sprintf(buf, "Wrote %d bytes\n", len);
  LOG(buf);
}

uint8_t onInquiryCommand(const uint8_t *cmd) {
  static uint8_t buf[96] = {
          // bytes 0-35 are the standard inquiry data
          0x09, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00,
          'N', 'u', 'v', 'o', 't', 'e', 'c', 'h',
          'N', 'u', 'v', 'o', 'S', 'C', 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          '1', '.', '1', 'r',
          // 36-95 are the extended page 2 stuff
          // ROM MAC
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          // 14 bytes of 0x00
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          // configured MAC
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          // 34 bytes of 0x00
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00
  };

  uint16_t alloc = ((cmd[3] & 1) << 8) + cmd[4];

  char log[50];
  sprintf(log, "Alloc %hu bytes\n", alloc);
  LOG(log);

  writeDataPhase(alloc, buf);
  return 0x00;
}

void onSetFilter(const uint8_t *cmd) {
  uint8_t data[8] = {0}; // init to 0x00 on all
  uint8_t alloc = cmd[4];
  if (alloc > 8) alloc = 8;

  TCD(GPIO_PIN_RESET); // Data out (initiator to target)

  for (uint8_t i = 0; i < alloc; i++) {
    data[i] = readHandshake();
  }

  TCD(GPIO_PIN_SET);
}

void onSendPacket(const uint8_t *cmd) {
  // parse the packet header, limiting total length to 2047
  int16_t packetLength = ((cmd[3] & 7) << 8) + cmd[4];
  if (packetLength > MAXIMUM_TRANSFER_LENGTH) {
    packetLength = MAXIMUM_TRANSFER_LENGTH;
  }
  LOG("packet length %" PRIu16 " bytes\n", packetLength);
  // read stream from initiator
  uint8_t packet[MAXIMUM_TRANSFER_LENGTH];

  TCD(GPIO_PIN_RESET); // Data out (initiator to target)

  for (uint16_t i = 0; i < packetLength; i++) {
    packet[i] = readHandshake();
    LOG("%x, ", packet[i]);

  }
  LOG("\n");
  LOG("packet received\n");
  int j = 0;
  uint8_t macaddr[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  while (j < 6) {
    packet[ETH_SRC_MAC + j] = macaddr[j];
    j++;
  }
  LOG("modified packet : \n");

  for (uint16_t i = 0; i < packetLength; i++) {

    LOG("%x, ", packet[i]);

  }
  LOG("\n");

  TCD(GPIO_PIN_SET);

  encSendPacket(packet, packetLength);

  LOG("packet sent\n");
}
