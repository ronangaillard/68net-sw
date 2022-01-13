#include "main.h"
#include "scsi_protocol.h"
#include "helpers.h"
#include "scsi.h"
#include "enc.h"
#include "timing.h"

extern uint8_t scsi_selected;
extern uint8_t scsi_re_selected;
extern uint8_t scsi_selected_address;
extern uint8_t log_please;
extern uint8_t rsel_log;
extern uint8_t rbsy_log;
extern volatile uint8_t packet_received;
extern volatile uint8_t ready_to_message_packets;
extern TIM_HandleTypeDef htim6;

void handle_scsi_com()
{
    if (!RBSY())
    {
        uint8_t requestId;

        if (RSEL() && !RRST())
        {
            requestId = RDB0_GPIO_Port->IDR;

            if ((~requestId & SCSI_ADDRESS) == SCSI_ADDRESS)
            {
                TBSY(GPIO_PIN_SET);
                scsi_selected = 1;
                // HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
            }

            return;
        }

        if (!RSEL() && packet_received && !scsi_re_selected)
        {
            HAL_TIM_Base_Start_IT(&htim6);
        }
    }
    else
    {
        // someone is using the bus, do not try to reselect
        HAL_TIM_Base_Stop_IT(&htim6);
    }
}

void reselect()
{
    uint8_t requestId;

    if (RBSY()){
        HAL_TIM_Base_Stop_IT(&htim6);
        return;
    }
    // arbitration

    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);

    TBSY(GPIO_PIN_SET);
    TDB0_GPIO_Port->ODR &= 0xff00;
    TDB0_GPIO_Port->ODR |= SCSI_ADDRESS;

    DWT->CYCCNT = 0;

    while (1)
    {
        // 2400 ns
        if (stopwatch_getticks() >= 173)
            break;

        if (RSEL())
            break;
    }

    uint8_t block_mask = 0x00;
    uint8_t m = 1;
    for (uint8_t i = 0; i < 8; i++)
    {
        if (m > SCSI_ADDRESS)
            block_mask |= m;
        m = m << 1;
    }

    requestId = (~(RDB0_GPIO_Port->IDR)) & 0xff;

    if (!(requestId & block_mask))
    {
        // won arbitration
        TSEL(GPIO_PIN_SET);
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");

        DWT->CYCCNT = 0;

        while (1)
        {
            // 1200 ns
            if (stopwatch_getticks() >= 86)
                break;
        }

        TDB0_GPIO_Port->ODR |= 1 << 7;
        if (getParity(SCSI_ADDRESS | (1 << 7)))
        {
            TDBP(GPIO_PIN_SET);
        }
        else
        {
            TDBP(GPIO_PIN_RESET);
        }

        TIO(GPIO_PIN_SET);

        WAIT();
        WAIT();
        WAIT();
        WAIT();
        WAIT();

        TBSY(GPIO_PIN_RESET);

        WAIT();

        for (uint8_t i = 0; i < 250; i++)
        {
            if (RBSY())
            {
                break;
            }
        }

        if (!RBSY())
        {
            // mac did not understand it was selected
            TDB0_GPIO_Port->ODR &= 0xff00;
            TBSY(GPIO_PIN_RESET);
            TIO(GPIO_PIN_RESET);
            TDBP(GPIO_PIN_RESET);
            TSEL(GPIO_PIN_RESET);
            HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
            return;
        }

        WAIT()

        TBSY(GPIO_PIN_SET);

        WAIT()
        WAIT()

        TSEL(GPIO_PIN_RESET);

        WAIT()

        TDB0_GPIO_Port->ODR &= 0xff00;
        TIO(GPIO_PIN_RESET);

        scsi_re_selected = 1;

        HAL_TIM_Base_Stop_IT(&htim6);
    }
    else
    {
        // lost arbitration
        TDB0_GPIO_Port->ODR &= 0xff00;
        TBSY(GPIO_PIN_RESET);
        TDBP(GPIO_PIN_RESET);
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    }
}