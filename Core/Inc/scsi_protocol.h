#ifndef SCSI_PROTOCOL_H
#define SCSI_PROTOCOL_H

void handle_scsi_com();
void reselect();

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