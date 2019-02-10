
#include "REG52.h"
#include "oled.h"

#define uchar unsigned char
#define uint  unsigned int
#include <intrins.h>
sbit 	DQ = P3^6;    //���ݿ�define interface
/***********************DS1302�˿ڶ���*******************************************/
sbit clock_rst=P3^5;  //1302��λ�ܽŶ���
sbit clock_dat=P1^3;  //1302I/O�ܽŶ���
sbit clock_clk=P1^2;  //1302ʱ�ӹܽŶ���  
/***************�Ĵ������壨���ڼ�¼1302IO������������*************************/
sbit a0=ACC^0;
sbit a1=ACC^1;
sbit a2=ACC^2;
sbit a3=ACC^3;
sbit a4=ACC^4;
sbit a5=ACC^5;
sbit a6=ACC^6;
sbit a7=ACC^7;

/**********************1302ȫ�ֱ�������******************************************/
uchar sec,min,hour,day,month,year1,year2=20;//�룬�֣�ʱ���գ��£����ڣ���
/************************�������˿ڶ���******************************************/
sbit beep=P3^7;
/***********************��������*************************************************/
sbit key1=P1^0;
sbit key2=P1^1;
sbit key3=P3^4;
uchar status=0;//��������ѡ���壨��0����1����2��ʱ3����4 ��



uint tmp;                           //�¶�ֵ variable of temperature
uint Temp_Buffer = 0;
uint t1;
uint  t2;
uint   t3;
float tt;
 void delay2(unsigned char i)           //��ʱ����
{
	while(--i); 
}
 /**********************��ʱ����*************************************************/
void delay(uint t)  
{
 uint b;
 for(;t;t--)
  for(b=0;b<121;b++); 
}


void Init_Ds18b20(void)     //DS18B20��ʼ��send reset and initialization command
{
	DQ = 1;                     //DQ��λ,��ҪҲ���С�
	delay2(1);                  //������ʱ
	DQ = 0;                    //��Ƭ����������
	delay2(250);                //��ȷ��ʱ��ά������480us
	DQ = 1;                    //�ͷ����ߣ�������������
	delay2(100);                //�˴���ʱ���㹻,ȷ������DS18B20�����������塣
}
uchar Read_One_Byte()       //��ȡһ���ֽڵ�����read a byte date
                            //������ʱ,�������ֽڵ������Чλ�ȴ������Ƴ�
{
	uchar i   = 0;
	uchar dat = 0;
	for(i=8;i>0;i--)
	{
	   DQ = 0;                 //���������ͣ�Ҫ��1us֮���ͷ�����
	                           //��Ƭ��Ҫ�ڴ��½��غ��15us�ڶ����ݲŻ���Ч��
	   _nop_();                 //����ά����1us,��ʾ��ʱ��ʼ
	   dat >>= 1;               //�ô������϶�����λ���ݣ����δӸ�λ�ƶ�����λ��
	   DQ = 1;                  //�ͷ����ߣ��˺�DS18B20���������,�����ݴ��䵽������
	   delay2(1);                 //��ʱ7us,�˴������Ƽ��Ķ�ʱ��ͼ�������ѿ���������ʱ��ŵ���ʱ����15us�ڵ���󲿷�
	   if(DQ)                   //���������в���
	   {
	    dat |= 0x80;    //����Ϊ1, DQΪ1,�ǾͰ�dat�����λ��1;��Ϊ0,������,����0
	   }        
	   delay2(10);               //����ʱ�����٣�ȷ����ʱ��ĳ���60us��
	}
	return (dat);
}
void Write_One_Byte(uchar dat)
{
	uchar i = 0;
	for(i=8;i>0;i--)
	{
	   DQ = 0;                        //��������
	   _nop_();                       //����ά����1us,��ʾдʱ��(����д0ʱ���д1ʱ��)��ʼ
	   DQ = dat&0x01;                 //���ֽڵ����λ��ʼ����
	                                 //ָ��dat�����λ���������,�������������ߺ��15us��,
	                                 //��Ϊ15us��DS18B20������߲�����
	   delay2(10);                     //������дʱ���������60us
	   DQ = 1;                        //д���,�����ͷ�����,
	   dat >>= 1;
	   delay2(1);
	}
}
uint Get_Tmp()                   //��ȡ�¶�get the temperature
{
	
    uchar a=0,b=0;
	
	Init_Ds18b20();                //��ʼ��
	Write_One_Byte(0xcc);          //����ROMָ��
	Write_One_Byte(0x44);          //�¶�ת��ָ��
	Init_Ds18b20();                 //��ʼ��
	Write_One_Byte(0xcc);          //����ROMָ��
	Write_One_Byte(0xbe);          //���ݴ���ָ��
	a = Read_One_Byte();           //��ȡ���ĵ�һ���ֽ�Ϊ�¶�LSB
	b = Read_One_Byte();           //��ȡ���ĵ�һ���ֽ�Ϊ�¶�MSB
	tmp = b;                      //�ȰѸ߰�λ��Ч���ݸ���temp
	tmp <<= 8;                    //������8λ���ݴ�temp�Ͱ�λ�Ƶ��߰�λ
	tmp = tmp|a;                //���ֽںϳ�һ�����ͱ���
	tt = tmp*0.0625;           //�õ���ʵʮ�����¶�ֵ��DS18B20���Ծ�ȷ��0.0625��
    tmp =(uint)( tt*10+0.5);   //�Ŵ�ʮ����Ŀ���ǽ�С������һλҲת��Ϊ����ʾ����                               
	return (tmp);
}
 void Display_Temp()
{
	
	Temp_Buffer = Get_Tmp() ;  	   //��ȡDS18B20��ֵ
	t1=(Temp_Buffer%1000/100);	   //��ʾ�¶�ʮλֵ
	t2=(Temp_Buffer%100/10);	   //��ʾ�¶ȸ�λֵ
						   
	t3=(Temp_Buffer%10);		   //��ʾ�¶�ʮ��λֵ
						   
		
}
 
