
// 624401 Mikhalevich Liza
// LR 4
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>


volatile int x = 0;     //  ���������� ���������� ������ � ������� 
volatile int y = 5;     //  ��� ������ �� �����
volatile int count = 0; //  ������� �������� �� ������
volatile int tik = 0;   //  ������� ������ ���������� �������
volatile int sec = 0;   //  ���������� ������
volatile int mscount = 0; //������� ��� �������� �������
volatile int alt = 0;

void init(void);        //������� ������������� 
void initTimer(void);   //������� ������������� �������
void printToVideoMemory1(char *str, int line, int column, int color); //������ ������ 
void printToVideoMemory(char *str);  //������� ������
void CleanScreen(int line, int counts, int column);  //������� ������� ������
//1
void interrupt newInt9(void);     //������� ��������� ���������� ����������
void interrupt(*oldInt9)(void);   //��������� �� ���������� ���������� ����������
//2
void interrupt TimerHandler(void); //������� ��������� ���������� ���������� �������
void interrupt(*oldTimerHandler)(void); //��������� �� ������� ��������� ���������� ���������� �������
//3
void interrupt far newInt70handler(void); //������� ��������� ���������� RTC
void interrupt far(*oldInt70h)(void); //��������� �� ������� ��������� ���������� RTC

void del_f(void);   //������� ������� �� 0

void interrupt delto0(void);   //������� ��������� ���������� ������� �� 0
void interrupt(*olddelto0)(void);  //��������� �� ������� ��������� ���������� ������� �� 0

void interrupt prtsc(void);  //������� ��������� ���������� ��� ������� �� ������� prt sc
void interrupt(*oldptrsc)(void);  //��������� �� ������� ��������� ���������� ��� ������� �� ������� prt sc

void interrupt ctrlbr(void);
void interrupt(*oldctrlbr)(void);

void delaytime(unsigned long delays);  //������� �������� �������
void byteToString(unsigned char temp, char *str);  //������� �������� �� int � char
volatile int quitFlag = 0;  //���� ������

void status(void);     //������� ������ ��������� ���������
void scroll(void);      //������� �������� �� ����� ������


void interrupt ctrlbr()
{
	status();
	disable();

	printToVideoMemory1("<Ctrl+Break>", 2, 40, 2);
	sec = 0;

	oldctrlbr();
	outp(0x20, 0x20);  //����� ����������� ����������
	enable();
}


void main()
{
	
	clrscr();
	init();    

	while (!quitFlag);

	setvect(9, oldInt9);             //   ���������� ��������� �� ������ �����������
	setvect(0x1c, oldTimerHandler);  
	setvect(0x0, olddelto0);
	setvect(0x05, oldptrsc);
	clrscr();
	return;
}

void init() 
{       
	char str[] = { "Mikhalevich Liza 624401  | interrupt system " };
	char str0[] = { "Esc - to exit, '1' - delay timer,  '2'- clear screen, '3' - delenie na 0" };

	oldInt9 = getvect(9);    //���������� ����������
	setvect(9, newInt9);

	oldptrsc = getvect(0x05);  //���������� prt sc
	setvect(0x05, prtsc);
	
	oldctrlbr = getvect(0x1B);
	setvect(0x05, ctrlbr);

	printToVideoMemory1(str, 0, 0, 10);
	printToVideoMemory1(str0, 1, 0, 10);

	printToVideoMemory1("Time from start:", 2, 0, 5);
	initTimer();
}

void interrupt newInt9()
{
	char str6[9];
	unsigned char value = 0;
	oldInt9();
	status();    //����� ������� ������ ��������� ���������
	if (count >= 1599)
	{
		scroll();  //����� ������� �������� �����
		y = 24;
		x = 0;
		count = 1519;
	}
	value = inp(0x60);    //��������� �������� �� ����� 60h
	if (value == 0x01) {  //���� Esc , �� ������������� ���� ������
		quitFlag = 1;
		if (alt == 1)
			olddelto0();
	}
	if (value == 0x03)   //���� 2 , �� ���������� ������� ������
	{
		CleanScreen(5, 1599, 0);
		y = 5;
		x = 0;
		count = 0;
	}
	if (value == 0x02)  //���� 1 , �� ���������� �������� �������
		delaytime(5000);
	if (value == 0x04)  //���� 3 , �� ����� ������� ������� �� 0
		del_f();

	itoa(value, str6, 16);
	printToVideoMemory1("keybord", 2, 40, 2);
	printToVideoMemory(str6);
	outp(0x20, 0x20);   //����� ����������� ����������
}


void initTimer() 
{
	disable();
	oldTimerHandler = getvect(0x1c); //���������� �������
	setvect(0x1c, TimerHandler);   //��������� 18,2 ���� � �������
	enable();
}
void interrupt TimerHandler()
{
	char buff[9];
	status();    //����� ������� ������ ��������� ���������
	disable();
	tik++;             //����������� ���������� �����
	if (!(tik % 18))   //���� ������ 18,�� ������ 1 �������
	{
		sec++;
		itoa(sec, buff, 10);
		printToVideoMemory1(buff, 2, 20, 3);
	}
	oldTimerHandler();
	outp(0x20, 0x20);  //����� ����������� ����������
	enable();
}
void del_f()
{
	int i = 0, c = 5;
	disable();
	olddelto0 = getvect(0x0);   //���������� ������� �� 0
	setvect(0x0, delto0);
	enable();
	c = c / i;
}
void interrupt delto0()
{
	alt = 1;
	status();
	disable();
	printToVideoMemory1("was del to 0", 2, 40, 14);
	/*olddelto0();*/
	outp(0x20, 0x20);  //����� ����������� ����������
	enable();
}

