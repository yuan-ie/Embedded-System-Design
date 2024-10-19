3	Software Timer 
The shell code below creates BtnTask for controlling the LEDs.  The task for this problem is to modify the shell code to and explain how the LEDs will be look like with your codes.
•	Create a first software timer that is a one-shot timer that inverts the LEDs every 5 seconds. 
•	Create a second software time that is an auto-reload timer that increments the LEDs and resets the one-shot timer every 30 seconds. 	

void vBtnTask(void *pvParameters);
TaskHandle_t xBtnTask;

int main(void) {

    // initialize GPIO devices
    XGpio_Initialize(&LedBtnInst, XPAR_AXI_GPIO_0_DEVICE_ID);

    // initially set the LEDs to 0000
    led_output = 0b0000;
    XGpio_DiscreteWrite(&LedBtnInst, LED_CHANNEL, led_output); 

    // your code 

    // pass program control to the FreeRTOS scheduler 
    vTaskStartScheduler();

return 0;
}

// BtnTask
void vBtnTask(void *pvParameters) { 
    while (1) { 
    #your code
    // OneShot
    //AutoReload

}