/*******************************************************************************/
/*************************DS1302�Ӻ���******************************************/
/************************1302д��������*****************************************/
void write_clock(uchar dat)
{
 ACC=dat;
 clock_dat=a0; clock_clk=1; clock_clk=0; //�����������ر仯
 clock_dat=a1; clock_clk=1; clock_clk=0; //�½���д��1302
 clock_dat=a2; clock_clk=1; clock_clk=0;
 clock_dat=a3; clock_clk=1; clock_clk=0;
 clock_dat=a4; clock_clk=1; clock_clk=0;
 clock_dat=a5; clock_clk=1; clock_clk=0;
 clock_dat=a6; clock_clk=1; clock_clk=0;
 clock_dat=a7; clock_clk=1; clock_clk=0;
}
/************************1302����������*****************************************/
uchar read_clock()
{
 clock_dat=1;   //�����1
 a0=clock_dat;clock_clk=1; clock_clk=0;  //λ0Ϊ1����1302
 a1=clock_dat;clock_clk=1; clock_clk=0; //�½��ض�����Ч
 a2=clock_dat;clock_clk=1; clock_clk=0; 
 a3=clock_dat;clock_clk=1; clock_clk=0;
 a4=clock_dat;clock_clk=1; clock_clk=0; 
 a5=clock_dat;clock_clk=1; clock_clk=0; 
 a6=clock_dat;clock_clk=1; clock_clk=0; 
 a7=clock_dat;clock_clk=1; clock_clk=0;
 return(ACC);
}
/***************************д������1302****************************************/
void write_dat_clock(uchar add,uchar dat)
{
 clock_clk=0; 
 clock_rst=0; //ʱ������Ϊ��ʱ
 clock_rst=1; //��λ�˲����ø�
 write_clock(add); //д���ַ�������ֽڣ�
 write_clock(dat); //д������
 clock_rst=0; //��ֹ���ݴ���
 clock_clk=1; 
}
/***************************��1302��������**************************************/
uchar read_dat_clock(uchar add)
{
 uchar dat=0; //�������ݴ������
 clock_clk=0;
 clock_rst=0;
 clock_rst=1;
 write_clock(add); //д��Ҫ�����ݵĵ�ַ
 dat=read_clock(); //��������
 clock_rst=0; //��ֹ���ݴ���
 clock_clk=1;
 return dat;  //��������
}
/**************************���ó�ʼʱ�亯��*************************************/
void set_time()
{
 read_dat_clock(0x81);    //��ȡ��
 if(sec==0x80)	  //secΪ���������ֵ��������λΪ1�����ʼ��
 write_dat_clock(0x8e,0x00);  //����д����
 write_dat_clock(0x8c,0x13);  //��
 write_dat_clock(0x8a,0x03);  //����
 write_dat_clock(0x88,0x10);  //��
 write_dat_clock(0x86,0x14);  //��
 write_dat_clock(0x84,0x21);  //Сʱ
 write_dat_clock(0x82,0x56);  //��
 write_dat_clock(0x80,0x00);  //�룬����������
 write_dat_clock(0x90,0xa5);  //���
 write_dat_clock(0x8e,0x80);  //��ֹд����
}

