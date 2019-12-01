/**
  ******************************************************************************
  * File Name          : FunctionTask.c
  * Description        : ���ڼ�¼�����˶��еĹ���
  ******************************************************************************
  *
  * Copyright (c) 2019 Team Jiao Long-Shanghai Jiao Tong University
  * All rights reserved.
  *
  ******************************************************************************
  */
#include "includes.h"

float rotate_speed = 0;
KeyboardMode_e KeyboardMode = NO_CHANGE;
RampGen_t LRSpeedRamp = RAMP_GEN_DAFAULT;
RampGen_t FBSpeedRamp = RAMP_GEN_DAFAULT;
ChassisSpeed_Ref_t ChassisSpeedRef; 

int32_t auto_counter = 0;                                                                       //�Զ������������������ÿ�����Լ�1������������ʱ����Զ�������
                                                                                              //��������Ҳ������ControlTask.c��158�д��ҵ��Լ���ԭ�����ɴ�����������ʱ�����
int16_t channelrrow = 0;                                                                      //���ĸ��������ڴ洢ң�������صĲ�����
int16_t channelrcol = 0;                                                                      //��������Ϊ�������ֵ���������ף���600����      
int16_t channellrow = 0;
int16_t channellcol = 0;

//��ʼ��
void FunctionTaskInit()
{
	LRSpeedRamp.SetScale(&LRSpeedRamp, MOUSE_LR_RAMP_TICK_COUNT);
	FBSpeedRamp.SetScale(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
	LRSpeedRamp.ResetCounter(&LRSpeedRamp);
	FBSpeedRamp.ResetCounter(&FBSpeedRamp);
	
	ChassisSpeedRef.forward_back_ref = 0.0f;
	ChassisSpeedRef.left_right_ref = 0.0f;
	ChassisSpeedRef.rotate_ref = 0.0f;
	
	KeyboardMode=NO_CHANGE;
}

void Limit_and_Synchronization()                                                             //�����������Ӳ�����Ƶ���Ƕȣ���ֹ���ת�������ʵ�λ��
{
	//demo
//	MINMAX(AnyMotor.TargetAngle,-900,270);//limit
	//demo end
}

//******************
//ң����ģʽ���ܱ�д
//******************
void RemoteControlProcess(Remote *rc)
{
	if(WorkState <= 0) return;
	//max=297
	channelrrow = (rc->ch0 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	channelrcol = (rc->ch1 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	channellrow = (rc->ch2 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
	channellcol = (rc->ch3 - (int16_t)REMOTE_CONTROLLER_STICK_OFFSET); 
////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////��Ҫ�����������д
	    
	if(WorkState == NORMAL_STATE)                                                            //�ϵ�
	{	
		// �ж� & ץ��
		// ������Ϊ�˵�������
		ChassisSpeedRef.forward_back_ref = -channelrcol * RC_CHASSIS_SPEED_REF;               //�����Ѿ�Ĭ��д���˵��̵Ŀ��ƺ��� 
		ChassisSpeedRef.left_right_ref   = -channelrrow * RC_CHASSIS_SPEED_REF/2;             //�ұ�ҡ�˿���ǰ�����ҵ�ƽ�� ���ҡ�˿�����ת
		rotate_speed = -channellrow * RC_ROTATE_SPEED_REF;                                   //RC_CHASSIS_SPEED_REF��һ��Ĭ�ϵ���ֵ���������н��ٶȴﵽ����ֵ
		                                                                                     //ChassisSpeedRef.forward_back_ref��һ����װ�õı�����ͨ���ı���
		                                                                                     //����ֱ�ӿ��Ƴ���ǰ�������ٶȣ���������ͬ��
	  if(channellcol < 0) {
			// ��� 1
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500 + 1000 * (-channellcol / 450.0));
			
			// ��� 2
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 2500 - (500 + 1000 * (-channellcol / 520.0)));
		}
	}                                                                                          
	if(WorkState == ADDITIONAL_STATE_ONE)                                                   //�е�
	{
		// �ж�
		ChassisSpeedRef.forward_back_ref = channelrcol * RC_CHASSIS_SPEED_REF;               //�����Ѿ�Ĭ��д���˵��̵Ŀ��ƺ��� 
		ChassisSpeedRef.left_right_ref   = channelrrow * RC_CHASSIS_SPEED_REF/2;             //�ұ�ҡ�˿���ǰ�����ҵ�ƽ�� ���ҡ�˿�����ת
		rotate_speed = -channellrow * RC_ROTATE_SPEED_REF;                                   //RC_CHASSIS_SPEED_REF��һ��Ĭ�ϵ���ֵ���������н��ٶȴﵽ����ֵ
		                                                                                     //ChassisSpeedRef.forward_back_ref��һ����װ�õı�����ͨ���ı���
		                                                                                     //����ֱ�ӿ��Ƴ���ǰ�������ٶȣ���������ͬ��
	}
	if(WorkState == ADDITIONAL_STATE_TWO)                                                   //�µ�
	{
		// �̶�ץ�����
		if(channellcol < 0) {
			// ��� 1
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500 + 1000 * (-channellcol / 450.0));
			
			// ��� 2
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 2500 - (500 + 1000 * (-channellcol / 520.0)));
		}
	}
	
	
////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
	Limit_and_Synchronization();
}


