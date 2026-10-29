/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_REVERSE
} MotorState;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOTOR_STP 400
#define  MOTOR_MAX 4180
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static MotorState g_motor_state=MOTOR_STOP;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */

void Motor_Init(void);
void Motor_Start(void);
void Motor_Stop(void);
void Motor_SetDir(MotorState state);
void Motor_SetSpeed(uint16_t speed);
void Motor_Control(MotorState state, uint16_t speed);
uint8_t Key_Pressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Motor_Init(void)
{
	 Motor_Stop();
	 Motor_SetSpeed(0);
	 g_motor_state = MOTOR_STOP;
}
void Motor_Start(void)
{
	HAL_GPIO_WritePin(PWM1_SD_GPIO_Port,PWM1_SD_Pin,GPIO_PIN_SET);
}
void Motor_Stop(void)
{
	HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1,TIM_CHANNEL_1);
    HAL_GPIO_WritePin(PWM1_SD_GPIO_Port,PWM1_SD_Pin, GPIO_PIN_RESET);
    g_motor_state=MOTOR_STOP;
}
void Motor_SetDir(MotorState state)
{  
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	
		if(state==MOTOR_FORWARD)
		{
			HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);/*正转*/
		}
		else if(state==MOTOR_REVERSE)
		{
			HAL_TIMEx_PWMN_Start(&htim1,TIM_CHANNEL_1);/*反转*/
		}
}
void Motor_SetSpeed(uint16_t speed)
{
	uint32_t max_speed;
  max_speed = __HAL_TIM_GET_AUTORELOAD(&htim1) - 0x0F;
	if(speed>max_speed)
	{
		speed=max_speed;
	}
	 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,speed);
}
void Motor_Control(MotorState state, uint16_t speed)
{
	
    if ((state == MOTOR_STOP) || (speed == 0U))
    {
        Motor_Stop();
        return;
    }

    /* 方向不变时，只更新速度，不重复启动 PWM */
    if (state == g_motor_state)
    {
        Motor_SetSpeed(speed);
        return;
    }

    /* 正反转直接切换时，先停止并延时 */
    if (g_motor_state != MOTOR_STOP)
    {
        Motor_Stop();
        HAL_Delay(50);
    }

    Motor_SetSpeed(speed);
    Motor_SetDir(state);
    Motor_Start();

    g_motor_state = state;

}
uint8_t Key_Pressed(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_RESET)
	{
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOx,GPIO_Pin)==GPIO_PIN_RESET)
		{
			return 1;
		}
	}
	return 0;
}
void Motor_pwm_set(int16_t motor_pwm)
{
	if(motor_pwm>0)
	{
	Motor_Control(MOTOR_FORWARD,motor_pwm);
	}
	else if(motor_pwm<0)
	{
	Motor_Control(MOTOR_REVERSE,-motor_pwm);	
	}
	else
	{
	Motor_Control(MOTOR_STOP,0);	
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	 int16_t motor_pwm = 0;
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
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  Motor_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if (Key_Pressed(GPIOE, KEY_0_Pin))
        {
             motor_pwm+=MOTOR_STP;
					if(motor_pwm>=MOTOR_MAX)
					{
						motor_pwm=MOTOR_MAX;
					}
						Motor_pwm_set(motor_pwm);
        }
        else if (Key_Pressed(GPIOE, KEY_1_Pin))
        {
             motor_pwm-=MOTOR_STP;
					if(motor_pwm<=-MOTOR_MAX)
					{
						motor_pwm=-MOTOR_MAX;
					}
						Motor_pwm_set(motor_pwm);
        }
        else if (Key_Pressed(GPIOE, KEY_2_Pin))
        {
						motor_pwm = 0;
            Motor_pwm_set(motor_pwm);
        }
		
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
