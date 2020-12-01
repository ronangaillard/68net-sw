//
// Created by Ronan Gaillard on 30/11/2020.
//

#ifndef INC_68NET_HELPERS_H
#define INC_68NET_HELPERS_H

#include "usbd_cdc_if.h"

#define LOG(buffer) while(CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer)) != HAL_OK);
#endif //INC_68NET_HELPERS_H
