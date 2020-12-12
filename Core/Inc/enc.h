//
// Created by Ronan Gaillard on 03/12/2020.
//

#ifndef INC_68NET_ENC_H
#define INC_68NET_ENC_H

#include <stdint.h>

#define ADDR_MASK 0x1f
#define BANK_MASK        0x60
#define SPRD_MASK        0x80

#define OP_READ_CTRL_REG  0x00

uint8_t encSendByte(uint8_t buffer);

uint8_t encReadOp(uint8_t op, uint8_t address);

void encWriteWord(uint8_t address, uint16_t data);

void encWrite(uint8_t address, uint8_t data);

void encWriteOp(uint8_t op, uint8_t address, uint8_t data);

void encSetBank(uint8_t address);

void netMoveTxpt(uint8_t buffer);

void encSendPacket(uint8_t *packet, uint16_t length);

void encWriteBuffer(uint16_t len, uint8_t *data);

void encInit();

uint16_t encPacketReceive(uint16_t maxlen, uint8_t *packet);

#define _BV(n) (1 << n)

// The RXSTART_INIT should be zero. See Rev. B4 Silicon Errata
// buffer boundaries applied to internal 8K ram
// the entire available packet buffer space is allocated
//
// start with recbuf at 0/
#define RXSTART_INIT     0x0
// receive buffer end
#define RXSTOP_INIT      (0x1FFF-0x0600-1)
// start TX buffer at 0x1FFF-0x0600, pace for one full ethernet frame (~1500 bytes)
#define TXSTART_INIT     (0x1FFF-0x0600)
// stp TX buffer at end of mem
#define TXSTOP_INIT      0x1FFF
//
// max frame length which the conroller will accept:
#define        MAX_FRAMELEN        1500        // (note: maximum ethernet frame length would be 1518)
//#define MAX_FRAMELEN     600

// All-bank registers
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
// Bank 0 registers
#define ERDPTL           (0x00|0x00)
#define ERDPTH           (0x01|0x00)
#define EWRPTL           (0x02|0x00)
#define EWRPTH           (0x03|0x00)
#define ETXSTL           (0x04|0x00)
#define ETXSTH           (0x05|0x00)
#define ETXNDL           (0x06|0x00)
#define ETXNDH           (0x07|0x00)
#define ERXSTL           (0x08|0x00)
#define ERXSTH           (0x09|0x00)
#define ERXNDL           (0x0A|0x00)
#define ERXNDH           (0x0B|0x00)
#define ERXRDPTL         (0x0C|0x00)
#define ERXRDPTH         (0x0D|0x00)
#define ERXWRPTL         (0x0E|0x00)
#define ERXWRPTH         (0x0F|0x00)
#define EDMASTL          (0x10|0x00)
#define EDMASTH          (0x11|0x00)
#define EDMANDL          (0x12|0x00)
#define EDMANDH          (0x13|0x00)
#define EDMADSTL         (0x14|0x00)
#define EDMADSTH         (0x15|0x00)
#define EDMACSL          (0x16|0x00)
#define EDMACSH          (0x17|0x00)
// Bank 1 registers
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCSL           (0x10|0x20)
#define EPMCSH           (0x11|0x20)
#define EPMOL            (0x14|0x20)
#define EPMOH            (0x15|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)
// Bank 2 registers
#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPGL           (0x06|0x40|0x80)
#define MAIPGH           (0x07|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFLL          (0x0A|0x40|0x80)
#define MAMXFLH          (0x0B|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWRL            (0x16|0x40|0x80)
#define MIWRH            (0x17|0x40|0x80)
#define MIRDL            (0x18|0x40|0x80)
#define MIRDH            (0x19|0x40|0x80)
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCSL          (0x08|0x60)
#define EBSTCSH          (0x09|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUSL           (0x18|0x60)
#define EPAUSH           (0x19|0x60)
// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01
// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

// SPI operation codes
#define ENC28J60_READ_CTRL_REG       0x00
#define ENC28J60_READ_BUF_MEM        0x3A
#define ENC28J60_WRITE_CTRL_REG      0x40
#define ENC28J60_WRITE_BUF_MEM       0x7A
#define ENC28J60_BIT_FIELD_SET       0x80
#define ENC28J60_BIT_FIELD_CLR       0xA0
#define ENC28J60_SOFT_RESET          0xFF
#define ENC_OP_RCR 0x00
#define ENC_OP_RBM 0x3A
#define ENC_OP_WCR 0x40
#define ENC_OP_WBM 0x7A
#define ENC_OP_BFS 0x80
#define ENC_OP_BFC 0xA0
#define ENC_OP_SRC 0xFF

