/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/pulse_cnt.h"
#include "bdc_motor.h"
#include "pid_ctrl.h"

static const char *TAG = "STEP";

// Enable this config,  we will print debug formated string, which in return can be captured and parsed by Serial-Studio
#define SERIAL_STUDIO_DEBUG           CONFIG_SERIAL_STUDIO_DEBUG

#define BDC_MCPWM_TIMER_RESOLUTION_HZ 10000000 // 10MHz, 1 tick = 0.1us
#define BDC_MCPWM_FREQ_HZ             25000    // 25KHz PWM
#define BDC_MCPWM_DUTY_TICK_MAX       (BDC_MCPWM_TIMER_RESOLUTION_HZ / BDC_MCPWM_FREQ_HZ) // maximum value we can set for the duty cycle, in ticks
// Motor 1
#define BDC_MCPWM_GPIO_A              22
#define BDC_MCPWM_GPIO_B              21
// Motor 2 
#define BDC_MCPWM_GPIO_C              19
#define BDC_MCPWM_GPIO_D              18

void app_main(void)
{
    ESP_LOGI(TAG, "Create DC motor 1");
    bdc_motor_config_t motor_1_config = {
        .pwm_freq_hz = BDC_MCPWM_FREQ_HZ,
        .pwma_gpio_num = BDC_MCPWM_GPIO_A,
        .pwmb_gpio_num = BDC_MCPWM_GPIO_B,
    };
    ESP_LOGI(TAG, "Create DC motor 2");
    bdc_motor_config_t motor_2_config = {
        .pwm_freq_hz = BDC_MCPWM_FREQ_HZ,
        .pwma_gpio_num = BDC_MCPWM_GPIO_C,
        .pwmb_gpio_num = BDC_MCPWM_GPIO_D,
    };
    bdc_motor_mcpwm_config_t mcpwm_1_config = {
        .group_id = 0,
        .resolution_hz = BDC_MCPWM_TIMER_RESOLUTION_HZ,
    };
    bdc_motor_mcpwm_config_t mcpwm_2_config = {
        .group_id = 1,
        .resolution_hz = BDC_MCPWM_TIMER_RESOLUTION_HZ,
    };
    bdc_motor_handle_t motor_1 = NULL;
    ESP_ERROR_CHECK(bdc_motor_new_mcpwm_device(&motor_1_config, &mcpwm_1_config, &motor_1));
    bdc_motor_handle_t motor_2 = NULL;
    ESP_ERROR_CHECK(bdc_motor_new_mcpwm_device(&motor_2_config, &mcpwm_2_config, &motor_2));

    ESP_LOGI(TAG, "Enable motors");
    ESP_ERROR_CHECK(bdc_motor_enable(motor_1));
    ESP_ERROR_CHECK(bdc_motor_enable(motor_2));
    
    while (1) {
        bdc_motor_set_speed(motor_1,300);
        bdc_motor_set_speed(motor_2,400);
    
        ESP_LOGI(TAG, "Forward motor 1");
        ESP_ERROR_CHECK(bdc_motor_forward(motor_1));
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "reverse motor 1");
        ESP_ERROR_CHECK(bdc_motor_reverse(motor_1));
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "stop motor 1");
        ESP_ERROR_CHECK(bdc_motor_coast(motor_1));
        vTaskDelay(pdMS_TO_TICKS(2000));   
        ESP_LOGI(TAG, "Forward motor 2");
        ESP_ERROR_CHECK(bdc_motor_forward(motor_2));
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "reverse motor 2");
        ESP_ERROR_CHECK(bdc_motor_reverse(motor_2));
        vTaskDelay(pdMS_TO_TICKS(2000));
        ESP_LOGI(TAG, "stop motor 2");
        ESP_ERROR_CHECK(bdc_motor_coast(motor_2));
        vTaskDelay(pdMS_TO_TICKS(2000));   
    }
}
