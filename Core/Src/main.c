/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "cmsis_os.h"
#include "fatfs.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mongoose.h"
#include "hal.h"
#include "net.h"
#include "db.h"
#include "multi_button.h"
#include "cJSON.h"
#include "setings.h"
#include "lwdtc.h" //

#include "ds18b20.h"
#include "ds18b20Config.h"

#define BLINK_PERIOD_MS 1000  // LED blinking period in millis
#define DEBOUNCE_DELAY 45 //Encoder (ms)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint16_t usbnum = 0;
// CRON variable
struct tm *timez;
time_t cronetime;
time_t cronetime_old;

// SNTP variable
extern uint64_t s_boot_timestamp;

TIM_HandleTypeDef htim[NUMPIN];
data_pin_t data_pin;

struct Button button[NUMPIN];

extern ApplicationTypeDef Appli_state;
extern struct dbSettings SetSettings;
extern struct dbCron dbCrontxt[MAXSIZE];
extern struct dbPinsConf PinsConf[NUMPIN];
extern struct dbPinsInfo PinsInfo[NUMPIN];
extern struct dbPinToPin PinsLinks[NUMPINLINKS];

extern Ds18b20Sensor_t ds18b20[_DS18B20_MAX_SENSORS];
extern OneWire_t    OneWire;
extern uint8_t	    OneWireDevices;
extern uint8_t 	    TempSensorCount;
extern uint8_t		Ds18b20StartConvert;
extern uint16_t	    Ds18b20Timeout;
extern uint64_t s_boot_timestamp;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma location=0x2007c000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x2007c0a0
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x2007c000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x2007c0a0))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
#endif

ETH_TxPacketConfig TxConfig;

ETH_HandleTypeDef heth;

RNG_HandleTypeDef hrng;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart3;

