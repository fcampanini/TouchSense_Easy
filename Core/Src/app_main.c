/*
*Copyright 2021 Filippo Campanini
*
*CC BY-NC-SA License
*
*http://creativecommons.org/licenses/by-nc-sa/4.0/
*
*NOTICES
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
*INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "app_main.h"

#define TOUCH_THRESHOLD			4000 // the threshold to trigger the touch condition

uint16_t ADC_value;

/**
 * @brief  Sense the touch condition returning the ADC value
 * @return The current ADC value
*/
static uint16_t TouchSense(void);

void app_main()
{
	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	HAL_TIM_Base_Start_IT(&htim1);

	while(1)
	{
		if(ADC_value < TOUCH_THRESHOLD)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		}
		else if(ADC_value >= TOUCH_THRESHOLD)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		}
	}
}

static uint16_t TouchSense()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_ChannelConfTypeDef sConfig = {0};
	uint16_t result = 0;

	/*Configure GPIO pins : CapTouch_Second pin and set it to VDD */
	GPIO_InitStruct.Pin = CapTouch_Second_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(CapTouch_Second_GPIO_Port, &GPIO_InitStruct);

	/* Point ADC to secondary line to charge the ADC cap*/
	sConfig.Channel = ADC_CHANNEL_10;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/*Configure GPIO pins : CapTouch_Second and ground the pin*/
	GPIO_InitStruct.Pin = CapTouch_Sensor_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CapTouch_Sensor_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : Set sensor line as input */
	GPIO_InitStruct.Pin = CapTouch_Sensor_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CapTouch_Sensor_GPIO_Port, &GPIO_InitStruct);

	/* Point ADC to sensor channel*/
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	result = HAL_ADC_GetValue(&hadc1);

	return result;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	  if (htim == &htim1)
	  {
		  ADC_value = TouchSense();
	  }
}