void get_time()
{
 sec=read_dat_clock(0x81);    //��ȡ��
 sec=(sec>>4)*10+(sec&0x0f);  //�������ת��
 delay(1);
 min=read_dat_clock(0x83);    //��ȡ����
 min=(min>>4)*10+(min&0x0f);  //�ֶ�����ת��
 delay(1);
 hour=read_dat_clock(0x85);   //��ȡСʱ
 hour=(hour>>4)*10+(hour&0x0f);//Сʱ������ת��
 delay(1);
 day=read_dat_clock(0x87);     //��ȡ����
 day=(day>>4)*10+(day&0x0f);   //���ڶ�����ת��
 delay(1);
 month=read_dat_clock(0x89);   //��ȡ�·�
 month=(month>>4)*10+(month&0x0f);//�¶�����ת��
 delay(1);
 //week=read_dat_clock(0x8b);    //��ȡ����
 //week=(week>>4)*10+(week&0x0f);//���ڶ�����ת��
 delay(1);
 year1=read_dat_clock(0x8d);     //��ȡ���
 year1=(year1>>4)*10+(year1&0x0f); //��ݶ�����ת��
}          
 
/******************************************************************************/
/*****************************�������ܺ���*************************************/
void key_1()     //ѡ���������
{
 key1=1;
 if(key1==0)
  delay(10);																												
 {
  if(key1==0)
  {
   if(++status>5)status=0; //1������ı仯��2�����µı仯,3�����յı仯
  }       //4����ʱ�ı仯��5���Ʒֵı仯
  while(key1==0);
 }
}
/******************************************************************************/
void key_2()       //�Ӻ��� 
{
 key2=1;
 if(key2==0)
 {
  delay(10);
  if(key2==0)
  {
   switch(status)
   {
    case 1:if(++year1>99)year1=0;break;
    case 2:if(++month>12)month=1;break;
    case 3:if(++day>31)day=1;break;
    case 4:if(++hour>=24)hour=0;break;
    case 5:if(++min>=60)min=0;break;
   }
  }
while(key2==0);
 }
}
/*****************************************************************************/
void key_3()//������ 
{
  key3=1;
 if(key3==0)
 {
  delay(10);
  if(key3==0)
  {
   switch(status)
   {
    case 1:if((year1--)<=0)year1=99;break;
    case 2:if((month--)<=1)month=12;break;
    case 3:if((day--)<=1)day=31;break;
    case 4:if((hour--)<=0)hour=23;break;
    case 5:if((min--)<=0)min=59;break;
   }
  }
  while(key3==0);
 }
}

/*****************************************************************************/
void key_time()    //��������ʱ��Ĵ���
{
 if(status>0)
 {
 write_dat_clock(0x8e,0x00);  //����д����
 write_dat_clock(0x8c,(year1/10)*16+year1%10);  //��
 //write_dat_clock(0x8a,(week/10)*16+week%10);  //����
 write_dat_clock(0x88,(month/10)*16+month%10);  //��
 write_dat_clock(0x86,(day/10)*16+day%10);  //��
 write_dat_clock(0x84,(hour/10)*16+hour%10);  //Сʱ
 write_dat_clock(0x82,(min/10)*16+min%10);  //��
 write_dat_clock(0x8e,0x80);  //��ֹд����
 }
} 
 
void  main()
 {	   
	    OLED_Init();			//��ʼ��OLED  
		OLED_Clear(); 
		Init_Ds18b20(); 
		Get_Tmp();
		 delay(10);
		 set_time();
		 
	while(1) 
	{		
		
		
		Display_Temp();
		//OLED_Clear();
		OLED_ShowCHinese(0,0,0);//��
		OLED_ShowCHinese(18,0,1);//��
		OLED_ShowCHinese(36,0,2);//��
		OLED_ShowCHinese(54,0,3);//��
		 
		
		OLED_ShowCHinese(0,2,7);//ʵ
	    OLED_ShowCHinese(18,2,8);//��
		OLED_ShowCHinese(36,2,9);//��
		OLED_ShowCHinese(54,2,10);//��
		 OLED_ShowCHinese(36,4,11);//��
		 OLED_ShowCHinese(74,4,12);//��
		 OLED_ShowCHinese(110,4,13);//��
		  get_time();
		 OLED_ShowCHinese(28,6,14);//ʱ
		 OLED_ShowCHinese(66,6,15);//��
		 OLED_ShowCHinese(102,6,16);//��
		 
		 OLED_ShowString(120,2,"C");  
		 OLED_ShowNum(84,2,t1,1,16);//�¶�ʮλ	
		 OLED_ShowNum(92,2,t2,1,16);//��ʾ��λ	
		 OLED_ShowString(100,2,".");  
		 OLED_ShowNum(108,2,t3,1,16);//С�����һλ
	     OLED_ShowNum(0,4,year2,2,16);	
		 OLED_ShowNum(18,4,year1,2,16);	
		 OLED_ShowNum(54,4,month,2,16);	
		 OLED_ShowNum(91,4,day,2,16);	
		 OLED_ShowNum(10,6,hour,2,16);	
		 OLED_ShowNum(46,6,min,2,16);	
		 OLED_ShowNum(84,6,sec,2,16);
		
		key_1();
        key_2();
        key_3();
        key_time();
	}	  
	
}