/*
 * Defines the high byte value for end of the region where the receive buffer
 * is, starting at 0x000 and extending through 0xXXFF, where 0xXX is this
 * value. All remaining space is allocated to the transmit buffer.
 */
#define NET_ERXNDH_VALUE    0x13

/*
 * Defines the starting point where packets to be transmitted are stored.
 * There are two regions, each 1536 bytes in size reserved for this purpose.
 * They can be switched between in the transmit functions based on the provided
 * buffer selection value.
 */
#define NET_XMIT_BUF1       0x14
#define NET_XMIT_BUF2       0x1A

/*
 * Defines the array offsets within received data where the various data
 * structures are supposed to live, indexed from zero.
 */
#define NET_HEAD_RXPTL      0
#define NET_HEAD_RXPTH      1
#define NET_HEAD_RXLENL     2
#define NET_HEAD_RXLENH     3
#define NET_HEAD_STATL      4
#define NET_HEAD_STATH      5

/*
 * Bit flags for the above registers.
 */

// EIE
#define ENC_INTIE_bm _BV(7)
#define ENC_PKTIE_bm _BV(6)
#define ENC_DMAIE_bm _BV(5)
#define ENC_LINKIE_bm _BV(4)
#define ENC_TXIE_bm _BV(3)
#define ENC_TXERIE_bm _BV(1)
#define ENC_RXERIE_bm _BV(0)

// EIR
#define ENC_PKTIF_bm _BV(6)
#define ENC_DMAIF_bm _BV(5)
#define ENC_LINKIF_bm _BV(4)
#define ENC_TXIF_bm _BV(3)
#define ENC_TXERIF_bm _BV(1)
#define ENC_RXERIF_bm _BV(0)

// ESTAT
#define ENC_INT_bm _BV(7)
#define ENC_BUFER_bm _BV(6)
#define ENC_LATECOL_bm _BV(4)
#define ENC_RXBUSY_bm _BV(2)
#define ENC_TXABRT_bm _BV(1)
#define ENC_CLKRDY_bm _BV(0)

// ECON2
#define ENC_AUTOINC_bm _BV(7)
#define ENC_PKTDEC_bm _BV(6)
#define ENC_PWRSV_bm _BV(5)
#define ENC_VRPS_bm _BV(3)

// ECON1
#define ENC_TXRST_bm _BV(7)
#define ENC_RXRST_bm _BV(6)
#define ENC_DMAST_bm _BV(5)
#define ENC_CSUMEN_bm _BV(4)
#define ENC_TXRTS_bm _BV(3)
#define ENC_RXEN_bm _BV(2)
#define ENC_BSEL1_bm _BV(1)
#define ENC_BSEL0_bm _BV(0)

// ERXFCON
#define ENC_UCEN_bm _BV(7)
#define ENC_ANDOR_bm _BV(6)
#define ENC_CRCEN_bm _BV(5)
#define ENC_PMEN_bm _BV(4)
#define ENC_MPEN_bm _BV(3)
#define ENC_HTEN_bm _BV(2)
#define ENC_MCEN_bm _BV(1)
#define ENC_BCEN_bm _BV(0)

// MACON1
#define ENC_TXPAUS_bm _BV(3)
#define ENC_RXPAUS_bm _BV(2)
#define ENC_PASSALL_bm _BV(1)
#define ENC_MARXEN_bm _BV(0)

// MACON3
#define ENC_PADCFG2_bm _BV(7)
#define ENC_PADCFG1_bm _BV(6)
#define ENC_PADCFG0_bm _BV(5)
#define ENC_TXCRCEN_bm _BV(4)
#define ENC_PHDREN_bm _BV(3)
#define ENC_HFRMEN_bm _BV(2)
#define ENC_FRMLNEN_bm _BV(1)
#define ENC_FULDPX_bm _BV(0)

// MACON4
#define ENC_DEFER_bm _BV(6)
#define ENC_BPEN_bm _BV(5)
#define ENC_NOBKOFF_bm _BV(4)

// EBSTCON
#define ENC_PSV2_bm _BV(7)
#define ENC_PSV1_bm _BV(6)
#define ENC_PSV0_bm _BV(5)
#define ENC_PSEL_bm _BV(4)
#define ENC_TMSEL1_bm _BV(3)
#define ENC_TMSEL0_bm _BV(2)
#define ENC_TME_bm _BV(1)
#define ENC_BISTST_bm _BV(0)

// MICMD
#define ENC_MIISCAN_bm _BV(1)
#define ENC_MIIRD_bm _BV(0)

