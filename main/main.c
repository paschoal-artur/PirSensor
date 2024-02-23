#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <stdbool.h>
#include <esp_log.h>
#include "esp_timer.h"

#define PIR_SENSOR_GPIO GPIO_NUM_4
#define LED_GPIO GPIO_NUM_2
#define BUZZER_GPIO GPIO_NUM_18

void init_gpio(void)
{
    //!PIR pin configuration
    esp_rom_gpio_pad_select_gpio(PIR_SENSOR_GPIO);
    gpio_set_direction(PIR_SENSOR_GPIO, GPIO_MODE_INPUT);
    gpio_pulldown_en(PIR_SENSOR_GPIO);
    gpio_pullup_dis(PIR_SENSOR_GPIO);

    //!LED pin configuration
    esp_rom_gpio_pad_select_gpio(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    //!Buzzer pin configuration
    esp_rom_gpio_pad_select_gpio(BUZZER_GPIO);
    gpio_set_direction(BUZZER_GPIO, GPIO_MODE_OUTPUT);

}

void pir_sensor_task(void *pvParameters)
{
    bool mudou = false;
    double start_time = 0;
    int anterior = 0;
    while(1)
    {
        int read_pir_state = gpio_get_level(PIR_SENSOR_GPIO); 
        double current_time = (double) esp_timer_get_time() / 1000;
        printf("%d\n", read_pir_state);
        if ((read_pir_state != 0) && (read_pir_state != anterior) )
        {
            if (!mudou){
                mudou = true;
                start_time =  current_time;
                gpio_set_level(LED_GPIO, 1);
                gpio_set_level(BUZZER_GPIO, 1); 
            }
            start_time = current_time;
        }

        if(mudou && (current_time - start_time > 1000)){
            gpio_set_level(LED_GPIO, 0);
            gpio_set_level(BUZZER_GPIO, 0);
            mudou = false;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
        anterior = read_pir_state;
    }
}

void app_main(void)
{
    init_gpio();
    xTaskCreate(pir_sensor_task, "pir_sensor_task", 2048, NULL, 5, NULL);
}