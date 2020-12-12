//
// Created by Ronan Gaillard on 03/12/2020.
//

#include "enc.h"
#include "main.h"
#include "helpers.h"

extern SPI_HandleTypeDef hspi2;

#define ENABLE_CHIP()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET)
#define DISABLE_CHIP()  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET)

static uint8_t encBank;
static uint16_t gNextPacketPtr;
static uint8_t erxfcon;

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


void encWriteWord(uint8_t address, uint16_t data) {
  encWrite(address, data & 0xff);
  encWrite(address + 1, data >> 8);
}

void encWrite(uint8_t address, uint8_t data) {
  // set the bank
  encSetBank(address);
  // do the write
  encWriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void encWriteOp(uint8_t op, uint8_t address, uint8_t data) {
  ENABLE_CHIP();

  encSendByte(op | (address & ADDR_MASK));
  encSendByte(data);

  DISABLE_CHIP();
}

void encWriteBuffer(uint16_t len, uint8_t *data) {
  ENABLE_CHIP();
  encSendByte(ENC28J60_WRITE_BUF_MEM);
  while (len--)
    encSendByte(*data++);

  DISABLE_CHIP();
}

void encSetBank(uint8_t address) {
  if ((address & BANK_MASK) != encBank) {
    encWriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
    encBank = address & BANK_MASK;
    encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, encBank >> 5);
  }
}

uint8_t encRead(uint8_t address) {
  // set the bank
  encSetBank(address);
  // do the read
  return encReadOp(ENC28J60_READ_CTRL_REG, address);
}

void netMoveTxpt(uint8_t buffer) {
  uint8_t start = buffer ? NET_XMIT_BUF1 : NET_XMIT_BUF2;
  encWriteOp(ENC_OP_WCR, EWRPTL, 0x00);
  encWriteOp(ENC_OP_WCR, EWRPTH, start);
}

void encSendPacket(uint8_t *packet, uint16_t length) {
  while (encReadOp(ENC28J60_READ_CTRL_REG, ECON1) & ECON1_TXRTS) {
    // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
    if ((encRead(EIR) & EIR_TXERIF)) {
      encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
      encWriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
    }
  }
  // TODO : check that no other packer is being sent
// Set the write pointer to start of transmit buffer area
  encWriteWord(EWRPTL, TXSTART_INIT);
  // Set the TXND pointer to correspond to the packet size given
  encWriteWord(ETXNDL, (TXSTART_INIT + length));
  // write per-packet control byte (0x00 means use macon3 settings)
  encWriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
  // copy the packet into the transmit buffer
  encWriteBuffer(length, packet);
  // send the contents of the transmit buffer onto the network
  encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
  // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
}

//void encInit() {
//  ENABLE_CHIP();
//
//  encWriteOp(ENC28J60_SOFT_RESET, 0,
//             ENC28J60_SOFT_RESET);
//  HAL_Delay(50);
//
//
//}

void encPhyWrite(uint8_t address, uint16_t data) {
  // set the PHY register address
  encWrite(MIREGADR, address);
  // write the PHY data
  encWrite(MIWRL, data);
  encWrite(MIWRH, data >> 8);
  // wait until the PHY write completes
  while (encRead(MISTAT) & MISTAT_BUSY) {
    HAL_Delay(15);
  }
}

void encoldInit() {
  encWriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
  HAL_Delay(50);
  // check CLKRDY bit to see if reset is complete
  // The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
  //while(!(encRead(ESTAT) & ESTAT_CLKRDY));
  // do bank 0 stuff
  // initialize receive buffer
  // 16-bit transfers, must write low byte first
  // set receive buffer start address
  gNextPacketPtr = RXSTART_INIT;
  // Rx start
  encWriteWord(ERXSTL, RXSTART_INIT);
  // set receive pointer address
  encWriteWord(ERXRDPTL, RXSTART_INIT);
  // RX end
  encWriteWord(ERXNDL, RXSTOP_INIT);
  // TX start
  encWriteWord(ETXSTL, TXSTART_INIT);
  // TX end
  encWriteWord(ETXNDL, TXSTOP_INIT);
  // do bank 1 stuff, packet filter:
  // For broadcast packets we allow only ARP packtets
  // All other packets should be unicast only for our mac (MAADR)
  //
  // The pattern to match on is therefore
  // Type     ETH.DST
  // ARP      BROADCAST
  // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
  // in binary these poitions are:11 0000 0011 1111
  // This is hex 303F->EPMM0=0x3f,EPMM1=0x30

  //encWrite(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
  //Change to add ERXFCON_BCEN recommended by epam
  //encWrite(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN|ERXFCON_BCEN);
  erxfcon = ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN;
  encWrite(ERXFCON, erxfcon);
  encWriteWord(EPMM0, 0x303f);
  encWriteWord(EPMCSL, 0xf7f9);
  //
  // do bank 2 stuff
  // enable MAC receive
  encWrite(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
  // bring MAC out of reset
  encWrite(MACON2, 0x00);
  // enable automatic padding to 60bytes and CRC operations
  encWriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);  //|MACON3_FULDPX);
  // set inter-frame gap (non-back-to-back)
  encWriteWord(MAIPGL, 0x0C12);
  // set inter-frame gap (back-to-back)
  encWrite(MABBIPG, 0x12);
  // Set the maximum packet size which the controller will accept
  // Do not send packets longer than MAX_FRAMELEN:
  encWriteWord(MAMXFLL, MAX_FRAMELEN);
  // do bank 3 stuff
  // write MAC address
  // NOTE: MAC address in ENC28J60 is byte-backward
  encWrite(MAADR5, 0x11);
  encWrite(MAADR4, 0x22);
  encWrite(MAADR3, 0x33);
  encWrite(MAADR2, 0x44);
  encWrite(MAADR1, 0x55);
  encWrite(MAADR0, 0x66);
  // no loopback of transmitted frames
  encPhyWrite(PHCON2, PHCON2_HDLDIS);
  // switch to bank 0
  encSetBank(ECON1);
  // enable interrutps
}

