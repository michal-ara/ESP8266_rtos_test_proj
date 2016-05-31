#include <esp_common.h>

#include <esp8266/uart_register.h>
#include <esp8266/eagle_soc.h>
#include <esp_softap.h>

#include "gpio.h"
#include "uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define LED 2
#define BUTTON 0
#define UART_QUEUE_LEN 3

#define SSID_NAME "test"
#define SSID_PASS "test_pass"

extern xQueueHandle xQueue;
xSemaphoreHandle semphr = NULL;
xSemaphoreHandle semphr1 = NULL;

void wifi_init(void)
{
	printf("\r\n Wifi initialization ... \r\n");

	struct softap_config sysConf;
	struct softap_config oldConf;

	sprintf(sysConf.ssid, "%s", SSID_NAME);
	sprintf(sysConf.password, "%s", SSID_PASS);

	wifi_softap_get_config(&oldConf);
	//oldInfo = wifi_softap_get_station_info();
	if( DHCP_STARTED == wifi_softap_dhcps_status())
		wifi_softap_dhcps_stop();


	printf("Name : %s Pass : %s \r\n", oldConf.ssid, oldConf.password);
	sprintf(oldConf.ssid, "%s", SSID_NAME);

	wifi_softap_set_config(&oldConf);
	printf("Name : %s Pass : %s \r\n", oldConf.ssid, oldConf.password);

	wifi_softap_dhcps_start();

	//if(wifi_softap_)

}


void task2(void *pvParameters)
{

	while(1)
	{
		if(semphr != NULL)
		{
			if(xSemaphoreTake( semphr, (portTickType) 10 ))
			{

				//vTaskDelay(1000 / portTICK_RATE_MS);

				xSemaphoreGive(semphr);
			}
		}
	}

}

void vTaskButton(void *pvParameters)
{
	uint32 inValueCount = 0;
	int inValue = 0;
	printf("Button task start ...");
	while(1)
	{
		if(UINT32_MAX == inValueCount) inValueCount = 0;
		vTaskDelay(500 / portTICK_RATE_MS);
		//printf("Task 1\n");
		gpio_output_set(0,0,0,BUTTON);
		inValue = GPIO_INPUT_GET(BUTTON);
		printf("%d : ", (system_get_time() / portTICK_RATE_MS) /10000);
		printf(" %d \n\r", inValue);


	}

}

void vTaskLed(void *pvParameters)
{
	uint8_t state = 0;
	printf("LED task start ...");
	while(1)
	{

		//gpio_output_set(state?1<<LED:0, state?0:1<<LED, 1<<LED ,0);
		printf("LED %s \n", state?"off":"on" );
		state ^= 1;
		vTaskDelay(2000 / portTICK_RATE_MS);
	}

}

void vTaskUartReceiver(void *pvParameters)
{

	printf("UART receiver start ...");
	const int max_char = 10;

	uart_rx rxChar;
	char msg[max_char];
	int count = 0;

	while(1)
	{
		//strcpy(rxChar, "");
		if( xQueueReceive(xQueue, &rxChar, 1000) )
		{

			if(0 == count)
				printf("=:");
			if(10==(int)rxChar.rx_char)
			{
				//msg[count] = rxChar.rx_char;
				count =0;
				printf("\n\r");
				continue;
				//printf("=: %s", msg);

			}
			printf("%c", rxChar.rx_char);
			count++;
		}

	}


}

void user_init(void)
{

	const_uart();

	wifi_init();

	printf("\r\nSystem started (SDK : %s)... \r\n", system_get_sdk_version() );





	printf("\r\nInit completed... \r\n");

	semphr = xSemaphoreCreateMutex();



	//vTaskEndScheduler();
	xTaskCreate(vTaskLed, "TASK_LED", 256, NULL, 2, NULL);
	//xTaskCreate(vTaskButton, "TASK_button", 256, NULL, 2, NULL);

	//xTaskCreate(task3, "TASK_3", 256, NULL, 2, NULL);

	xTaskCreate(vTaskUartReceiver, "uart_reciver", 256, NULL, 2, NULL);

	//vTaskStartScheduler();

	//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	//gpio_output_conf(0, 0, 0, BUTTON);


}

