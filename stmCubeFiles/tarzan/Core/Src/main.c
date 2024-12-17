#include "main.h"

// Function Prototypes
void GPIO_Init(void);
void TIM16_PWM_Init(void);
void TIM3_PWM_Init(void);
void TIM6_Delay_Init(void);
void Motor_Control(uint8_t direction, uint16_t speed);
void TIM6_ms_Delay(uint32_t delay);

// Timer Handles
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;

// GPIO Initialization
void GPIO_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE(); // Enable GPIOB Clock
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Enable GPIOA Clock

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // TIM16_CH1 on PB8
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM16;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // TIM3_CH1 on PA6
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// Timer 16 Initialization (PWM)
void TIM16_PWM_Init(void) {
    __HAL_RCC_TIM16_CLK_ENABLE();

    htim16.Instance = TIM16;
    htim16.Init.Prescaler = 16 - 1;   // 1 MHz timer clock
    htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim16.Init.Period = 20000 - 1;   // 20 ms period
    htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim16);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;              // Initial duty cycle 0%
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);
}

// Timer 3 Initialization (PWM)
void TIM3_PWM_Init(void) {
    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 16 - 1;   // 1 MHz timer clock
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 20000 - 1;   // 20 ms period
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;              // Initial duty cycle 0%
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

// Timer 4 Initialization (for delay)
void TIM6_Delay_Init(void) {
    __HAL_RCC_TIM6_CLK_ENABLE();

    htim6.Instance = TIM6;
    htim6.Init.Prescaler = 16000 - 1; // 1 ms timer clock
    htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim6.Init.Period = 1000;         // Default 1-second period
    HAL_TIM_Base_Init(&htim6);
}

// Motor Control Function
void Motor_Control(uint8_t direction, uint16_t speed) {
    if (speed > 100) speed = 100; // Limit speed to 100%
    uint16_t duty_cycle = (uint16_t)((speed / 100.0) * 20000);

    if (direction == 0) { // Clockwise
        __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, duty_cycle); // TIM16 (PB8)
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);           // TIM3 (PA6)
    } else { // Counterclockwise
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty_cycle);  // TIM3 (PA6)
        __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, 0);          // TIM16 (PB8)
    }
}

// Millisecond Delay Using TIM6
void TIM6_ms_Delay(uint32_t delay) {
    __HAL_TIM_SET_AUTORELOAD(&htim6, delay);
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    HAL_TIM_Base_Start(&htim6);

    while (__HAL_TIM_GET_FLAG(&htim6, TIM_FLAG_UPDATE) == RESET) {}
    __HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
    HAL_TIM_Base_Stop(&htim6);
}

// Main Function
int main(void) {
    HAL_Init();
    SystemClock_Config();

    GPIO_Init();
    TIM16_PWM_Init();
    TIM3_PWM_Init();
    TIM6_Delay_Init();

    while (1) {
        Motor_Control(1, 70);  // Counterclockwise at 70% speed
        TIM6_ms_Delay(5000);   // Delay 5 seconds

        Motor_Control(0, 50);  // Clockwise at 50% speed
        TIM6_ms_Delay(5000);   // Delay 5 seconds
    }
}
