#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <stdbool.h>
#include <esp_log.h>

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
    gpio_set_direction(BUZZER_GPIO, OUTPUT);
}

void pir_sensor_task(void *pvParameters)
{
    int last_pir_state = 0; 
    int current_pir_state = 0;
    const int debounce_time = 100 / portTICK_PERIOD_MS; //! Debounce time in milliseconds
    int last_debounce_time = 0; //! Last time the PIR output pin was toggled
    bool mudou = false;
    double start_time = 0;
    double end_time = 0;
    while(1)
    {
        int read_pir_state = gpio_get_level(PIR_SENSOR_GPIO); 
        printf("%d\n",read_pir_state);
        //! Check if PIR state has changed
        if ((read_pir_state != 0) && (!mudou)){
            mudou = true;
            start_time =  (double) esp_timer_get_time()/1000;
            printf("Delay\n");
        }
        else end_time = (double) esp_timer_get_time()/1000;
        
        if(end_time - start_time > 7*1000){
            gpio_set_level(LED_GPIO, 1);
            gpio_set_level(BUZZER_GPIO, 1);
        }

        //last_pir_state = read_pir_state; //! Update the last PIR state for the next loop iteration
        vTaskDelay(pdMS_TO_TICKS(50)); //! Short delay to prevent excessive CPU usage, adjust as needed
    }
}

void app_main(void)
{
    init_gpio();
    xTaskCreate(pir_sensor_task, "pir_sensor_task", 2048, NULL, 5, NULL);
}