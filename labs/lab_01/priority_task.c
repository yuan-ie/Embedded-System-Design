/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xil_printf.h"

/* Definitions */
#define INPUT_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID
#define LED_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define LED_CHANNEL 1
#define BTN_CHANNEL 1
#define SW_CHANNEL 2
#define printf xil_printf
#define DELAY_1_SECOND 1000UL

// instances of GPIO
XGpio LEDInst, InputInst;

// declare task functions
static void taskLED( void *taskLEDParams );
static void taskBTN( void *taskBTNParams );
static void taskSW( void *taskSWParams );

// declare task handles
static TaskHandle_t taskHandleLED;
static TaskHandle_t taskHandleBTN;
static TaskHandle_t taskHandleSW;

// global variable
int countDir = 0;
int twothree = 0;

int main( void ) {
	int Status;
	Status = XGpio_Initialize(&LEDInst, LED_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XGpio_Initialize(&InputInst, INPUT_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	// set write direction to LED
	XGpio_SetDataDirection(&LEDInst, LED_CHANNEL, 0x00);

	// set read direction to BTN and SW
	XGpio_SetDataDirection(&InputInst, BTN_CHANNEL, 0xFF);
	XGpio_SetDataDirection(&InputInst, SW_CHANNEL, 0xFF);

	// create tasks of LED, BTN, and SW, all with priority 1
	xTaskCreate(taskLED, (const char *) "LED Task", 1000, NULL, tskIDLE_PRIORITY + 1, &taskHandleLED);
	xTaskCreate(taskBTN, (const char *) "BTN Task", 1000, NULL, tskIDLE_PRIORITY + 1, &taskHandleBTN);
	xTaskCreate(taskSW, (const char *) "SW Task", 1000, NULL, tskIDLE_PRIORITY + 1, &taskHandleSW);

	/* Start the tasks. */
	vTaskStartScheduler();

	// for( ;; );
}

static void taskLED( void *taskLEDParams ) {
	const TickType_t x1SecondInTicks = pdMS_TO_TICKS( DELAY_1_SECOND );
	/* Declare variables */
	int led_data = 0;
	int output = 0;


	for( ;; ) {
		// if switch 0 is up, count up.
		if (countDir == 1) {
			led_data = (led_data + 1) % 16;
		}
		// if switch 0 is down, count down.
		else if (countDir == 0) {
			led_data = (led_data - 1);
		}
		output = led_data;

		// write output to LED
		XGpio_DiscreteWrite(&LEDInst, LED_CHANNEL, output);
		vTaskDelay(x1SecondInTicks);
	}
}

static void taskBTN( void *taskBTNParams ) {
	int button_data = 0;
	for( ;; ) {

		// button_data reads the inputs of the buttons
		button_data = XGpio_DiscreteRead(&InputInst, BTN_CHANNEL);


		// if button 2 & 3 are pressed, switch 0 and LED suspended
		if ((button_data & 0b1100) == 0b1100 || twothree == 1) {
			vTaskSuspend(taskHandleSW);
			vTaskSuspend(taskHandleLED);
			twothree = 1;
		}
		else {
			// if button 0 is pressed, the LED pauses
			if ((button_data & 0b0001) == 0b0001)
				vTaskSuspend(taskHandleLED);
			// else if button 0 is released, the LED continues
			else if ((button_data & 0b0001) == 0b0000)
				vTaskResume(taskHandleLED);

			// if button 1 is pressed, switch 0 does not take effect
			if ((button_data & 0b0010) == 0b0010)
				vTaskSuspend(taskHandleSW);
			// else if button 1 is released, switch 0 takes effect
			else if ((button_data & 0b0010) == 0b0000)
				vTaskResume(taskHandleSW);
		}

		// when released, switch 0 and LED resumes after 5 sec
		if ((button_data & 0b1100) != 0b1100 && twothree == 1) {
			// 5 second delay
			vTaskDelay(pdMS_TO_TICKS(5000));
			// turn flag off for buttons 2 & 3
			twothree = 0;
			// resume tasks
			vTaskResume(taskHandleSW);
			vTaskResume(taskHandleLED);
		}
	}
}

static void taskSW( void *taskSWParams ) {
	int switch_data = 0;
	for( ;; ) {

		// switch_data reads the inputs of the switches
		switch_data = XGpio_DiscreteRead(&InputInst, SW_CHANNEL);
		// turn on flag to count up
		if ((switch_data & 0b0001) == 0b0001) {
			countDir = 1;
		}
		// turn off flag to count down
		else {
			countDir = 0;
		}
	}
}
