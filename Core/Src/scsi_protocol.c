#include "../Inc/main.h"
#include "../Inc/scsi_protocol.h"
#include "../Inc/helpers.h"
#include "../Inc/scsi.h"
#include "../Inc/enc.h"

void handle_scsi_com()
{
    uint8_t requestId;
     uint8_t status = 0;

    if (RSEL() || RBSY() || !RRST())
    {
        return;
    }

    requestId = RDB0_GPIO_Port->IDR;

    if ((~requestId & SCSI_ADDRESS) == SCSI_ADDRESS)
    {
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
        TBSY(GPIO_PIN_SET);
        // Wait for SEL inactive
        LOG_INT("SELECTED\r\n");
        while (!RSEL())
            ;

        if (!RATN())
        {
            LOG_INT("ATN !!\r\n");
            // Message out
            uint8_t message;
            TCD(GPIO_PIN_SET); // Data out (initiator to target)
            TMSG(GPIO_PIN_SET);
            message = readHandshake();
            LOG_INT("MESSAGE RECEIVED");

            TCD(GPIO_PIN_SET); // Data out (initiator to target)
            while (!RATN())
                ;

            TMSG(GPIO_PIN_RESET);
            TBSY(GPIO_PIN_RESET);
        }

        TCD(GPIO_PIN_SET);

        uint8_t cmd[12];

        cmd[0] = readHandshake();
        cmd[1] = readHandshake();
        cmd[2] = readHandshake();
        cmd[3] = readHandshake();
        cmd[4] = readHandshake();
        cmd[5] = readHandshake();

        static const int cmd_class_len[8] = {6, 10, 10, 6, 6, 12, 6, 6};
        int len = cmd_class_len[cmd[0] >> 5u];

        for (int i = 6; i < len; i++)
        {
            cmd[i] = readHandshake();
        }

        LOG_INT("CMD\r\n");

        switch (cmd[0])
        {
        case 0x12:
            LOG_INT("[Inquiry]\r\n");
            status = onInquiryCommand(cmd);
            break;
        case 0x09: // "Set Filter"
            LOG_INT("[Set Filter]\r\n");
            onSetFilter(cmd);
            break;
        case 0x0C: // "Medium Sense"
            LOG_INT("[Medium Sense]\r\n");
            break;
        case 0x05: // "Send Packet"
            LOG_INT("[Send Packet]\r\n");
            onSendPacket(cmd);
            break;
        default:
            LOG_INT("UNDEFINED CMD\r\n");
        }

        TIO(GPIO_PIN_SET);
        TCD(GPIO_PIN_SET);
        LOG_INT("status\r\n");
        writeHandshake(status);
        LOG_INT(" ok\r\n");
        if (!RATN())
            LOG_INT("ATN !!\r\n");

        // Message In
        TMSG(GPIO_PIN_SET);
        LOG_INT("message in\r\n");
        writeHandshake(0);
        LOG_INT(" ok\r\n");
        if (!RATN())
            LOG_INT("ATN !!\r\n");

        TCD(GPIO_PIN_RESET);
        TIO(GPIO_PIN_RESET);
        TMSG(GPIO_PIN_RESET);
        TBSY(GPIO_PIN_RESET);

        LOG_INT("BUS FREE\r\n");
    }
}