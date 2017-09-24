#include "stable.h"
GyroscopeData* GetGyroscopeData;
void Stable_LoadFunction ()
{
	DriverTreeData* PTEMP;
	GyroscopeData* PGyroscopeData;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=50 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ��ص���ʾ����
	if (PTEMP->DriverInfo.DriverID==50)//����з��ϵ������Ͱѻ�����ͼ������������
	{/*������ȷ*/
		PGyroscopeData=SYS_CallMem (sizeof (GyroscopeData),STABLE_MENID);
		PGyroscopeData=PTEMP->DriverInfo.PInfo;
		GetGyroscopeData->Get_X_Angular_Acceleration=PGyroscopeData->Get_X_Angular_Acceleration;
		GetGyroscopeData->Get_Y_Angular_Acceleration=PGyroscopeData->Get_Y_Angular_Acceleration;
		GetGyroscopeData->Get_Z_Angular_Acceleration=PGyroscopeData->Get_Z_Angular_Acceleration;
		GetGyroscopeData->Get_X_Acceleration=PGyroscopeData->Get_X_Acceleration;
		GetGyroscopeData->Get_Y_Acceleration=PGyroscopeData->Get_Y_Acceleration;
		GetGyroscopeData->Get_Z_Acceleration=PGyroscopeData->Get_Z_Acceleration;
	}
	else
	{/*��������*/
		
	}
}
void Stable_Init ()
{
	SYS_CallHEAP (500,STABLE_MENID);
	Stable_LoadFunction ();
	xTaskCreate(stable,"test",50,NULL,1,NULL);
}
void stable ()
{
	
}
