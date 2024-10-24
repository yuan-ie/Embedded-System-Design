3	Software Timer 
The shell code below creates BtnTask for controlling the LEDs. 
The task for this problem is to modify the shell code to and explain
how the LEDs will be look like with your codes.
•	Create a first software timer that is a one-shot timer that
    inverts the LEDs every 5 seconds. 
•	Create a second software time that is an auto-reload timer that
    increments the LEDs and resets the one-shot timer every 30 seconds. 	

void vBtnTask(void *pvParameters);
void vOneShotTimerCallback(TimerHandle_t xTimer);
void vAutoReloadTimerCallback(TimerHandle_t xTimer);

TaskHandle_t xBtnTask;
int led_output;

int main(void) {

    // initialize GPIO devices
    XGpio_Initialize(&LedBtnInst, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&LedBtnInst, LED_CHANNEL, 0x00);

    // initially set the LEDs to 0000
    led_output = 0b0000;
    XGpio_DiscreteWrite(&LedBtnInst, LED_CHANNEL, led_output); 

    // create timers
    xTaskCreate = (vBtnTask, (const char *)"Button Task", 1000, NULL, 1, &xBtnTask);
    xOneShotTimer = xTimerCreate("OneShotTimer", pdMS_TO_TICKS(5000), pdFALSE, 0, vOneShotTimerCallback);
    xAutoReloadTimer = xTimerCreate("AutoReloadTimer", pdMS_TO_TICKS(30000), pdTRUE, 0, vAutoReloadTimerCallback);

    // start the timers
    xTimerStart(xOneShotTimer, 0);
    xTimerStart(xAutoReloadTimer, 0);
    
    // pass program control to the FreeRTOS scheduler 
    vTaskStartScheduler();

return 0;
}

// One-shot timer callback function
void vOneShotTimerCallback(TimerHandle_t xTimer) {
    // Invert the LEDs state
    led_output ^= 0b1111; // Toggle all 4 LEDs
    XGpio_DiscreteWrite(&LedBtnInst, LED_CHANNEL, led_output);
}

// Auto-reload timer callback function
void vAutoReloadTimerCallback(TimerHandle_t xTimer) {
    // Increment the LEDs
    led_output = (led_output + 1) & 0b1111; // Increment and keep within 4 bits (0-15)
    // Reset the one-shot timer to ensure it inverts the LEDs again after 5 seconds
    xTimerReset(xOneShotTimer, 0);
}

// BtnTask
void vBtnTask(void *pvParameters) { 
    while (1) { 
        XGpio_DiscreteWrite(&LedBtnInst, LED_CHANNEL, led_output);

}