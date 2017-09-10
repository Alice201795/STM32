#include "GUI.h"
#include "GUI_Core.h"

HardDraw* PHardDrawFUN;

void GUI_LoadFunction ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//Ѱ���Ѽ��ص���ʾ����
	if (PTEMP->DriverInfo.DriverID==22)//����з��ϵ������Ͱѻ�����ͼ������������
	{/*������ȷ*/
		PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUI_MENID);
		PLCDdata=PTEMP->DriverInfo.PInfo;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
	}
	else
	{/*��������*/
		
	}
}
void GUI_Init ()
{
	SYS_CallHEAP (GUI_MEMSIZE,GUI_MENID);
	GUI_LoadFunction ();
	PHardDrawFUN->DrawRange (3,3,7,7,0xffff);
}
