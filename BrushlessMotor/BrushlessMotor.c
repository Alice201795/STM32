#include "BrushlessMotor.h"
#define A GPIO_Pin_8
#define B GPIO_Pin_9
#define C GPIO_Pin_10
#define US 72
#define HalfCYCLE 500//500usΪһ��pwm����
#define PI_Init 120

static uint8_t flag=1;//���Ա���

static BrushlessMotorData BrushlessMotordata;
volatile uint16_t pi,freetime=20;

void GPIOA_RCC_Init ()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
}
void GPIOA_8_9_10_Init()
{
	GPIO_InitTypeDef GPIO_InitData;
	GPIO_InitData.GPIO_Pin = A | B | C;
	GPIO_InitData.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitData.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitData);
}
void Time1_Init ()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);/*ʹ�ܶ�ʱ��1��ʱ��*/
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;/*��ʱ��1���ж�ͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;/*��ʱ��1���ж�ͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 22;/*��ռ���ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;/*��Ӧ���ȼ�*/
	NVIC_Init(&NVIC_InitStructure);/*�����жϷ��飬��ʹ���ж�*/

	TIM_ARRPreloadConfig(TIM1, ENABLE);/*����TIM1Ӱ�ӼĴ���*/
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);/*����OCӰ�ӼĴ���*/
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_TimeBaseInitStrecture.TIM_Period =(1.5f*freetime)+HalfCYCLE;/*��װ�ؼĴ���*/
	TIM_TimeBaseInitStrecture.TIM_Prescaler = US;/*Ԥ����*/
	TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*ʱ�ӷ�Ƶ*/
	TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_CenterAligned1;/*�������*/
	TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*�ظ������Ĵ���*/
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStrecture);/*��ʼ��*/

	TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*���־λ*/
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*ʹ���ж�*/
}
void BrushlessMotor_SVPWM()
{
	uint16_t block=0;
	block=pi/60;
	switch (block)
	{
		//HalfCYCLE+freetime 0 																ʵ�� 1
		//freetime+(1.1547*(sinlist[p])*HalfCYCLE) ��С       ʵ�� ����
		//freetime+(1.1547*(sinlist[60-p])*HalfCYCLE) ����    ʵ�� ��С 
		//freetime 1 																					ʵ�� 0
		case 0 :		TIM1->CCR1 = HalfCYCLE+freetime;TIM1->CCR2=freetime+(1.1547f*(sinlist[60-pi])*HalfCYCLE);TIM1->CCR3=freetime;	//A:0 B:1-0 C:1
								break;
		case 1 :/*59*/TIM1->CCR1 = freetime+(1.1547f*(sinlist[pi])*HalfCYCLE);TIM1->CCR2=freetime;TIM1->CCR3 =HalfCYCLE+freetime;	//A:0-1 B:0 C:1
								break;
		case 2 :/*0*/TIM1->CCR1=freetime;TIM1->CCR2=HalfCYCLE+freetime;TIM1->CCR3=freetime+(1.1547f*(sinlist[180-pi])*HalfCYCLE);	//A:1 B:0 C:1-0
								break;
		case 3 : 		TIM1->CCR1 =freetime;TIM1->CCR2 = freetime+(1.1547f*(sinlist[pi])*HalfCYCLE);TIM1->CCR3 = HalfCYCLE+freetime; 	//A:1 B:0-1 C:0
								break;
		case 4 : 		TIM1->CCR1 =freetime+(1.1547f*(sinlist[300-pi])*HalfCYCLE);TIM1->CCR2=freetime;TIM1->CCR3=HalfCYCLE+freetime; 	//A:1-0 B:1 C:0
								break;
		case 5 : 		TIM1->CCR1 = HalfCYCLE+freetime;TIM1->CCR2 =freetime;TIM1->CCR3 = freetime+(1.1547f*(sinlist[pi])*HalfCYCLE); 	//A:0 B:1 C:0-1
								break;
		case 6 :		TIM1->CCR1 = HalfCYCLE+freetime;TIM1->CCR2=freetime+(1.1547f*(sinlist[60-pi])*HalfCYCLE);TIM1->CCR3=freetime;	//A:0 B:1-0 C:1
	}
}
void TIM1_UP_IRQHandler()
{
	
	if (TIM1->CNT>HalfCYCLE>>1) //���е�
	{
		TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE);//�رն�ʱ���ж�
		BrushlessMotor_SVPWM();
		TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*���жϱ�־*/
		TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);/*ʹ���ж�*/
	}
	else /*�ڱ�Ե*/TIM_ClearFlag(TIM1,TIM_FLAG_Update);/*���жϱ�־*/
}
void BrushlessMotor_SVPWM_Init()
{
	GPIO_SetBits(GPIOB, A);
	GPIO_SetBits(GPIOB, B);
	GPIO_SetBits(GPIOB, C);
	pi=PI_Init;
	BrushlessMotor_SVPWM();
}
void BrushlessMotor_Turn()
{
	if (flag)++pi;
	else --pi;
	if(pi>=121) flag=0;
	else if (pi<=119) flag=1;
}
void BrushlessMotor_Init ()
{
	GPIOA_8_9_10_Init ();
	GPIOA_RCC_Init ();
	Time1_Init ();
	BrushlessMotor_SVPWM_Init();
	TIM_Cmd(TIM1,ENABLE);/*ʹ�ܼ���*/
}
TESTPASS BrushlessMotor_TEST ()
{
	return PASS;
}
void BrushlessMotor_LoadInfo (DriverInfoData* PDriverInfo)
{
	BrushlessMotordata.TurnPower=BrushlessMotor_Turn;
	PDriverInfo->Init=BrushlessMotor_Init;
	PDriverInfo->Test=BrushlessMotor_TEST;
	PDriverInfo->PInfo=&BrushlessMotordata;
	PDriverInfo->DriverID=51;
}
