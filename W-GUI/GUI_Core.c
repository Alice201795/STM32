#include "GUI.h"
#include "GUI_Core.h"

HardDraw* PHardDrawFUN;

void GUI_LoadFunction ()
{
	DriverTreeData* PTEMP;
	LCDData* PLCDdata;
	for (PTEMP=PDriverPassTree;PTEMP->DriverInfo.DriverID!=22 && PTEMP->PNext!=NULL;PTEMP=PTEMP->PNext);//寻找已加载的显示驱动
	if (PTEMP->DriverInfo.DriverID==22)//如果有符合的驱动就把基本绘图函数链接起来
	{/*驱动正确*/
		PHardDrawFUN=SYS_CallMem (sizeof (HardDraw),GUI_MENID);
		PLCDdata=PTEMP->DriverInfo.PInfo;
		PHardDrawFUN->DrawPoint=PLCDdata->DrawPoint;
		PHardDrawFUN->DrawRange=PLCDdata->DrawRange;
		PHardDrawFUN->ReadColor=PLCDdata->ReadColor;
	}
	else
	{/*驱动错误*/
		
	}
}
void GUI_Init ()
{
	SYS_CallHEAP (GUI_MEMSIZE,GUI_MENID);
	GUI_LoadFunction ();
	PHardDrawFUN->DrawRange (3,3,7,7,0xffff);
}
