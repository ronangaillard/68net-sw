//
// Created by Ronan Gaillard on 02/12/2020.
//


#include <stdio.h>
#include "helpers.h"

#include "usbd_cdc_if.h"

void LOG(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char buffer[50];
  sprintf(buffer, format, args);
  while (CDC_Transmit_FS((uint8_t *) buffer, strlen(buffer)) != HAL_OK);
  va_end(args);
}