// MISTAT
#define ENC_NVALID_bm _BV(2)
#define ENC_SCAN_bm _BV(1)
#define ENC_BUSY_bm _BV(0)

// ECOCON
#define ENC_COCON2_bm _BV(2)
#define ENC_COCON1_bm _BV(1)
#define ENC_COCON0_bm _BV(0)

// EFLOCON
#define ENC_FULDPXS_bm _BV(2)
#define ENC_FCEN1_bm _BV(1)
#define ENC_FCEN0_bm _BV(0)

// start TX buffer at 0x1FFF-0x0600, pace for one full ethernet frame (~1500 bytes)
#define TXSTART_INIT     (0x1FFF-0x0600)

// ******* ETH *******
#define ETH_HEADER_LEN  14
// values of certain bytes:
#define ETHTYPE_ARP_H_V 0x08
#define ETHTYPE_ARP_L_V 0x06
#define ETHTYPE_IP_H_V  0x08
#define ETHTYPE_IP_L_V  0x00
// byte positions in the ethernet frame:
//
// Ethernet type field (2bytes):
#define ETH_TYPE_H_P 12
#define ETH_TYPE_L_P 13
//
#define ETH_DST_MAC 0
#define ETH_SRC_MAC 6


// ******* ARP *******
#define ETH_ARP_OPCODE_REPLY_H_V 0x0
#define ETH_ARP_OPCODE_REPLY_L_V 0x02
#define ETH_ARP_OPCODE_REQ_H_V 0x0
#define ETH_ARP_OPCODE_REQ_L_V 0x01
// start of arp header:
#define ETH_ARP_P 0xe
//
#define ETHTYPE_ARP_L_V 0x06
// arp.dst.ip
#define ETH_ARP_DST_IP_P 0x26
// arp.opcode
#define ETH_ARP_OPCODE_H_P 0x14
#define ETH_ARP_OPCODE_L_P 0x15
// arp.src.mac
#define ETH_ARP_SRC_MAC_P 0x16
#define ETH_ARP_SRC_IP_P 0x1c
#define ETH_ARP_DST_MAC_P 0x20
#define ETH_ARP_DST_IP_P 0x26

// ******* IP *******
#define IP_HEADER_LEN    20

#define IP_PROTO_ICMP_V    0x01
#define IP_PROTO_TCP_V    0x06
#define IP_PROTO_UDP_V    0x11
#define IP_V4_V        0x40
#define IP_HEADER_LENGTH_V  0x05

#define IP_P        0x0E
#define IP_HEADER_VER_LEN_P  0x0E
#define IP_TOS_P      0x0F
#define IP_TOTLEN_H_P    0x10
#define IP_TOTLEN_L_P    0x11
#define IP_ID_H_P      0x12
#define IP_ID_L_P      0x13
#define IP_FLAGS_P      0x14
#define IP_FLAGS_H_P    0x14
#define IP_FLAGS_L_P    0x15
#define IP_TTL_P      0x16
#define IP_PROTO_P      0x17
#define IP_CHECKSUM_P    0x18
#define IP_CHECKSUM_H_P    0x18
#define IP_CHECKSUM_L_P    0x19
#define IP_SRC_IP_P      0x1A
#define IP_DST_IP_P      0x1E

#define IP_SRC_P      0x1a
#define IP_DST_P      0x1e
#define IP_HEADER_LEN_VER_P 0xe

// ip.src
/*
#define IP_SRC_P 0x1a
#define IP_DST_P 0x1e
#define IP_P 0xe
#define IP_HEADER_LEN_VER_P 0xe
#define IP_CHECKSUM_P 0x18
#define IP_TTL_P 0x16
#define IP_FLAGS_P 0x14
#define IP_TOTLEN_H_P 0x10
#define IP_TOTLEN_L_P 0x11
#define IP_PROTO_P 0x17
#define IP_PROTO_ICMP_V 1
#define IP_PROTO_TCP_V 6
// 17=0x11
#define IP_PROTO_UDP_V 17
#define IP_V4_V         0x40
*/
// ******* ICMP *******
#define ICMP_TYPE_ECHOREPLY_V 0
#define ICMP_TYPE_ECHOREQUEST_V 8
//
#define ICMP_TYPE_P 0x22
#define ICMP_CHECKSUM_P 0x24
#define ICMP_CHECKSUM_H_P 0x24
#define ICMP_CHECKSUM_L_P 0x25
#define ICMP_IDENT_H_P 0x26
#define ICMP_IDENT_L_P 0x27
#define ICMP_DATA_P 0x2a

#endif //INC_68NET_ENC_H
