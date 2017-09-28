#include "SYS_Memory.h"
#include "FreeRTOS.h"

HeapList* _heaplist;
void SYS_MemoryHEAPCall (u32 size,u8 ID, HeapList* heaplist)
{
	void* TEMP;
	TEMP=heaplist->PNextHapeList;
	heaplist->PNextHapeList=pvPortMalloc (size);//�ڴ�������
	
	heaplist=heaplist->PNextHapeList;
	
	heaplist->PNextHapeList=TEMP;
	heaplist->HeapID=ID;
	heaplist->Size=size-sizeof (HeapList);
	heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)heaplist+sizeof (HeapList));
	
	heaplist->PHeapNodeHead->USEState=NOTUSED;
	heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) heaplist->PHeapNodeHead+sizeof (HeapNode) );
	heaplist->PHeapNodeHead->Size=_heaplist->Size-sizeof (HeapNode);
	heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	heaplist->PHeapNodeHead->PNextHeapNode=NULL;
}
void SYS_MemoryInit ()
{
	_heaplist=pvPortMalloc (SYS_MEMSIZE);//�ڴ�������
	_heaplist->HeapID=SYS_MENID | 0x80;
	_heaplist->Size=SYS_MEMSIZE-sizeof (HeapList);
	_heaplist->PHeapNodeHead=(HeapNode*)((uint32_t)_heaplist+sizeof (HeapList));
	
	_heaplist->PNextHapeList=NULL;
	
	_heaplist->PHeapNodeHead->USEState=NOTUSED;
	_heaplist->PHeapNodeHead->PBlock=(void *)( (uint32_t) _heaplist->PHeapNodeHead+sizeof (HeapNode) );
	_heaplist->PHeapNodeHead->Size=_heaplist->Size-sizeof (HeapNode);
	_heaplist->PHeapNodeHead->PFrontHeapNode=NULL;
	_heaplist->PHeapNodeHead->PNextHeapNode=NULL;
}
u32 SYS_MemorySizeChoice(u32 size)
{
	if (SYS_NEW_MEMSIZE<size) return SYS_NEW_MEMSIZE+SYS_MemorySizeChoice(size-SYS_NEW_MEMSIZE);
	else return SYS_NEW_MEMSIZE;
}
void* SYS_CallMem (u32 size,uint8_t ID)//�ⲿ����
{
	HeapNode* PN;//��ǰ�ڵ�
	HeapList* PL;//��ǰ��
	for (PL=_heaplist;PL->HeapID!=ID && PL->HeapID!=(ID | 0x80);PL=PL->PNextHapeList);//��ʼ��Ϊ�׶�
	while (1)
	{
		for (PN=PL->PHeapNodeHead;PN->USEState==USED;PN=PN->PNextHeapNode)//ָ���һ���ѵĵ�һ���ڵ㣬����ڵ㱻ʹ���������һ���ڵ㣬û��ʹ�þ��˳�
		{
			if (PN->PNextHeapNode);                                        //��һ���ڵ�ָ�벻Ϊ�վͼ���
			else                                                           //Ϊ�վͻ���һ���Ѳ��˳�
			{
				if (PL->PNextHapeList==NULL)                                       //�����һ���Ѳ����ھ��½�һ����
				{
					SYS_MemoryHEAPCall (SYS_MemorySizeChoice(size),0,PL);
				}
				PL=PL->PNextHapeList;                                        //��ǰ���л�Ϊ�µĶ�
				PN=PL->PHeapNodeHead;                                       //��ǰ�ڵ��л�Ϊ�µĶѵĽڵ�
			}
		}
		if (PN->Size<size+sizeof (HeapNode) )                            //�жϴ�С���ϸ�
		{
			if (PN->PNextHeapNode==NULL)            //�ж���һ���ڵ㲻���ھ��л���һ����
			{
				if (PL->PNextHapeList==NULL)        //�����һ���Ѳ����ھ��½�һ����
				{
					SYS_MemoryHEAPCall (SYS_MemorySizeChoice(size),ID,PL);
				}
				PL=PL->PNextHapeList; //��ǰ���л�Ϊ�µĶ�
				PN=PL->PHeapNodeHead;	//��ǰ�ڵ��л�Ϊ�µĶѵĽڵ�
			}
			else		                        	       //���ھͻ���һ���ڵ�
			{
				PN=PN->PNextHeapNode;
			}
		}
		else break;                                                      //�ϸ����˳�
	}
	/*�����½ڵ�*/
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->USEState=NOTUSED;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->Size=PN->Size-sizeof (HeapNode)-size;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PBlock=(void *)( (uint32_t) ((HeapNode*)((uint32_t)(PN->PBlock)+size))+sizeof (HeapNode) );
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PFrontHeapNode=PN;
	((HeapNode*)((uint32_t)(PN->PBlock)+size))->PNextHeapNode=NULL;
	/*�Ͻڵ�����*/
	PN->USEState=USED;
	PN->Size=size;
	PN->PNextHeapNode=((HeapNode*)((uint32_t)(PN->PBlock)+size));
	return PN->PBlock;
}
void  SYS_FreeMem (void * Addr)
{
	HeapNode* TEMP;
	HeapNode* FTEMP;
	HeapNode* NTEMP;
	HeapList* HTEMP;
	HeapList* FHTEMP;
	TEMP=((HeapNode*)((uint32_t)Addr-sizeof (HeapNode)));
	for (FTEMP=TEMP->PFrontHeapNode;FTEMP && FTEMP->USEState==NOTUSED;FTEMP=FTEMP->PFrontHeapNode);
	for (NTEMP=TEMP->PNextHeapNode;NTEMP && NTEMP->USEState==NOTUSED;NTEMP=NTEMP->PNextHeapNode);
	if (FTEMP && NTEMP)
	{
		FTEMP->PNextHeapNode->PNextHeapNode=NTEMP;
		NTEMP->PFrontHeapNode=FTEMP->PNextHeapNode;
		FTEMP->PNextHeapNode->USEState=NOTUSED;
		FTEMP->PNextHeapNode->Size=(uint32_t)NTEMP-(uint32_t)(FTEMP->PNextHeapNode)-sizeof (HeapNode);
	}
	else if (FTEMP)
	{
		for (;TEMP->PNextHeapNode;TEMP=TEMP->PNextHeapNode);
		FTEMP->PNextHeapNode->PNextHeapNode=NULL;
		FTEMP->PNextHeapNode->USEState=NOTUSED;
		FTEMP->PNextHeapNode->Size=(uint32_t)TEMP+TEMP->Size-(uint32_t)(FTEMP->PNextHeapNode);	
	}
	else if (NTEMP)
	{
		for (;TEMP->PFrontHeapNode;TEMP=TEMP->PFrontHeapNode);
		TEMP->PNextHeapNode=NTEMP;
		TEMP->USEState=NOTUSED;
		TEMP->Size=(uint32_t)NTEMP-(uint32_t)TEMP-sizeof (HeapNode);
		NTEMP->PFrontHeapNode=TEMP;
	}
	else
	{
		for (;TEMP->PFrontHeapNode;TEMP=TEMP->PFrontHeapNode);
		HTEMP=(HeapList*)((uint32_t)TEMP-sizeof (HeapList));
		if (HTEMP->HeapID & 0x80)//���ID���׶�
		{
			TEMP->PFrontHeapNode=NULL;
			TEMP->PNextHeapNode=NULL;
			TEMP->USEState=NOTUSED;
			TEMP->Size=HTEMP->Size-sizeof (HeapNode);
		}
		else                       //���ID�����׶�
		{
			for (FHTEMP=_heaplist;FHTEMP->PNextHapeList!=HTEMP && FHTEMP->PNextHapeList!=NULL;FHTEMP=FHTEMP->PNextHapeList);
			FHTEMP->PNextHapeList=HTEMP->PNextHapeList;
			free (HTEMP);  //�ͷű��
		}
	}
}
void SYS_CallHEAP (uint32_t Size,uint8_t ID)
{
	HeapList* TEMP;
	for (TEMP=_heaplist;TEMP->PNextHapeList!=NULL;TEMP=TEMP->PNextHapeList);
	SYS_MemoryHEAPCall (Size,ID | 0x80,TEMP);
}
void SYS_FreeHEAP (uint8_t ID)
{
	HeapList* TEMP;
	for (TEMP=_heaplist;TEMP->HeapID!=(0x80 | ID) && TEMP->PNextHapeList;TEMP=TEMP->PNextHapeList);
	while (1)
	{
		if (TEMP->HeapID==(0x80 | ID) || TEMP->HeapID==ID)
		{
			free (TEMP);
		}
		else return;
		if (TEMP->PNextHapeList==NULL) return;
		else TEMP=TEMP->PNextHapeList;
	}
}

