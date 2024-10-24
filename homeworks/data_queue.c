// includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"

// definitions
#define BTN_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define SW_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID

#define BTN_CHANNEL 1
#define SW_CHANNEL 2

// global variables
static TaskHandle_t xTaskBtnHandle;
static TaskHandle_t xTaskErrorMode;
static QueueHandle_t xQueueBtnSw;

// instances
XGpio BtnInst;
XGpio SwInst;

// function declarations
int initialize();
static void vTaskBTN(void *pvParameters);

// *********************************MAIN **********************************
int main(void) {

// Run Initialization
    if (initialize() != XST_SUCCESS) {
    xil_printf("GPIO setup failed!\r\n");
    }

    // Create Tasks
    xQueueBtnSw = xQueueCreate(5, sizeof(int32_t));

    // If the queue exists, create the tasks and start the scheduler
    if (xQueueBtnSw!=NULL){
        xTaskCreate(vTaskBTN, (const char *) "button task", 1000, NULL, 1, &xTaskBtnHandle);

        // Pass control to FreeRTOS
        vTaskStartScheduler();
    }
}


// **********************Initialize********************************
int initialize() {
    int status;

    // Initialize Push Buttons and Switches (GPIO driver initialization)
    status = XGpio_Initialize(&BtnInst, BTN_DEVICE_ID);
    if (status != XST_SUCCESS)
    return XST_FAILURE;

    status = XGpio_Initialize(&SwInst, SW_DEVICE_ID);
    if (status != XST_SUCCESS)
    return XST_FAILURE;

    // Set button directions to input
    XGpio_SetDataDirection(&BtnInst, BTN_CHANNEL, 0xFF);

    // Set switch directions to input
    XGpio_SetDataDirection(&SwInst, SW_CHANNEL, 0xFF);
}

// *************************TaskBTN*********************************
/*
o	When a switch is ON, and the corresponding button is depressed,
    TaskBTN writes a value to front of the queue that identifies the
    button (e.g. if SW0 is ON and BTN0 is depressed, then 0 is written
    to the queue). If the queue is full, wait 100ms.  If cant write,
    print error.
o	When a switch is OFF, and the corresponding button is depressed,
    TaskBTN reads a value from the queue (e.g. if SW0 is OFF and BTN0
    is depressed, then a value is read from the queue).  If the queue
    is empty, wait 100ms.  If cant read, print error.
*/
static void vTaskBTN(void *pvParameters) {
    int btn_config;
    int sw_config;
    int read_value;
    int send_value;
    int sw_config0;
    int sw_config1;
    int btn_config0;
    int btn_config1;

    BaseType_t xStatus;

    while (1) {
    sw_config = XGpio_DiscreteRead(&SwInst, SW_CHANNEL);
    btn_config = XGpio_DiscreteRead(&BtnInst, BTN_CHANNEL);

    // mask the configs
    sw_config0 = sw_config & 0b0001;
    btn_config0 = btn_config & 0b0001;
    sw_config1 = sw_config & 0b0010;
    btn_config1 = btn_config & 0b0010;

        // if switch 0 on or switch 1 is on
        if ((sw_config0 == 0b0001) || (sw_config1 == 0b0010)){

            // switch 0 is on and button 0 is clicked
            if (btn_config0 == sw_config0)
                send_value = 0;
            // switch 1 in on and button 1 is clicked
            else if(btn_config1 == sw_config1)
                send_value == 1;

            // if queue is full, wait 100 ms. else, fail.
            xStatus = xQueueSend(xQueueBtnSw, &send_value, pdMS_TO_TICKS(100));
            if (xStatus == pdPASS){
                xil_printf("(%d) sent to queue!\n", send_value);
                // prevents multiple clicks in one click
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            else{
                xil_printf("Queue is Full!\n");
                // prevents multiple clicks in one click
                vTaskDelay(pdMS_TO_TICKS(200));
            }
        }

        // if either switch is off
        else if ((sw_config0 == 0b0000) || (sw_config1 == 0b0000)){
            // switch 0 is off and button 0 is clicked <OR> switch 1 is off and button 1 is clicked
            if ((btn_config0 != sw_config0) || (btn_config1 != sw_config1)){

                // receive from queue. if queue is empty, wait 100 ms.
                xStatus = xQueueReceive(xQueueBtnSw, &read_value, pdMS_TO_TICKS(100));
                if (xStatus == pdPASS){
                    xil_printf("(%d) sent to queue!\n", send_value);
                    // prevents multiple clicks in one click
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
                else{
                    xil_printf("Queue is Full!\n");
                    // prevents multiple clicks in one click
                    vTaskDelay(pdMS_TO_TICKS(200));
                }
            }
        }
    }
}
