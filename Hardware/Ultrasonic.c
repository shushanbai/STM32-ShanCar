#include "stm32f10x.h"                  // Device header
#include "Delay.h"   


float Cnt;


/**
  * @brief  初始化超声波测距模块，配置GPIO和定时器
  * @param  无
  * @retval 无
  */
void Ultrasonic_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//trig
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//echo
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_InternalClockConfig(TIM4);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	//计数次数 = (输入时钟频率72MHz)/(PSC+1) = 1MHz,
	//每次计数时间 = 1/计数频率 = 1/1MHz = 1us
	//计数周期 = 计数总次数*每次计数时间 = (ARR+1)/计数频率 = 60000/1MHz = 60ms

	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

}

/**
  * @brief  获得触发超声波测量双程时间
  * @param  无
  * @retval 时间，单位微秒
  */
float Get_Time()
{
	//1.触发超声波测量:
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//连接在PB12上的TRIG引脚变成高电平
	Delay_us(20);	
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	/*常见HC-SR04模块要求TRIG输入一个至少约10μs的高电平触发脉冲
	以上3行：发出高点平信号>10us给Trig口，发出超声波*/
	
	//2.等待ECHO变为高电平，最多等待30ms
	TIM_Cmd(TIM4, DISABLE);
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
		//计数清零重启，开始计数

    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
    {
        if (TIM_GetCounter(TIM4) >= 30000)
        {
            TIM_Cmd(TIM4, DISABLE);
            TIM_SetCounter(TIM4, 0);
            return -1.0f;  //超时，没有等到回波，返回错误值
        }
    }

    // 3. ECHO变高，重新开始计时
    TIM_Cmd(TIM4, DISABLE);
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);

    // 4. 等待ECHO变为低电平，最多等待30ms
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == SET)
    {
        if (TIM_GetCounter(TIM4) >= 30000)
        {
            TIM_Cmd(TIM4, DISABLE);
            TIM_SetCounter(TIM4, 0);
            return -1.0f;  //超时，返回错误值
        }
    }
	
	TIM_Cmd(TIM4, DISABLE);
	Cnt=TIM_GetCounter(TIM4);//单位：微秒
	return Cnt;
}

/**
  * @brief  计算测量距离
  * @param  无
  * @retval 距离(cm)
  */
float Get_Distance()
{
	Cnt = Get_Time();
	
	if (Cnt <= 0)
	{
		return -1.0f; //没有障碍物/回波出错
	}
	
	//换算距离
		float distance=(Cnt*1.0/10*0.34)/2;//单位：cm
		//常温下声速大约为343m/s，也就是约0.343mm/μs
	
		TIM4->CNT=0;//清零计数器
		Delay_ms(100);
		return distance;
		
}
//float是为了获取更高精度的距离