void interrupt prtsc() 
{
	int i;
	int column = 0;
	char *str4 = malloc(1600);
	char far* start = (char far*)0xb8000000;
	char far *v;
	int line = 5;
	status();  
	disable();

	for (i = 0; i < x; i++)   //�������� ���������� �����
	{
		v = start + line * 160 + column * 2;
		column++;
		str4[i] = *v;
		v++;
	}
	printToVideoMemory(str4);
	free(str4);
	oldptrsc();
	outp(0x20, 0x20);  //����� ����������� ����������
	enable();
}


void interrupt far newInt70handler() 
{
	//chtenie();
	mscount++;
	outp(0x70, 0x0C);
	inp(0x71);
	outp(0x20,0x20);
	outp(0xA0,0x20);
}

void delaytime(unsigned long delays)
{
	unsigned char value;
	status();  
	disable();
	printToVideoMemory1("Delay 5 seconds", 2, 40, 2);
	oldInt70h = getvect(0x70);   //��������� ������ ���������� 
	setvect(0x70, newInt70handler); //�������� �����
	enable();
	setvect(9, oldInt9);
	setvect(0x1c, oldTimerHandler);
	setvect(0x05, oldptrsc);
	value = inp(0xA1);
	outp(0xA1, value & 0xFE);
	outp(0x70, 0x0B);
	value = inp(0x0B);
	outp(0x70, 0x0B);
	outp(0x71, value & 0x40);
	mscount = 0;
	while (mscount != delays);
	initTimer();
	setvect(9, newInt9);
	setvect(0x05, prtsc);
	//setvect(01B5, ctrlbr);
	CleanScreen(2,22,40);
}

/////////----------------------------------------------------------------------------------------------------------------------------------------------

void printToVideoMemory1(char *str, int line, int column, int color)
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far *v;
	for (i = 0; str[i] != '\0'; i++)
	{
		
		v = start + line * 160 + column * 2;
		column++;
		*v = str[i];
		v++;
		*v = color;
	}
}
void CleanScreen(int line, int counts,int column)
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far *v;
	for (i = 0; i<counts; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		*v = ' ';
		v++;
	}
}
void printToVideoMemory(char *str)  //������� ������ 
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far *v;
	for (i = 0; str[i] != '\0'; i++)
	{
		v = start + y * 160 + x * 2;
		x++;
		*v = str[i];
		v++;
		count++;
	}
	v = start + y * 160 + x * 2;
	x++;
	*v = ' ';
	v++;
	count++;
}
void byteToString(unsigned char temp, char *str)
{
	int i;
	str[8] = 0;
	i = 7;
	while (temp)
	{
		str[i] = '0' + temp % 2;
		temp = temp / 2;
		i--;
	}
	for (; i>-1; i--)
		str[i] = '0';
}
void status()
{
	unsigned char isr_master, isr_slave; // ������� ������������� ����������
	unsigned char irr_master, irr_slave; // ������� �������� �� ����������
	unsigned char imr_master, imr_slave; // ������� ����� ����������

	char buff[9];

	imr_master = inp(0x21);
	imr_slave = inp(0xA1);
	outp(0x20, 0x0A);

	irr_master = inp(0x20);
	outp(0x20, 0x0B);
	isr_master = inp(0x20);
	outp(0xA0, 0x0A);
	irr_slave = inp(0xA0);
	outp(0xA0, 0x0B);
	isr_slave = inp(0xA0);

	printToVideoMemory1("MASTER PIC | ISR: ", 3, 0, 5);
	byteToString(isr_master, buff);
	printToVideoMemory1(buff, 3, 20, 9);

	printToVideoMemory1(" | IRR: ", 3, 30, 5);
	byteToString(irr_master, buff);
	printToVideoMemory1(buff, 3, 40, 9);

	printToVideoMemory1(" | IMR: ", 3, 50, 5);
	byteToString(imr_master, buff);
	printToVideoMemory1(buff, 3, 60, 9);

	printToVideoMemory1("SLAVE PIC | ISR: ", 4, 0, 5);
	byteToString(isr_slave, buff);
	printToVideoMemory1(buff, 4, 20, 9);

	printToVideoMemory1(" | IRR: ", 4, 30, 5);
	byteToString(irr_slave, buff);
	printToVideoMemory1(buff, 4, 40, 9);

	printToVideoMemory1(" | IMR: ", 4, 50, 5);
	byteToString(imr_slave, buff);
	printToVideoMemory1(buff, 4, 60, 9);
}

void scroll()
{
	int i;
	int column = 0;
	char *str4 = malloc(1600);
	char far* start = (char far*)0xb8000000; //��������� ����� ����� ������
	char far *v;
	int line = 6;  //����� ������ , ������ ���������� ����������
	for (i = 0; i < 1520; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		str4[i] = *v;  // ������ ����������� � �����
		v++;
	}
	printToVideoMemory1(str4, line - 1, 0, 2); //����������
	CleanScreen(24, 80,0);  //������� ��������� ������
	free(str4);
}