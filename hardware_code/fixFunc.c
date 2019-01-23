#define MAIN_Fosc		22118400L	//������ʱ��

#include	"STC15Fxxxx.H"
/******************************/

u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};
extern u8 IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;	//���м��̱���
extern u8	KeyHoldCnt;	//�����¼�ʱ
extern u8	KeyCode;	//���û�ʹ�õļ���, 1~16��Ч
extern u8	cnt50ms;


void IO_KeyDelay(void)
{
	u8 i;
	i = 60;
	while(--i)	;
}

void	IO_KeyScan(void)	//50ms call
{
	u8	j;

	j = IO_KeyState1;	//������һ��״̬

	P0 = 0xf0;	//X�ͣ���Y
	IO_KeyDelay();
	IO_KeyState1 = P0 & 0xf0;

	P0 = 0x0f;	//Y�ͣ���X
	IO_KeyDelay();
	IO_KeyState1 |= (P0 & 0x0f);
	IO_KeyState1 ^= 0xff;	//ȡ��
	
	if(j == IO_KeyState1)	//�������ζ����
	{
		j = IO_KeyState;
		IO_KeyState = IO_KeyState1;
		if(IO_KeyState != 0)	//�м�����
		{
			F0 = 0;
			if(j == 0)	F0 = 1;	//��һ�ΰ���
			else if(j == IO_KeyState)
			{
				if(++IO_KeyHoldCnt >= 20)	//1����ؼ�
				{
					IO_KeyHoldCnt = 18;
					F0 = 1;
				}
			}
			if(F0)
			{
				j = T_KeyTable[IO_KeyState >> 4];
				if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
					KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;	//������룬17~32
			}
		}
		else	IO_KeyHoldCnt = 0;
	}
	P0 = 0xff;
}


 #define MAIN_Fosc		22118400L	//������ʱ��

#include	"STC15Fxxxx.H"
