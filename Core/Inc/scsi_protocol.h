#ifndef SCSI_PROTOCOL_H
#define SCSI_PROTOCOL_H

void handle_scsi_com();

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

#endif