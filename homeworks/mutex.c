#define LED_CHANNEL 1
#define BTN_CHANNEL 1
#define SW_CHANNEL 2

void vBtnTask(void *pvParameters);
TaskHandle_t xBtnTask;

void vSwTask(void *pvParameters);
TaskHandle_t xSwTask;

int main(void) {
    // initialize GPIO devices
    XGpio_Initialize(&LedBtnInst, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&LedBtnInst, LED_CHANNEL, 0x00);
    XGpio_SetDataDirection(&LedBtnInst, BTN_CHANNEL, 0xFF);
    XGpio_SetDataDirection(&LedBtnInst, SW_CHANNEL, 0xFF);

    // initially set the LEDs to 0000
    led_output = 0b0000;
    XGpio_DiscreteWrite(&LedBtnInst, LED_CHANNEL, led_output); 
    #your code for creating mutex and tasks

    // pass program control to the FreeRTOS scheduler 
    vTaskStartScheduler();

    return 0;
}
 
// BtnTask
void vBtnTask(void *pvParameters) { 
while (1) { 
#your code
}
}

// SwTask
#your code

}
}






