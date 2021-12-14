//
// Created by Ronan Gaillard on 30/11/2020.
//

#ifndef INC_68NET_HELPERS_H
#define INC_68NET_HELPERS_H

#include <stdarg.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void LOG(const char *format, ...);
void LOG_INT(const char* buffer);

#endif //INC_68NET_HELPERS_H
