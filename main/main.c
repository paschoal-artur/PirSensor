#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>

#define PIR_SENSOR_GPIO GPIO_NUM_4
#define LED_GPIO GPIO_NUM_2

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
}

void pir_sensor_task(void *pvParameters)
{
    int last_pir_state = 0; 
    int current_pir_state = 0;
    const int debounce_time = 100 / portTICK_PERIOD_MS; //! Debounce time in milliseconds
    int last_debounce_time = 0; //! Last time the PIR output pin was toggled

    while(1)
    {
        int read_pir_state = gpio_get_level(PIR_SENSOR_GPIO); 

        //! Check if PIR state has changed
        if (read_pir_state != last_pir_state) {
            last_debounce_time = xTaskGetTickCount(); //! Reset the debouncing timer
        }

        if ((xTaskGetTickCount() - last_debounce_time) > debounce_time) {

            if (current_pir_state != read_pir_state) {
                current_pir_state = read_pir_state; //! Update the stable PIR state

                if (current_pir_state) {
                    printf("Motion detected, turning LED on\n");
                    gpio_set_level(LED_GPIO, 1); 
                } else {
                    printf("No motion detected. LED off\n");
                    gpio_set_level(LED_GPIO, 0); 
                }
            }
        }

        last_pir_state = read_pir_state; //! Update the last PIR state for the next loop iteration
        vTaskDelay(pdMS_TO_TICKS(50)); //! Short delay to prevent excessive CPU usage, adjust as needed
    }
}

void app_main(void)
{
    init_gpio();
    xTaskCreate(pir_sensor_task, "pir_sensor_task", 2048, NULL, 5, NULL);
}