void encInit() {
  HAL_GPIO_WritePin(SPI_RST_GPIO_Port, SPI_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(SPI_RST_GPIO_Port, SPI_RST_Pin, GPIO_PIN_SET);


  encWriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
  HAL_Delay(50);
// set receive buffer start address
  gNextPacketPtr = RXSTART_INIT;

  // Rx start
  encWriteWord(ERXSTL, RXSTART_INIT);
  // set receive pointer address
  encWriteWord(ERXRDPTL, RXSTART_INIT);
  // RX end
  encWriteWord(ERXNDL, RXSTOP_INIT);
  // TX start
  encWriteWord(ETXSTL, TXSTART_INIT);
  // TX end
  encWriteWord(ETXNDL, TXSTOP_INIT);

  erxfcon = ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN | ERXFCON_BCEN;
  encWrite(ERXFCON, erxfcon);
  encWriteWord(EPMM0, 0x303f);
  encWriteWord(EPMCSL, 0xf7f9);
  //
  // do bank 2 stuff
  // enable MAC receive
  encWrite(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
  // bring MAC out of reset
  encWrite(MACON2, 0x00);
  // enable automatic padding to 60bytes and CRC operations
  encWriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);  //|MACON3_FULDPX);
  // set inter-frame gap (non-back-to-back)
  encWriteWord(MAIPGL, 0x0C12);
  // set inter-frame gap (back-to-back)
  encWrite(MABBIPG, 0x12);
  // Set the maximum packet size which the controller will accept
  // Do not send packets longer than MAX_FRAMELEN:
  encWriteWord(MAMXFLL, MAX_FRAMELEN);
  // do bank 3 stuff:

  uint8_t flags;


  do {
    flags = encRead(ESTAT);
  } while (!(flags & ENC_CLKRDY_bm));
  LOG("Oscillator  ready\n");


// assign initial MAC address to what the configuration specifies
// f6:d3:52:b6:8c:f0
  encWrite(MAADR0, 0x00);
  encWrite(MAADR1, 0xe0);
  encWrite(MAADR2, 0x4c);
  encWrite(MAADR3, 0x6d);
  encWrite(MAADR4, 0x11);
  encWrite(MAADR5, 0x08);

// no loopback of transmitted frames
  encPhyWrite(PHCON2, PHCON2_HDLDIS);
  // switch to bank 0
  encSetBank(ECON1);
  // enable interrutps
  //encWriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
  // enable packet reception
//  encWriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);

  // Set clock to 25MHz
//  encWrite(ECOCON, 25 & 0x7);

  int f;
  for (f = 0; f < 3; f++) {
    // 0x880 is PHLCON LEDB=on, LEDA=on
    // encPhyWrite(PHLCON,0b0011 1000 1000 00 00);
    encPhyWrite(PHLCON, 0x3880);
    HAL_Delay(500);

    // 0x990 is PHLCON LEDB=off, LEDA=off
    // encPhyWrite(PHLCON,0b0011 1001 1001 00 00);
    encPhyWrite(PHLCON, 0x3990);
    HAL_Delay(500);
  }

  encPhyWrite(PHLCON, 0x3476);
  HAL_Delay(100);
}

void encReadBuffer(uint16_t len, uint8_t *data) {
  ENABLE_CHIP();
  encSendByte(ENC28J60_READ_BUF_MEM);
  while (len--) {
    *data++ = encSendByte(0x00);
  }
  DISABLE_CHIP();
  // Remove next line suggested by user epam - not needed
//    *data='\0';
}

static uint16_t encReadBufferWord() {
  uint16_t result;
  encReadBuffer(2, (uint8_t *) &result);
  return result;
}

uint16_t encPacketReceive(uint16_t maxlen, uint8_t *packet) {
  uint16_t rxstat;
  uint16_t len;
  // check if a packet has been received and buffered
  //if( !(encRead(EIR) & EIR_PKTIF) ){
  // The above does not work. See Rev. B4 Silicon Errata point 6.
  if (encRead(EPKTCNT) == 0) {
    return (0);
  }

  // Set the read pointer to the start of the received packet
  encWriteWord(ERDPTL, gNextPacketPtr);
  // read the next packet pointer
  gNextPacketPtr = encReadBufferWord();
  // read the packet length (see datasheet page 43)
  len = encReadBufferWord() - 4;
  // read the receive status (see datasheet page 43)
  rxstat = encReadBufferWord();
  // limit retrieve length
  if (len > maxlen - 1) {
    len = maxlen - 1;
  }
  // check CRC and symbol errors (see datasheet page 44, table 7-3):
  // The ERXFCON.CRCEN is set by default. Normally we should not
  // need to check this.
  if ((rxstat & 0x80) == 0) {
    // invalid
    len = 0;
  } else {
    // copy the packet from the receive buffer
    encReadBuffer(len, packet);
  }
  // Move the RX read pointer to the start of the next received packet
  // This frees the memory we just read out
  encWriteWord(ERXRDPTL, gNextPacketPtr);
  // However, compensate for the errata point 13, rev B4: enver write an even address!
  if ((gNextPacketPtr - 1 < RXSTART_INIT)
      || (gNextPacketPtr - 1 > RXSTOP_INIT)) {
    encWriteWord(ERXRDPTL, RXSTOP_INIT);
  } else {
    encWriteWord(ERXRDPTL, (gNextPacketPtr - 1));
  }
  // decrement the packet counter indicate we are done with this packet
  encWriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
  return (len);
}


