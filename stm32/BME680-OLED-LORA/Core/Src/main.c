/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "bme680.h"
#include "fonts.h"
#include "ssd1306.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
struct bme680_dev gas_sensor;
struct bme680_field_data data;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t var1 = 0;
uint32_t var2 = 0;
uint8_t set_required_settings;
int8_t rslt = 0;
uint16_t meas_period;
uint32_t ii = 0;
RTC_TimeTypeDef sTime = { 0 };
uint8_t Data[256];

// Sensor measures
float temperature;
float pressure;
float humidity;
float gas_resistance;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void user_delay_ms(uint32_t period);
int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data,
		uint16_t len);
int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data,
		uint16_t len);

void read_bme680(void);
void display_measures(void);
void send_measures(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

	// BME680 Init
	gas_sensor.dev_id = BME680_I2C_ADDR_PRIMARY;
	gas_sensor.intf = BME680_I2C_INTF;
	gas_sensor.read = user_i2c_read;
	gas_sensor.write = user_i2c_write;
	gas_sensor.delay_ms = user_delay_ms;
	gas_sensor.amb_temp = 25;

	/* Set the temperature, pressure and humidity settings */
	gas_sensor.tph_sett.os_hum = BME680_OS_16X;
	gas_sensor.tph_sett.os_pres = BME680_OS_16X;
	gas_sensor.tph_sett.os_temp = BME680_OS_16X;
	gas_sensor.tph_sett.filter = BME680_FILTER_SIZE_127;

	/* Set the remaining gas sensor settings and link the heating profile */
	gas_sensor.gas_sett.run_gas = BME680_ENABLE_GAS_MEAS;
	/* Create a ramp heat waveform in 3 steps */
	gas_sensor.gas_sett.heatr_temp = 320; /* degree Celsius */
	gas_sensor.gas_sett.heatr_dur = 150; /* milliseconds */

	/* Select the power mode */
	/* Must be set before writing the sensor configuration */
	gas_sensor.power_mode = BME680_FORCED_MODE;

	/* Set the required sensor settings needed */
	set_required_settings = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL
			| BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

	rslt = bme680_init(&gas_sensor);

	/* Set the desired sensor configuration */

	rslt = bme680_set_sensor_settings(set_required_settings, &gas_sensor);

	/* Set the power mode */

	rslt = bme680_set_sensor_mode(&gas_sensor);

	bme680_get_profile_dur(&meas_period, &gas_sensor);

	// OLED Init
	ssd1306_Init();
	ssd1306_Fill(White);
	ssd1306_UpdateScreen();
	ssd1306_SetCursor(16, 4);
	HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	while (1) {
		read_bme680();
		display_measures();
		send_measures();
		HAL_Delay(10000);

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void user_delay_ms(uint32_t period) {
	HAL_Delay(period);
}

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data,
		uint16_t len) {
	int8_t rslt = 0;
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Read(&hi2c1, dev_id, reg_addr, I2C_MEMADD_SIZE_8BIT,
			(uint8_t*) reg_data, len, 0x10000);
	if (status != HAL_OK)
		rslt = -3;
	return rslt;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data,
		uint16_t len) {
	int8_t rslt = 0;
	HAL_StatusTypeDef status = HAL_OK;
	status = HAL_I2C_Mem_Write(&hi2c1, dev_id, reg_addr, I2C_MEMADD_SIZE_8BIT,
			(uint8_t*) reg_data, len, 0x10000);
	if (status != HAL_OK)
		rslt = -3;
	return rslt;
}

void read_bme680(void) {

	user_delay_ms(meas_period);
	rslt = bme680_get_sensor_data(&data, &gas_sensor);

	/* Take measures */
	temperature = data.temperature / 100.0f;
	pressure = data.pressure / 100.0f;
	humidity = data.humidity / 100.0f;

	if (data.status & BME680_GASM_VALID_MSK) {
		gas_resistance = data.gas_resistance / 1000.0f;
	}

	/*  Trigger the next measurement if you would like to read data out continuously*/
	if (gas_sensor.power_mode == BME680_FORCED_MODE) {
		rslt = bme680_set_sensor_mode(&gas_sensor);
	}
}

void display_measures(void) {
	char SSDVis_Buff[30];

	ssd1306_SetCursor(6, 6);
	sprintf(SSDVis_Buff, "BME680 over LoRa", temperature);
	ssd1306_WriteString(SSDVis_Buff, Font_7x10, Black);

	ssd1306_SetCursor(6, 20);
	sprintf(SSDVis_Buff, "T:%4.2f C ", temperature);
	ssd1306_WriteString(SSDVis_Buff, Font_7x10, Black);

	ssd1306_SetCursor(6, 30);
	sprintf(SSDVis_Buff, "H:%4.2f rH ", humidity);
	ssd1306_WriteString(SSDVis_Buff, Font_7x10, Black);

	ssd1306_SetCursor(6, 40);
	sprintf(SSDVis_Buff, "P:%4.2f hPa ", pressure);
	ssd1306_WriteString(SSDVis_Buff, Font_7x10, Black);

	ssd1306_SetCursor(6, 50);
	sprintf(SSDVis_Buff, "G:%4.2f Kohms ", gas_resistance);
	ssd1306_WriteString(SSDVis_Buff, Font_7x10, Black);
	ssd1306_UpdateScreen();
	ssd1306_Fill(White);
}

void send_measures(void){
	uint8_t measures[30];
	sprintf(measures, "%2.2f %4.2f %4.2f %4.2f", temperature, humidity, pressure, gas_resistance);
	HAL_UART_Transmit(&huart1, measures, sizeof(measures), 10); // Sending in normal mode
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
