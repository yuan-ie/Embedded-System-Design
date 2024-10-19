//LAB 3 Shell Code

// includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"

// defines
#define BTNSW_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define LED_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define BTN_CHANNEL 1
#define SW_CHANNEL 2
#define LED_CHANNEL 1

// global variables
static TaskHandle_t xTaskQueueSend; // FreeRTOS task handle static
static TaskHandle_t xTaskQueueReceive; // FreeRTOS task handle static
QueueHandle_t xQueue; // FreeRTOS queue handle
XGpio LedInst; // GPIO device driver instance
XGpio BtnSwInst; // GPIO device driver instance
volatile int click = 0;

// function declarations
int initialize();
static void vTaskQueueSend(void *pvParameters);
static void vTaskQueueReceive(void *pvParameters);

// *********************************MAIN ***************************************
int main(void) {
	// Run Initialization
	if (initialize() != XST_SUCCESS) {
		xil_printf("GPIO setup failed!\r\n");
	}
	else
		xil_printf("GPIO setup success!\r\n");

	xQueue = xQueueCreate(4, sizeof(int32_t));

	if (xQueue != NULL){

		// Create tasks of Send and Receive all with priority 1
		xTaskCreate(vTaskQueueSend, (const char *) "Sender", 1000, NULL, 1, &xTaskQueueSend);
		xTaskCreate(vTaskQueueReceive, (const char *) "Receiver", 1000, NULL, 1, &xTaskQueueReceive);

		/* Start the tasks. */
		vTaskStartScheduler();
	}

	while(1);

}
// **********************Initialize********************************
int initialize() {
	int status;
	// Initialize LEDs (GPIO driver initialization) status =
	status = XGpio_Initialize(&LedInst, LED_DEVICE_ID);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Initialize Push Buttons and Switches (GPIO driver initialization)
	status = XGpio_Initialize(&BtnSwInst, BTNSW_DEVICE_ID);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// Set LED directions to output
	XGpio_SetDataDirection(&LedInst, LED_CHANNEL, 0x00);
	// Set button directions to input
	XGpio_SetDataDirection(&BtnSwInst, BTN_CHANNEL, 0xFF);
	// Set switch directions to input
	XGpio_SetDataDirection(&BtnSwInst, SW_CHANNEL, 0xFF);

	return XST_SUCCESS;
}
// *********************TaskQueueSend*********************************
static void vTaskQueueSend(void *pvParameters) {

	int sendOne = 1;
	int recOne;
	int button_data = 0;
	int switch_data = 0;
	int prev_sw_data = XGpio_DiscreteRead(&BtnSwInst, SW_CHANNEL);
	int messages = 0;
	int output = 0;

	BaseType_t xStatus;

	while (1) {
	//Send Logic

		// get the button data
		button_data = XGpio_DiscreteRead(&BtnSwInst, BTN_CHANNEL);
		switch_data = XGpio_DiscreteRead(&BtnSwInst, SW_CHANNEL);

		// if button 0 is pressed,
		if (button_data == 0b0001){

			click++;
			xil_printf("CLICK: %d\n", click);

			// Send the value 1 to the queue
			xil_printf("button 0 pressed. sending 1 to queue.\n");

			// Send message to queue
			xStatus = xQueueSend(xQueue, &sendOne, 0);
			if(xStatus == pdPASS)
			{

				// Get the number of messages
				messages = uxQueueMessagesWaiting(xQueue);
				xil_printf("number of messages: %d\n", messages);

				// Display the number of messages in queue
				if(messages==0)
					output = 0b0000;
				else if(messages==1)
					output = 0b0001;
				else if(messages==2)
					output = 0b0011;
				else if(messages==3)
					output = 0b0111;
				else if(messages==4)
					output = 0b1111;

				// Write output to LED
				XGpio_DiscreteWrite(&LedInst, LED_CHANNEL, output);

				// Delay for 5 ms to prevent multiple clicks
				vTaskDelay(pdMS_TO_TICKS(500));


			}
			else if(xStatus == pdFAIL)
			{
				// Failed to send to the queue
				xil_printf("Queue Full!\r\n");
				vTaskDelay(pdMS_TO_TICKS(500));
			}
		}

		// reset the queue
		if ((switch_data & 1) != prev_sw_data){
			xQueueReset(xQueue);
			messages = uxQueueMessagesWaiting(xQueue);

			xil_printf("number of messages: %d\n", messages);

			prev_sw_data = switch_data;
			xil_printf("Queue reset completed.\n");

			// Write output to LED
			XGpio_DiscreteWrite(&LedInst, LED_CHANNEL, messages);

			// Delay for 5 ms to prevent multiple clicks
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	}
}
// *************************TaskQueueReceive*********************************
static void vTaskQueueReceive(void *pvParameters) {

	int receivedValue;
	int button_data = 0;
	int messages = 0;
	int output = 0;

	BaseType_t xStatus;

	while (1) {


	// Receive Log

		// Get the button data
		button_data = XGpio_DiscreteRead(&BtnSwInst, BTN_CHANNEL);

		// If button 1 is pressed
		if (button_data == 0b0010) {

			click++;
			xil_printf("CLICK: %d\n", click);

			xil_printf("button 1 pressed. receiving 1 from queue.\n");

			// Receive message from the queue
			xStatus = xQueueReceive(xQueue, &receivedValue, 0);
			if(xStatus == pdPASS)
			{
				// Get the number of messages
				messages = uxQueueMessagesWaiting(xQueue);
				xil_printf("number of messages: %d\n", messages);

				// Successfully received from the queue
				if(messages==0)
					output = 0b0000;
				else if(messages==1)
					output = 0b0001;
				else if(messages==2)
					output = 0b0011;
				else if(messages==3)
					output = 0b0111;
				else if(messages==4)
					output = 0b1111;

				// Write output to LED
				XGpio_DiscreteWrite(&LedInst, LED_CHANNEL, output);

				// Delay for 5 ms to prevent multiple clicks
				vTaskDelay(pdMS_TO_TICKS(500));
			}
			else if(xStatus == pdFAIL)
			{
				// Failed to send to the queue
				xil_printf("Queue Empty!\r\n");
				vTaskDelay(pdMS_TO_TICKS(500));
			}
		}
	}
}