uint16_t KM_FORWORD_BACK_SPEED 	= NORMAL_FORWARD_BACK_SPEED;
uint16_t KM_LEFT_RIGHT_SPEED  	= NORMAL_LEFT_RIGHT_SPEED;
void KeyboardModeFSM(Key *key);

//****************
//����ģʽ���ܱ�д  У�����в����õ�����
//****************
void MouseKeyControlProcess(Mouse *mouse, Key *key)
{	
	if(WorkState <= 0) return;
	
	MINMAX(mouse->x, -150, 150); 
	MINMAX(mouse->y, -150, 150); 

	KeyboardModeFSM(key);
	
	switch (KeyboardMode)
	{
		case SHIFT_CTRL:		//State control
		{
			
			break;
		}
		case CTRL:				//slow
		{
			
		}//DO NOT NEED TO BREAK
		case SHIFT:				//quick
		{
			
		}//DO NOT NEED TO BREAK
		case NO_CHANGE:			//normal
		{//CM Movement Process
			if(key->v & KEY_W)  		//key: w
				ChassisSpeedRef.forward_back_ref =  KM_FORWORD_BACK_SPEED* FBSpeedRamp.Calc(&FBSpeedRamp);
			else if(key->v & KEY_S) 	//key: s
				ChassisSpeedRef.forward_back_ref = -KM_FORWORD_BACK_SPEED* FBSpeedRamp.Calc(&FBSpeedRamp);
			else
			{
				ChassisSpeedRef.forward_back_ref = 0;
				FBSpeedRamp.ResetCounter(&FBSpeedRamp);
			}
			if(key->v & KEY_D)  		//key: d
				ChassisSpeedRef.left_right_ref =  KM_LEFT_RIGHT_SPEED * LRSpeedRamp.Calc(&LRSpeedRamp);
			else if(key->v & KEY_A) 	//key: a
				ChassisSpeedRef.left_right_ref = -KM_LEFT_RIGHT_SPEED * LRSpeedRamp.Calc(&LRSpeedRamp);
			else
			{
				ChassisSpeedRef.left_right_ref = 0;
				LRSpeedRamp.ResetCounter(&LRSpeedRamp);
			}
		}
	}
	Limit_and_Synchronization();
}

void KeyboardModeFSM(Key *key)
{
	if((key->v & 0x30) == 0x30)//Shift_Ctrl
	{
		KM_FORWORD_BACK_SPEED=  LOW_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = LOW_LEFT_RIGHT_SPEED;
		KeyboardMode=SHIFT_CTRL;
	}
	else if(key->v & KEY_SHIFT)//Shift
	{
		KM_FORWORD_BACK_SPEED=  HIGH_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = HIGH_LEFT_RIGHT_SPEED;
		KeyboardMode=SHIFT;
	}
	else if(key->v & KEY_CTRL)//Ctrl
	{
		KM_FORWORD_BACK_SPEED=  LOW_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = LOW_LEFT_RIGHT_SPEED;
		KeyboardMode=CTRL;
	}
	else
	{
		KM_FORWORD_BACK_SPEED=  NORMAL_FORWARD_BACK_SPEED;
		KM_LEFT_RIGHT_SPEED = NORMAL_LEFT_RIGHT_SPEED;
		KeyboardMode=NO_CHANGE;
	}	
}
