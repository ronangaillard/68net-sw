/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "helpers.h"
#include "scsi.h"
#include "enc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
volatile uint8_t scsi_selected = 0;
volatile uint8_t scsi_re_selected = 0;
volatile uint8_t scsi_selected_address = 0;
volatile uint8_t log_please = 0;
volatile uint8_t rsel_log = 0;
volatile uint8_t rbsy_log = 0;
volatile uint8_t packet_received = 0;

volatile uint8_t ready_to_message_packets = 0;

uint8_t packet_id = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void fill_buf_p(uint8_t *buf, uint16_t len, const char *s)
{
  // fill in tcp data at position pos
  //
  // with no options the data starts after the checksum + 2 more bytes (urgent ptr)
  while (len)
  {
    *buf = *s;
    buf++;
    s++;
    len--;
  }
}

const char arpreqhdr[] = {0, 1, 8, 0, 6, 4, 0, 1};
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */

  // Init SCSI transmit pins
  HAL_GPIO_WritePin(TDB0_GPIO_Port, TDB0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB1_GPIO_Port, TDB1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB2_GPIO_Port, TDB2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB3_GPIO_Port, TDB3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB4_GPIO_Port, TDB4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB5_GPIO_Port, TDB5_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB6_GPIO_Port, TDB6_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB7_GPIO_Port, TDB7_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDBP_GPIO_Port, TDBP_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(TREQ_GPIO_Port, TREQ_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TMSG_GPIO_Port, TMSG_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TCD_GPIO_Port, TCD_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TIO_GPIO_Port, TIO_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TBSY_GPIO_Port, TBSY_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TSEL_GPIO_Port, TSEL_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t requestId;
  char buffer[20];
  uint8_t status;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  HAL_GPIO_WritePin(SPI_RST_GPIO_Port, SPI_RST_Pin, GPIO_PIN_SET);
  encInit();
  uint8_t arp[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xE0, 0x4C, 0x6D, 0x11, 0x08, 0x08, 0x06, 0x00, 0x01, 0x08,
                   0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0xE0, 0x4C, 0x6D, 0x11, 0x08, 0xC0, 0xA8, 0x11, 0x02, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0xC0, 0xA8, 0x11, 0x01};
  uint8_t buf[MAX_PACKET_SIZE];
  uint16_t buf_size = 0;
  GPIO_PinState ps = GPIO_PIN_RESET;
  uint8_t macaddr[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  uint8_t ip_we_search[] = {192, 168, 17, 1};
  uint8_t ipaddr[] = {192, 168, 17, 17};
  //    encSendPacket(arp, 60);
  uint8_t i = 0;
  //
  while (i < 6)
  {
    buf[ETH_DST_MAC + i] = 0xff;
    buf[ETH_SRC_MAC + i] = macaddr[i];
    i++;
  }
  buf[ETH_TYPE_H_P] = ETHTYPE_ARP_H_V;
  buf[ETH_TYPE_L_P] = ETHTYPE_ARP_L_V;
  fill_buf_p(&buf[ETH_ARP_P], 8, arpreqhdr);
  i = 0;
  while (i < 6)
  {
    buf[ETH_ARP_SRC_MAC_P + i] = macaddr[i];
    buf[ETH_ARP_DST_MAC_P + i] = 0;
    i++;
  }
  i = 0;
  while (i < 4)
  {
    buf[ETH_ARP_DST_IP_P + i] = *(ip_we_search + i);
    buf[ETH_ARP_SRC_IP_P + i] = ipaddr[i];
    i++;
  }

  // 0x2a=42=len of packet
  // encSendPacket(buf, 0x2a);
  // LOG("sent\r\n");
  //  while (1) {
  ////    encSendPacket(arp, 60);

  // 0x2a=42=len of packet
  //  encSendPacket(buf, 42);
  //  }

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Wait for reset
    status = 0;

    uint16_t size = encPacketReceive(MAX_PACKET_SIZE, buf);
    if (size != 0)
    {
      LOG("received packet of %hu bytes\r\n", size);
      buf_size = size;
      packet_received = 1;
    }

    if (scsi_selected)
    {
      LOG("SELECTED:");

      while (RSEL())
        ;

      LOG(" ok\r\n");

      if (RATN())
      {
        LOG("ATN !!\r\n");
        // Message out
        uint8_t message;
        TCD(GPIO_PIN_SET); // Data out (initiator to target)
        TMSG(GPIO_PIN_SET);
        message = readHandshake();
        LOG("MESSAGE : 0x%02X\n", message & 0xff);

        TCD(GPIO_PIN_SET); // Data out (initiator to target)
        while (RATN())
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

      sprintf(buffer, "CMD : 0x%02X\r\n", cmd[0]);
      LOG(buffer);

      switch (cmd[0])
      {
      case 0x12:
        LOG("[Inquiry]\r\n");
        status = onInquiryCommand(cmd);
        ready_to_message_packets = 1;
        break;
      case 0x09: // "Set Filter"
        LOG("[Set Filter]\r\n");
        onSetFilter(cmd);
        break;
      case 0x0C: // "Medium Sense"
        LOG("[Medium Sense]\r\n");
        break;
      case 0x05: // "Send Packet"
        LOG("[Send Packet]\r\n");
        onSendPacket(cmd);
        break;
      default:
        LOG("UNDEFINED CMD\r\n");
      }

      // Status
      TIO(GPIO_PIN_SET);
      TCD(GPIO_PIN_SET);
      LOG("status (%x) ...", status);
      writeHandshake(status);
      LOG(" ok\r\n");
      if (RATN())
        LOG("ATN !!\r\n");

      // bug avant cette ligne
      // Message In
      TMSG(GPIO_PIN_SET);
      LOG("message in (%x) ...", 0);
      writeHandshake(0);
      LOG(" ok\r\n");
      if (RATN())
        LOG("ATN !!\r\n");

      TCD(GPIO_PIN_RESET);
      TIO(GPIO_PIN_RESET);
      TMSG(GPIO_PIN_RESET);
      TBSY(GPIO_PIN_RESET);

      LOG("BUS FREE\r\n");
      scsi_selected = 0;

      TDB0_GPIO_Port->ODR &= 0xff00;
      TCD(GPIO_PIN_RESET);
      TIO(GPIO_PIN_RESET);
      TMSG(GPIO_PIN_RESET);
      TREQ(GPIO_PIN_RESET);
      TDBP(GPIO_PIN_RESET);
      TBSY(GPIO_PIN_RESET);

      HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    }

    if (scsi_re_selected)
    {
      LOG("WON ARBITRATION AFTER RESELECTION for packet of size : %hu\r\n", buf_size);

      LOG("MESSAGE OUT : ");

      // message out
      TCD(GPIO_PIN_SET);
      TIO(GPIO_PIN_RESET);
      TMSG(GPIO_PIN_SET);

      uint8_t msg = readHandshake();
      LOG("0x%x\r\n", msg);

      if (msg == 0xC0)
      {
        LOG("BUS FREE after 0xC0\r\n");

        TDB0_GPIO_Port->ODR &= 0xff00;
        TCD(GPIO_PIN_RESET);
        TIO(GPIO_PIN_RESET);
        TMSG(GPIO_PIN_RESET);
        TREQ(GPIO_PIN_RESET);
        TDBP(GPIO_PIN_RESET);
        TBSY(GPIO_PIN_RESET);

        scsi_re_selected = 0;
        HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);

        continue;
      }

      LOG("DATA IN\r\n");

      // transmit packet
      // data in
      TCD(GPIO_PIN_RESET);
      TIO(GPIO_PIN_SET);
      TMSG(GPIO_PIN_RESET);

      // flag byte
      writeHandshake(1);
      LOG(".");

      // id byte
      writeHandshake(packet_id);
      packet_id++;
      LOG(".");

      // packet size
      writeHandshake(buf_size & 0xff);
      LOG(".");
      writeHandshake((buf_size >> 8) & 0xff);
      LOG(".");

      // packet
      for (uint16_t i = 0; i < buf_size; i++)
      {
        writeHandshake(buf[i]);
        LOG(".");
      }
      LOG("\r\n");

      LOG("MESSAGE OUT : ");

      // message out
      TCD(GPIO_PIN_SET);
      TIO(GPIO_PIN_RESET);
      TMSG(GPIO_PIN_SET);

      msg = readHandshake();
      LOG("0x%x\r\n", msg);

      LOG("MESSAGE IN\r\n");
      // message in

      TCD(GPIO_PIN_SET);
      TIO(GPIO_PIN_SET);
      TMSG(GPIO_PIN_SET);

      // disconnect
      writeHandshake(0x04);

      LOG("BUS FREE\r\n");

      TDB0_GPIO_Port->ODR &= 0xff00;
      TCD(GPIO_PIN_RESET);
      TIO(GPIO_PIN_RESET);
      TMSG(GPIO_PIN_RESET);
      TREQ(GPIO_PIN_RESET);
      TDBP(GPIO_PIN_RESET);
      TBSY(GPIO_PIN_RESET);

      scsi_re_selected = 0;

      packet_received = 0;

      HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    }
    // HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    //    while (1);
    // while (RSEL() || RBSY() || !RRST()) {
    // //  int size = encPacketReceive(400, buf);
    //  int size = 0;
    //  if (size != 0) {
    //    LOG("received packet of %d bytes\n", size);
    //    for (int k = 0; k < size; k++)
    //      LOG("%02X ", buf[k]);

    //    LOG("\r\n");
    //  }
    // }

    //     requestId = RDB0_GPIO_Port->IDR;
    //     if ((~requestId & SCSI_ADDRESS) == SCSI_ADDRESS) {
    //       HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
    //       TBSY(GPIO_PIN_SET);
    //       // Wait for SEL inactive
    //       LOG("SELECTED\r\n");
    //       while (!RSEL());

    //       if (!RATN()) {
    //         LOG("ATN !!\r\n");
    //         // Message out
    //         uint8_t message;
    //         TCD(GPIO_PIN_SET); // Data out (initiator to target)
    //         TMSG(GPIO_PIN_SET);
    //         message = readHandshake();
    //         LOG("MESSAGE : 0x%02X\n", message & 0xff);

    //         TCD(GPIO_PIN_SET); // Data out (initiator to target)
    //         while (!RATN());

    //         TMSG(GPIO_PIN_RESET);
    //         TBSY(GPIO_PIN_RESET);
    //       }

    //       TCD(GPIO_PIN_SET);

    //       uint8_t cmd[12];

    //       cmd[0] = readHandshake();
    //       cmd[1] = readHandshake();
    //       cmd[2] = readHandshake();
    //       cmd[3] = readHandshake();
    //       cmd[4] = readHandshake();
    //       cmd[5] = readHandshake();

    //       static const int cmd_class_len[8] = {6, 10, 10, 6, 6, 12, 6, 6};
    //       int len = cmd_class_len[cmd[0] >> 5u];

    //       for (int i = 6; i < len; i++) {
    //         cmd[i] = readHandshake();
    //       }

    //       sprintf(buffer, "CMD : 0x%02X\r\n", cmd[0]);
    //       LOG(buffer);

    // // Bug après cette ligne
    // // sur de sur
    //       switch (cmd[0]) {
    //         case 0x12:
    //           LOG("[Inquiry]\r\n");
    //           status = onInquiryCommand(cmd);
    //           break;
    //         case 0x09: // "Set Filter"
    //           LOG("[Set Filter]\r\n");
    //           onSetFilter(cmd);
    //           break;
    //         case 0x0C: // "Medium Sense"
    //           LOG("[Medium Sense]\r\n");
    //           break;
    //         case 0x05: // "Send Packet"
    //           LOG("[Send Packet]\r\n");
    //           onSendPacket(cmd);
    //           break;
    //         default:
    //           LOG("UNDEFINED CMD\r\n");
    //       }
    // //      while (1);
    // //      HAL_Delay(500);
    //       // Status
    //       TIO(GPIO_PIN_SET);
    //       TCD(GPIO_PIN_SET);
    //       LOG("status (%x) ...", status);
    //       writeHandshake(status);
    //       LOG(" ok\r\n");
    //       if (!RATN())
    //         LOG("ATN !!\r\n");

    //       // bug avant cette ligne
    //       // Message In
    //       TMSG(GPIO_PIN_SET);
    //       LOG("message in (%x) ...", 0);
    //       writeHandshake(0);
    //       LOG(" ok\r\n");
    //       if (!RATN())
    //         LOG("ATN !!\r\n");

    //       TCD(GPIO_PIN_RESET);
    //       TIO(GPIO_PIN_RESET);
    //       TMSG(GPIO_PIN_RESET);
    //       TBSY(GPIO_PIN_RESET);

    //       LOG("BUS FREE\r\n");
    // }
  }
#pragma clang diagnostic pop
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the Systick interrupt time
   */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, STATUS_LED_Pin | TDBP_Pin | TSEL_Pin | TBSY_Pin | TCD_Pin | TMSG_Pin | TREQ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TDB0_Pin | TDB1_Pin | TDB2_Pin | GPIO_PIN_12 | TDB3_Pin | TDB4_Pin | TDB5_Pin | TDB6_Pin | TDB7_Pin | SPI_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TIO_GPIO_Port, TIO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : STATUS_LED_Pin TDBP_Pin TSEL_Pin TBSY_Pin
                           TCD_Pin TMSG_Pin TREQ_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_Pin | TDBP_Pin | TSEL_Pin | TBSY_Pin | TCD_Pin | TMSG_Pin | TREQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : RRST_Pin */
  GPIO_InitStruct.Pin = RRST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RRST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RBSY_Pin */
  GPIO_InitStruct.Pin = RBSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RBSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RDB0_Pin RDB1_Pin RDB2_Pin RDB3_Pin
                           RDB4_Pin RDB5_Pin RDB6_Pin RDB7_Pin
                           PA9 RACK_Pin RATN_Pin */
  GPIO_InitStruct.Pin = RDB0_Pin | RDB1_Pin | RDB2_Pin | RDB3_Pin | RDB4_Pin | RDB5_Pin | RDB6_Pin | RDB7_Pin | GPIO_PIN_9 | RACK_Pin | RATN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TDB0_Pin TDB1_Pin TDB2_Pin PB12
                           TDB3_Pin TDB4_Pin TDB5_Pin TDB6_Pin
                           TDB7_Pin SPI_RST_Pin */
  GPIO_InitStruct.Pin = TDB0_Pin | TDB1_Pin | TDB2_Pin | GPIO_PIN_12 | TDB3_Pin | TDB4_Pin | TDB5_Pin | TDB6_Pin | TDB7_Pin | SPI_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RDBP_Pin */
  GPIO_InitStruct.Pin = RDBP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RDBP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : RSEL_Pin */
  GPIO_InitStruct.Pin = RSEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RSEL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TIO_Pin */
  GPIO_InitStruct.Pin = TIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TIO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_INT_Pin */
  GPIO_InitStruct.Pin = SPI_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