/* Definitions for ConfigTask */
osThreadId_t ConfigTaskHandle;
const osThreadAttr_t ConfigTask_attributes = {
  .name = "ConfigTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for WebServerTask */
osThreadId_t WebServerTaskHandle;
const osThreadAttr_t WebServerTask_attributes = {
  .name = "WebServerTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OutputTask */
osThreadId_t OutputTaskHandle;
const osThreadAttr_t OutputTask_attributes = {
  .name = "OutputTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CronTask */
osThreadId_t CronTaskHandle;
const osThreadAttr_t CronTask_attributes = {
  .name = "CronTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for InputTask */
osThreadId_t InputTaskHandle;
const osThreadAttr_t InputTask_attributes = {
  .name = "InputTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OnewireTask */
osThreadId_t OnewireTaskHandle;
const osThreadAttr_t OnewireTask_attributes = {
  .name = "OnewireTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for EncoderTask */
osThreadId_t EncoderTaskHandle;
const osThreadAttr_t EncoderTask_attributes = {
  .name = "EncoderTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myQueue */
osMessageQueueId_t myQueueHandle;
const osMessageQueueAttr_t myQueue_attributes = {
  .name = "myQueue"
};
/* Definitions for usbQueue */
osMessageQueueId_t usbQueueHandle;
const osMessageQueueAttr_t usbQueue_attributes = {
  .name = "usbQueue"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RNG_Init(void);
static void MX_TIM1_Init(void);
void StartConfigTask(void *argument);
void StartWebServerTask(void *argument);
void StartOutputTask(void *argument);
void StartCronTask(void *argument);
void StartInputTask(void *argument);
void StartOnewireTask(void *argument);
void StartEncoderTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*********************** Mongoose ******************************/
void mg_random(void *buf, size_t len) {  // Use on-board RNG
	extern RNG_HandleTypeDef hrng;
	for (size_t n = 0; n < len; n += sizeof(uint32_t)) {
		uint32_t r;
		HAL_RNG_GenerateRandomNumber(&hrng, &r);
		memcpy((char*) buf + n, &r, n + sizeof(r) > len ? len - n : sizeof(r));
	}
}

static void timer_fn(void *arg) {
	struct mg_tcpip_if *ifp = arg;                  // And show
	const char *names[] = { "down", "up", "req", "ready" };  // network stats
	MG_INFO(("Ethernet: %s, IP: %M, rx:%u, tx:%u, dr:%u, er:%u", names[ifp->state], mg_print_ip4, &ifp->ip, ifp->nrecv, ifp->nsent, ifp->ndrop, ifp->nerr));
}
uint64_t mg_millis(void) {
	return HAL_GetTick();
}

//static void send_mqtt_message(struct mg_connection *conn, const char *msg) {
//	struct mg_mqtt_opts pub_opts;
//	memset(&pub_opts, 0, sizeof(pub_opts));
//	pub_opts.topic = mg_str(s_pub_topic);
//	pub_opts.message = mg_str(msg);
//	pub_opts.qos = s_qos;
//	pub_opts.retain = false;
//	mg_mqtt_pub(conn, &pub_opts);
//	MG_INFO(("%lu PUBLISHED %s -> %.*s", conn->id, msg, (int) pub_opts.topic.len, pub_opts.topic.buf));
//}
/*********************** End Mongoose ******************************/

// Функция обратного вызова для обработки событий кнопки
 void button_event_handler(Button* handle)
 {
     // Обработчик событий кнопки
     PressEvent event = get_button_event(handle);

     switch (event) {
         case NONE_PRESS:
             // Нет нажатия
             break;
         case PRESS_DOWN:
             // Кнопка нажата
             MG_INFO(("Button %d: PRESS_DOWN!\r\n", handle->button_id));
             break;
         case PRESS_UP:
             // Кнопка отпущена
             MG_INFO(("Button %d: PRESS_UP!\r\n", handle->button_id));
             break;
         case LONG_PRESS_START:
             // Начало долгого нажатия
             MG_INFO(("Button %d: LONG_PRESS_START!\r\n", handle->button_id));
             break;
         case LONG_PRESS_HOLD:
             // Продолжение долгого нажатия
             MG_INFO(("Button %d: LONG_PRESS_HOLD!\r\n", handle->button_id));
             break;
         case SINGLE_CLICK:
             // Одиночное нажатие кнопки
				for (uint8_t a = 0; a < NUMPINLINKS; a++) {
					if (PinsLinks[a].idin == handle->button_id) {
						data_pin.pin = PinsLinks[a].idout;
						data_pin.action = 2;
						xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
					}
				}
             MG_INFO(("Button %d: SINGLE_CLICK!\r\n", handle->button_id));
             break;
         case DOUBLE_CLICK:
             // Двойное нажатие кнопки
             MG_INFO(("Button %d: DOUBLE_CLICK!\r\n", handle->button_id));
             break;
         case PRESS_REPEAT:
             // Повторное нажатие кнопки
             MG_INFO(("Button %d: PRESS_REPEAT!\r\n", handle->button_id));
             break;
         default:
             // Обработка неизвестного значения event
             break;
     }
 }

 void pwm_event_handler(Button* handle)
  {
      // Обработчик событий кнопки
      PressEvent event = get_button_event(handle);

      int i = 0;

      switch (event) {
          case NONE_PRESS:
              // Нет нажатия
              break;
          case PRESS_DOWN:
              // Кнопка нажата
              MG_INFO(("Button %d: PRESS_DOWN!\r\n", handle->button_id));
              break;
          case PRESS_UP:
              // Кнопка отпущена
              MG_INFO(("Button %d: PRESS_UP!\r\n", handle->button_id));
              break;
          case LONG_PRESS_START:
              // Начало долгого нажатия
              MG_INFO(("Button %d: LONG_PRESS_START!\r\n", handle->button_id));
              break;
          case LONG_PRESS_HOLD:
        	  if(PinsConf[handle->button_id].sclick == 2){
				for (uint8_t a = 0; a < NUMPINLINKS; a++) {
					if (PinsLinks[a].idin == handle->button_id) {
						//PinsInfo[i].tim->CCR1 = 50;

								// PWM
								i = PinsLinks[a].idout;
								if (PinsConf[i].topin == 5 ){
									  //for (int d = 0; d <= 11; ++d) {
									PinsConf[i].dvalue  = (int) HAL_TIM_ReadCapturedValue(&htim[i], PinsInfo[i].tim_channel);

									if(PinsConf[handle->button_id].on == 1) {
										PinsConf[i].dvalue += 1;
										if(PinsConf[i].dvalue > 100){
											PinsConf[i].dvalue = 100;
											//pwmflag[handle->button_id] = 0;
										}
									}
									if(PinsConf[handle->button_id].on == 0) {
										PinsConf[i].dvalue -= 1;
										if(PinsConf[i].dvalue < 0){
											PinsConf[i].dvalue = 0;
											//pwmflag[handle->button_id] = 1;
										}
									}

									__HAL_TIM_SET_COMPARE(&htim[i], PinsInfo[i].tim_channel, PinsConf[i].dvalue);
									MG_INFO(("PWM pwmValue %d %s \r\n", PinsConf[i].dvalue, PinsInfo[i].pins));
								}


// 						data_pin.pin = PinsLinks[a].idout;
// 						data_pin.action = 2;
// 						xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
								MG_INFO(("Button %d: SINGLE_CLICK PWM pwmValue %d flag %d!\r\n", handle->button_id, PinsConf[i].dvalue, PinsConf[handle->button_id].on));
					}
				}
        	  }
              MG_INFO(("Button %d: LONG_PRESS_HOLD!\r\n", handle->button_id));
              break;
          case SINGLE_CLICK:
              // Одиночное нажатие кнопки
        	  if(PinsConf[handle->button_id].sclick == 2){
 				for (uint8_t a = 0; a < NUMPINLINKS; a++) {
 					if (PinsLinks[a].idin == handle->button_id) {

 						//PinsInfo[i].tim->CCR1 = 50;

 							//for (uint8_t i = 0; i < NUMPIN; i++) {

 								// PWM
 								i = PinsLinks[a].idout;
 								if (PinsConf[i].topin == 5){
 									  //for (int d = 0; d <= 11; ++d) {
 									PinsConf[i].dvalue  = (int) HAL_TIM_ReadCapturedValue(&htim[i], PinsInfo[i].tim_channel);
 									//MG_INFO(("PWM pwmValue %d \r\n", PinsConf[i].dvalue));
 									if(PinsConf[handle->button_id].on == 1) {
 										PinsConf[i].dvalue += 1;
										if(PinsConf[i].dvalue > 100){
											PinsConf[i].dvalue = 100;
											PinsConf[handle->button_id].on = 0;
										}
 									}
 									if(PinsConf[handle->button_id].on == 0) {
 										PinsConf[i].dvalue -= 1;
										if(PinsConf[i].dvalue < 0){
											PinsConf[i].dvalue = 0;
											PinsConf[handle->button_id].on = 1;
										}
 									}

									__HAL_TIM_SET_COMPARE(&htim[i], PinsInfo[i].tim_channel, PinsConf[i].dvalue);
									MG_INFO(("PWM pwmValue %d %s \r\n", PinsConf[i].dvalue, PinsInfo[i].pins));
 								//}
 							}

// 						data_pin.pin = PinsLinks[a].idout;
// 						data_pin.action = 2;
// 						xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
 						MG_INFO(("Button %d: SINGLE_CLICK PWM pwmValue %d flag %d!\r\n", handle->button_id, PinsConf[i].dvalue, PinsConf[handle->button_id].on));
 					}
 				}
        	  }
              //MG_INFO(("Button %d: SINGLE_CLICK PWM!\r\n", handle->button_id));
              break;
          case DOUBLE_CLICK:
              // Двойное нажатие кнопки

        	  PinsConf[handle->button_id].on ^= 1;

              MG_INFO(("Button %d: DOUBLE_CLICK PWM %d!\r\n", handle->button_id, PinsConf[handle->button_id].on));
              break;
          case PRESS_REPEAT:
              // Повторное нажатие кнопки
              MG_INFO(("Button %d: PRESS_REPEAT PWM!\r\n", handle->button_id));
              break;
          default:
              // Обработка неизвестного значения event
              break;
      }
  }

 // Функция для получения состояния GPIO кнопки
  uint8_t read_button_level(uint8_t button_id)
  {
      // Вернуть состояние GPIO пина, к которому подключена кнопка
 	 return  HAL_GPIO_ReadPin(PinsInfo[button_id].gpio_name, PinsInfo[button_id].hal_pin);

      //return GPIO_PIN_RESET; // Значение по умолчанию, если кнопка не найдена
  }
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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_RNG_Init();
  MX_TIM1_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
  test_init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue */
  myQueueHandle = osMessageQueueNew (16, sizeof(struct data_pin_t), &myQueue_attributes);

  /* creation of usbQueue */
  usbQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &usbQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ConfigTask */
  ConfigTaskHandle = osThreadNew(StartConfigTask, NULL, &ConfigTask_attributes);

  /* creation of WebServerTask */
  WebServerTaskHandle = osThreadNew(StartWebServerTask, NULL, &WebServerTask_attributes);

  /* creation of OutputTask */
  OutputTaskHandle = osThreadNew(StartOutputTask, NULL, &OutputTask_attributes);

  /* creation of CronTask */
  CronTaskHandle = osThreadNew(StartCronTask, NULL, &CronTask_attributes);

  /* creation of InputTask */
  InputTaskHandle = osThreadNew(StartInputTask, NULL, &InputTask_attributes);

  /* creation of OnewireTask */
  OnewireTaskHandle = osThreadNew(StartOnewireTask, NULL, &OnewireTask_attributes);

  /* creation of EncoderTask */
  EncoderTaskHandle = osThreadNew(StartEncoderTask, NULL, &EncoderTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 216-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SENSOR_GPIO_Port, SENSOR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SENSOR_Pin */
  GPIO_InitStruct.Pin = SENSOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SENSOR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// int pause  0 - до паузы 1 - после паузы
void parse_string(char *str, time_t cronetime_olds, int i, int pause) {
	char *token;
	char *saveptr;
	int flag = 0;
	int k = 0;
	int pin = 0;
	char delim[] = ";";

	// Разбиваем строку на элементы, разделенные точкой с запятой
	token = strtok_r(str, delim, &saveptr);
	while (token != NULL) {
		char *end_token;
		// Если нашли элемент "p", устанавливаем флаг

		if (token[0] == 'p') {
			char *newstring = token + 1;
			//MG_INFO(("Pause %d seconds\n", atoi(newstring)));
			dbCrontxt[i].ptime = cronetime_olds + atoi(newstring);
			flag = 1;
		}
		// в зависимости от флага отправляем в очередь до или после паузы
		if (flag == pause) {
			//MG_INFO(("%s\n", token));

			//strcpy(data_pin.message, pch);

			char *token2 = strtok_r(token, ":", &end_token);
			//MG_INFO(("pin = %d\n", atoi(token2)));

			while (token2 != NULL) {
				// тут отправляем в очередь
				if (k == 0) {
					pin = atoi(token2);
					if(pin != 0){
						data_pin.pin = pin-1;
					}
					//MG_INFO(("pin = %s\n", token2));
				}
				if (k == 1) {
					data_pin.action = atoi(token2);
					//MG_INFO(("action = %s\n", token2));
				}

				token2 = strtok_r(NULL, ":", &end_token);
				k++;
				// MG_INFO(("action = %d\n", atoi(token2));
			}

			if(k == 2) {
				xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
			}
			k = 0;
		}
		token = strtok_r(NULL, delim, &saveptr);
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartConfigTask */
/**
  * @brief  Function implementing the ConfigTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartConfigTask */
void StartConfigTask(void *argument)
{
  /* init code for USB_HOST */
  MX_USB_HOST_Init();
  /* USER CODE BEGIN 5 */
	int usbflag = 1;
	FILINFO finfo;
	/* Infinite loop */
	for (;;) {
		switch (Appli_state) {
		case APPLICATION_READY:
			if (usbflag == 1) {
				osDelay(1000);
				MG_INFO(("APPLICATION_READY! \r\n"));

				FRESULT fresult = f_stat("setings.ini", &finfo);
				if (fresult == FR_OK) {
					GetSetingsConfig();
					GetCronConfig();
					GetPinConfig();
					GetPinToPin();
					InitPin();

					xTaskNotifyGive(WebServerTaskHandle); // ТО ВКЛЮЧАЕМ ЗАДАЧУ WebServerTask
					xTaskNotifyGive(CronTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ CronTask
					xTaskNotifyGive(OutputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ OutputTask
					xTaskNotifyGive(InputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ InputTask
					xTaskNotifyGive(EncoderTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ PWMTask
					osDelay(100);
					xTaskNotifyGive(OnewireTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ OneWire

				} else {
					StartSetingsConfig();

					xTaskNotifyGive(WebServerTaskHandle); // ТО ВКЛЮЧАЕМ ЗАДАЧУ WebServerTask
					xTaskNotifyGive(CronTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ CronTask
					xTaskNotifyGive(OutputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ OutputTask
					xTaskNotifyGive(InputTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ InputTask
					xTaskNotifyGive(EncoderTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ PWMTask
					osDelay(100);
					xTaskNotifyGive(OnewireTaskHandle); // И ВКЛЮЧАЕМ ЗАДАЧУ OneWire

				}
				usbflag = 0;
			}
			// Функция для чтения целых чисел из очереди
			if (xQueueReceive(usbQueueHandle, &usbnum, portMAX_DELAY) == pdTRUE) {
				switch (usbnum) {
				case 1:
					SetPinConfig();
					break;
				case 2:
					SetSetingsConfig();
					break;
				case 3:
					SetCronConfig();
					break;
				case 4:
					SetPinToPin();
					break;
				default:
					//MG_INFO(("Wrong data! \r\n"));
					break;
				}
				MG_INFO(("xQueueReceive number: %u\n", usbnum));
			}
			break;
		default:
			//MG_INFO(("Wrong data! \r\n");
			break;
		}
		osDelay(1);
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartWebServerTask */
/**
* @brief Function implementing the WebServerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartWebServerTask */
void StartWebServerTask(void *argument)
{
  /* USER CODE BEGIN StartWebServerTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
	/* Infinite loop */
	volatile unsigned long prevtime;
	prevtime = HAL_GetTick();

	struct mg_mgr mgr;        // Initialise Mongoose event manager
	mg_mgr_init(&mgr);        // and attach it to the interface
	mg_log_set(MG_LL_DEBUG);  // Set log level

	// Initialise Mongoose network stack
	struct mg_tcpip_driver_stm32f_data driver_data = { .mdc_cr = 4 };
	struct mg_tcpip_if mif = { .mac = GENERATE_LOCALLY_ADMINISTERED_MAC(),
			// Uncomment below for static configuration:
			//.ip = mg_htonl(MG_U32(192, 168, 11, 80)),
			//.mask = mg_htonl(MG_U32(255, 255, 255, 0)),
			//.gw = mg_htonl(MG_U32(192, 168, 11, 1)),
			.driver = &mg_tcpip_driver_stm32f, .driver_data = &driver_data };
	mg_tcpip_init(&mgr, &mif);
	//mg_timer_add(&mgr, 1000, MG_TIMER_REPEAT, timer_fn, &mif);
	MG_INFO(("MAC: %M. Waiting for IP...", mg_print_mac, mif.mac));
	while (mif.state != MG_TCPIP_STATE_READY) {
		mg_mgr_poll(&mgr, 0);
	}

	MG_INFO(("Initialising application.."));
	web_init(&mgr);

	MG_INFO(("Starting event loop"));
	for (;;) {
		mg_mgr_poll(&mgr, 1);  // Infinite event loop
	}
	(void) argument;
  /* USER CODE END StartWebServerTask */
}

/* USER CODE BEGIN Header_StartOutputTask */
/**
* @brief Function implementing the OutputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOutputTask */
void StartOutputTask(void *argument)
{
  /* USER CODE BEGIN StartOutputTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
	/* Infinite loop */
	for (;;) {
		if (xQueueReceive(myQueueHandle, &data_pin, portMAX_DELAY) == pdTRUE) {
			if (data_pin.action == 0) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_WritePin(PinsInfo[data_pin.pin].gpio_name, PinsInfo[data_pin.pin].hal_pin, GPIO_PIN_RESET);
				//MG_INFO(("%d-%d  \r\n", (int) data_pin.pin, (int) data_pin.action));
			}
			if (data_pin.action == 1) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_WritePin(PinsInfo[data_pin.pin].gpio_name, PinsInfo[data_pin.pin].hal_pin, GPIO_PIN_SET);
				//MG_INFO(("%d-%d  \r\n", (int) data_pin.pin, (int) data_pin.action));
			}
			if (data_pin.action == 2) {
				//@todo  проверить что data_pin.pin число
				HAL_GPIO_TogglePin(PinsInfo[data_pin.pin].gpio_name, PinsInfo[data_pin.pin].hal_pin);
				//MG_INFO(("%d-%d  \r\n", (int) data_pin.pin, (int) data_pin.action);
			}
		}
		osDelay(1);
	}
  /* USER CODE END StartOutputTask */
}

/* USER CODE BEGIN Header_StartCronTask */
/**
* @brief Function implementing the CronTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCronTask */
void StartCronTask(void *argument)
{
  /* USER CODE BEGIN StartCronTask */
	ulTaskNotifyTake(0, portMAX_DELAY);

		static lwdtc_cron_ctx_t cron_ctxs[MAXSIZE];
		int i = 0;
		char str[40] = { 0 };

		/* Define context for CRON, used to parse data to */
		size_t fail_index;
		MG_INFO(("Count task %d\r\n", LWDTC_ARRAYSIZE(dbCrontxt)));
		/* Parse all cron strings */
		if (lwdtc_cron_parse_multi(cron_ctxs, dbCrontxt, MAXSIZE, &fail_index) != lwdtcOK) {
			MG_INFO(("Failed to parse cron at index %d\r\n", (int) fail_index));
		}
		MG_INFO(("CRONs parsed and ready to go\r\n"));

		/* Infinite loop */
		for (;;) {
			if (s_boot_timestamp != 0) {
				//MG_INFO(("TIME: %lld ms ", s_boot_timestamp));
				cronetime = (time_t) (s_boot_timestamp / 1000) + (mg_millis() / 1000);
				if (cronetime != cronetime_old) {
					cronetime_old = cronetime;
					timez = localtime(&cronetime);
					i = 0;

					while (i < LWDTC_ARRAYSIZE(dbCrontxt)) {
						if (cronetime >= dbCrontxt[i].ptime && dbCrontxt[i].ptime != 0) {

							strcpy(str, dbCrontxt[i].activ);
							parse_string(str, cronetime_old, i, 1);
							dbCrontxt[i].ptime = 0;
						}
						i++;
					}
					i = 0;
					/* Check if CRON should execute */
					while (i < LWDTC_ARRAYSIZE(cron_ctxs)) {
						if (lwdtc_cron_is_valid_for_time(timez, cron_ctxs, &i) == lwdtcOK) {

							strcpy(str, dbCrontxt[i].activ);
							parse_string(str, cronetime_old, i, 0);
							//xQueueSend(myQueueHandle, &i, 0);
						}
						i++;
					}
				}
				//MG_INFO(("TIME s: %lld s ", cronetime));
			}
			osDelay(1);
		}
  /* USER CODE END StartCronTask */
}

/* USER CODE BEGIN Header_StartInputTask */
/**
* @brief Function implementing the InputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInputTask */
void StartInputTask(void *argument)
{
  /* USER CODE BEGIN StartInputTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
//	MG_INFO(("Start 'Input' task \r\n");
	uint8_t pinStates[NUMPIN] = { 0 };
	uint32_t pinTimes[NUMPIN] = { 0 };
	uint32_t millis;
	uint8_t pinLevel[NUMPIN] = { 0 };

	osDelay(1000);
	InitMultibutton();
	/* Infinite loop */
	for (;;) {
		millis = HAL_GetTick();
		for (uint8_t i = 0; i < NUMPIN; i++) {
			// INPUT Button
			if (PinsConf[i].topin == 1 && PinsConf[i].act == 1) {
				if ((millis - pinTimes[i]) >= 5) {
					pinTimes[i] = millis;
					button_ticks(&button[i]);
				}
			}
			// INPUT Switch
			if (PinsConf[i].topin == 3) { // Для 'switch'
				pinStates[i] = HAL_GPIO_ReadPin(PinsInfo[i].gpio_name, PinsInfo[i].hal_pin);
				if (pinStates[i] == 1 && (millis - pinTimes[i]) >= 200 && pinLevel[i] != pinStates[i]) {
					pinLevel[i] = pinStates[i];
					pinTimes[i] = millis;

					// OUTPUT (вынести в отдельную функцию)
					for (uint8_t a = 0; a < NUMPINLINKS; a++) {
						if (PinsLinks[a].idin == i) {
							data_pin.pin = PinsLinks[a].idout;
							data_pin.action = 1;
							xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
						}
					}
				}
				if (pinStates[i] == 0 && (millis - pinTimes[i]) >= 200 && pinLevel[i] != pinStates[i]) {
					pinLevel[i] = pinStates[i];
					pinTimes[i] = millis;

					// OUTPUT (вынести в отдельную функцию)
					for (uint8_t a = 0; a < NUMPINLINKS; a++) {
						if (PinsLinks[a].idin == i) {
							data_pin.pin = PinsLinks[a].idout;
							data_pin.action = 0;
							xQueueSend(myQueueHandle, (void* ) &data_pin, 0);
						}
					}
				}
			}
		}
		//osDelay(5);
	}
  /* USER CODE END StartInputTask */
}

/* USER CODE BEGIN Header_StartOnewireTask */
/**
* @brief Function implementing the OnewireTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOnewireTask */
void StartOnewireTask(void *argument)
{
  /* USER CODE BEGIN StartOnewireTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
		//	MG_INFO(("Start 'OneWire' task \r\n"));
		uint8_t Ds18b20TryToFind = 5;
		do {
			OneWire_Init(&OneWire, _DS18B20_GPIO, _DS18B20_PIN);
			TempSensorCount = 0;
			while (HAL_GetTick() < 3000)
				Ds18b20Delay(100);
			OneWireDevices = OneWire_First(&OneWire);
			while (OneWireDevices) {
				Ds18b20Delay(100);
				TempSensorCount++;
				OneWire_GetFullROM(&OneWire, ds18b20[TempSensorCount - 1].Address);
				OneWireDevices = OneWire_Next(&OneWire);
			}
			MG_INFO(("TempSensorCount = %d\r\n", TempSensorCount));

			if (TempSensorCount > 0)
				break;
			Ds18b20TryToFind--;
		} while (Ds18b20TryToFind > 0);
		//	if(Ds18b20TryToFind==0)
		//		vTaskDelete(Ds18b20Handle);
		for (uint8_t i = 0; i < TempSensorCount; i++) {
			Ds18b20Delay(50);
			DS18B20_SetResolution(&OneWire, ds18b20[i].Address, DS18B20_Resolution_12bits);
			Ds18b20Delay(50);
			DS18B20_DisableAlarmTemperature(&OneWire, ds18b20[i].Address);
		}
		/* Infinite loop */
		for (;;) {
			while (_DS18B20_UPDATE_INTERVAL_MS == 0) {
				if (Ds18b20StartConvert == 1)
					break;
				Ds18b20Delay(10);
			}
			Ds18b20Timeout = _DS18B20_CONVERT_TIMEOUT_MS / 10;
			DS18B20_StartAll(&OneWire);
			osDelay(100);
			while (!DS18B20_AllDone(&OneWire)) {
				osDelay(10);
				Ds18b20Timeout -= 1;
				if (Ds18b20Timeout == 0)
					break;
			}
			if (Ds18b20Timeout > 0) {
				for (uint8_t i = 0; i < TempSensorCount; i++) {
					osDelay(1000);	//300
					ds18b20[i].DataIsValid = DS18B20_Read(&OneWire, ds18b20[i].Address, &ds18b20[i].Temperature);
				}
			} else {
				for (uint8_t i = 0; i < TempSensorCount; i++)
					ds18b20[i].DataIsValid = false;
			}
			Ds18b20StartConvert = 0;
			osDelay(500);	//_DS18B20_UPDATE_INTERVAL_MS
		}
  /* USER CODE END StartOnewireTask */
}

/* USER CODE BEGIN Header_StartEncoderTask */
/**
* @brief Function implementing the EncoderTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartEncoderTask */
void StartEncoderTask(void *argument)
{
  /* USER CODE BEGIN StartEncoderTask */
	ulTaskNotifyTake(0, portMAX_DELAY);
//	MG_INFO(("Start 'Encoder' task \r\n"));
	uint8_t pinb = 0;
	uint32_t millis;
	uint32_t pinTimes[NUMPIN] = { 0 };
	uint8_t prev_A[NUMPIN] = { 0, };
	uint8_t prev_B[NUMPIN] = { 0, };
	uint8_t i = 0;
	uint8_t a = 0;
	uint8_t pwm = 0;
	/* Infinite loop */
	for (;;) {
		millis = HAL_GetTick();
		for (i = 0; i < NUMPIN; i++) {
			if (PinsConf[i].topin == 8) {   // EncodrerA
				pinb = PinsConf[i].encoderb;   // EncodrerB
				if (pinb != 0) {
					pinb = pinb - 1;
					if (millis - pinTimes[i] >= DEBOUNCE_DELAY) { // игнорируем дребезг
						PinsConf[i].on = HAL_GPIO_ReadPin(PinsInfo[i].gpio_name, PinsInfo[i].hal_pin);
						osDelay(3);
						PinsConf[pinb].on = HAL_GPIO_ReadPin(PinsInfo[pinb].gpio_name, PinsInfo[pinb].hal_pin);
						if (PinsConf[i].on != prev_A[i] || PinsConf[pinb].on != prev_B[pinb]) { //Если состояние изменилось
							pinTimes[i] = millis; // Сбрасываем дребезг
							if (PinsConf[i].on == 1 && PinsConf[pinb].on == 0) { // A && B
								MG_INFO(("ID:%d  A = %d & B = %d\r\n",i, PinsConf[i].on, PinsConf[pinb].on));
								for (a = 0; a < NUMPINLINKS; a++) {
									if (PinsLinks[a].idin == i) { // A
										pwm = PinsLinks[a].idout; // B
										if (PinsConf[pwm].topin == 5) { // PWM
											PinsConf[pwm].dvalue = (int) HAL_TIM_ReadCapturedValue(&htim[pwm], PinsInfo[pwm].tim_channel);
											PinsConf[pwm].dvalue -= 1;
											if (PinsConf[pwm].dvalue <= 0) {
												PinsConf[pwm].dvalue = 0;
											}
											__HAL_TIM_SET_COMPARE(&htim[pwm], PinsInfo[pwm].tim_channel, PinsConf[pwm].dvalue);
											MG_INFO(("PWM = %d\r\n", PinsConf[pwm].dvalue));
										}
									}
								}
							} else if (PinsConf[i].on == 0 && PinsConf[pinb].on == 1) { // A && B
								MG_INFO(("ID:%d  A = %d & B = %d\r\n",i, PinsConf[i].on, PinsConf[pinb].on));
								for (a = 0; a < NUMPINLINKS; a++) {
									if (PinsLinks[a].idin == i) { // A
										pwm = PinsLinks[a].idout; // B
										if (PinsConf[pwm].topin == 5) { // PWM
											PinsConf[pwm].dvalue = (int) HAL_TIM_ReadCapturedValue(&htim[pwm], PinsInfo[pwm].tim_channel);
											PinsConf[pwm].dvalue += 1;
											if (PinsConf[pwm].dvalue >= 100) {
												PinsConf[pwm].dvalue = 100;
											}
											__HAL_TIM_SET_COMPARE(&htim[pwm], PinsInfo[pwm].tim_channel, PinsConf[pwm].dvalue);
											MG_INFO(("PWM = %d\r\n", PinsConf[pwm].dvalue));
										}
									}
								}
							}
							prev_A[i] = PinsConf[i].on; //A
							prev_B[pinb] = PinsConf[pinb].on; //B;
						}
					}
				}
			}
		}
	}
  /* USER CODE END StartEncoderTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
