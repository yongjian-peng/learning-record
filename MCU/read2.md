#include "stm32f4xx.h"                  // Device header
#include "key.h"
#include "led.h"
#include "beep.h"
#include "function.h"
#include "usart.h"
#include "stdio.h"
#include "core_cm4.h"
#include "nvic.h"
#include "string.h"

static void delay_ms(u32 ms)
{
	u32 i = 168 / 4 * 1000 * ms;
	while(i)
	{
		i--;
	}
}

 


int main(void)
{
	
	u8 led_flag = 0;
	
	u8 led_speed = 5;
	u8 key;
	beep_init();
	led_init();
	key_init();
	usart1_init(115200);
	
	
	// void NVIC_SetPriorityGrouping(uint32_t PriorityGroup); // 分配抢占和响应各占多少位的。 1535
	NVIC_SetPriorityGrouping(5);

	
	
	
	
	
	while(1)
	{
		
		// key = key_scan();
		
		
		
//		if (key != 0xff)
//		{
//			usart1_send_byte(key+48);
//		}
		
		
		
		if (usart1_val.usart1_flag == 1) {
			usart1_val.usart1_flag = 0;		
			if (strcmp((char *)usart1_val.usart1_buff, "open") == 0) 
			{
				led_flag = 1;
			}
		
			else if (strcmp((char *)usart1_val.usart1_buff, "close") == 0) 
			{
				led_flag = 0;
				allled_off();
			}	
			// sp1 ~ 9 来控制 流水灯的速度
			
			printf("接收到的字符串长度是: %d\r\n\r\n", strlen((char *)usart1_val.usart1_buff));
			
			if (strlen((char *)usart1_val.usart1_buff) == 3 && strcmp((char *)usart1_val.usart1_buff, "sp1") >= 0 && strcmp((char *)usart1_val.usart1_buff, "sp9") <= 0)
			{
				led_speed = usart1_val.usart1_buff[2] - 48;
			}
			
		}
		
		// 按键扫描 如果是按键 1  则 控制蜂鸣器的开关
		key = key_scan();
		
		
		if (key == 1)
		{
			beep_overturn;
		}
		
		
		if (led_flag == 1)
		{
			led_flash(led_speed);
		}
		
		
		
	}
	
	return  0;
}





