//
// Created by Ronan Gaillard on 30/11/2020.
//

#ifndef INC_68NET_SCSI_H
#define INC_68NET_SCSI_H

#define SCSI_ADDRESS 1u << 5u
#define MAXIMUM_TRANSFER_LENGTH 1522

#define RRST() HAL_GPIO_ReadPin(RRST_GPIO_Port, RRST_Pin)
#define RBSY() HAL_GPIO_ReadPin(RBSY_GPIO_Port, RBSY_Pin)
#define RSEL() HAL_GPIO_ReadPin(RSEL_GPIO_Port, RSEL_Pin)
#define RATN() HAL_GPIO_ReadPin(RATN_GPIO_Port, RATN_Pin)
#define RACK() HAL_GPIO_ReadPin(RACK_GPIO_Port, RACK_Pin)
#define RDBP() HAL_GPIO_ReadPin(RDBP_GPIO_Port, RDBP_Pin)

#define TBSY(status) HAL_GPIO_WritePin(TBSY_GPIO_Port, TBSY_Pin, status)
#define TSEL(status) HAL_GPIO_WritePin(TSEL_GPIO_Port, TSEL_Pin, status)
#define TCD(status) HAL_GPIO_WritePin(TCD_GPIO_Port, TCD_Pin, status)
#define TIO(status) HAL_GPIO_WritePin(TIO_GPIO_Port, TIO_Pin, status)
#define TREQ(status) HAL_GPIO_WritePin(TREQ_GPIO_Port, TREQ_Pin, status)
#define TDBP(status) HAL_GPIO_WritePin(TDBP_GPIO_Port, TDBP_Pin, status)
#define TMSG(status) HAL_GPIO_WritePin(TMSG_GPIO_Port, TMSG_Pin, status)

uint8_t readHandshake();

uint8_t readHandshakeMessageOut(void);

void writeHandshake(uint8_t d);

void writeDataPhase(int len, const uint8_t *p);

uint8_t onInquiryCommand(const uint8_t *cmd);

void onSetFilter(const uint8_t *cmd);

void onSendPacket(const uint8_t *cmd);

int getParity(unsigned int n);

#endif //INC_68NET_SCSI_H
