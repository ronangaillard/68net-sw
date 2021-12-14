#include "main.h"
#include "scsi_protocol.h"
#include "helpers.h"
#include "scsi.h"
#include "enc.h"

extern uint8_t scsi_selected;
extern uint8_t scsi_selected_address;
extern uint8_t log_please;
extern uint8_t rsel_log;
extern uint8_t rbsy_log;

void handle_scsi_com()
{
    uint8_t requestId;

    if (RSEL() && !RRST())
    {
        requestId = RDB0_GPIO_Port->IDR;

        if ((~requestId & SCSI_ADDRESS) == SCSI_ADDRESS)
        {
            TBSY(GPIO_PIN_SET);
            scsi_selected = 1;
            HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
        }
    }
}