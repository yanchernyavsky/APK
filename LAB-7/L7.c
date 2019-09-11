//724402-2 Chernyavsky Y.A.
//L7 Rabota Videopodsistems
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#define VIDEO (char far*)0xb8000000

void print(char*str);
void printInfo(void);
void CGA(void);
void EGA(void);
void myPutsXY(char*, int, int);	//вывод строки в конкретное место
void myclear(int, int, int);	//очистка области экрана (начальная строка, начальный столбец, длина)
void top(void); 				//вывод шапки
volatile int Column = 0;
volatile int Line = 0;

int main(void)
{
	int flag = 1;
	char ch = 0;
	while (flag) 
	{
		myclear(0, 0, 2000);
		top();
		ch = getch();
		switch (ch)
		{
		case '1':
			CGA();
			break;
		case '2': 
			EGA();
			break;
		case '0':
			flag = 0;
			break;
		}
	}
	myclear(0, 0, 2000);
	return 0;
}

void EGA(void) { //режим EGA
	union  REGS inregs, outregs;
	unsigned char j, color = 1;
	inregs.h.ah = 0x00;	//Выбор режима
	inregs.h.al = 0x0e;	//установка режима EGA
	int86(0x10, &inregs, &outregs);
	inregs.h.ah = 0x0B;
	inregs.h.bh = 0x00; //задание цвета фона
	inregs.h.bl = 0x07;	//номер устанавливаемого цвета фона
	int86(0x10, &inregs, &outregs);
	while (1) {//отрисовка линии
		if (kbhit() && getch() == 48)
			break;
		inregs.h.ah = 0x0C;
		inregs.h.al = color++;//задание цвета линии
		int86(0x10, &inregs, &outregs);
		for (j = 0; j < 200; j++) {
			inregs.h.ah = 0x0c;  // функция отрисовки пикселя
			if (color > 15) color = 1;
			if (color == 7) color = 8;
			inregs.h.al = color; // задание цвета линии
			inregs.x.cx = 0x30 + j; // x координата
			inregs.x.dx = 0xaa - j / 3.3; // y координата
			int86(0x10, &inregs, &outregs);
			inregs.x.cx = 0x30 + j;  // x координата
			inregs.x.dx = 0x30 + j / 3.3; // y координата
			int86(0x10, &inregs, &outregs);
			if (j < 123)
			{
				inregs.x.cx = 0x30; // x координата
				inregs.x.dx = 0x30 + j; // y координата
				int86(0x10, &inregs, &outregs);
			}
			if (j < 100)
			{
				inregs.x.cx = 0x30 + 200; // x координата
				inregs.x.dx = 0x30 + j + 10; // y координата
				int86(0x10, &inregs, &outregs);
			}
			color++;
		}
		
		delay(300);
	}
	inregs.h.ah = 0x0;
	inregs.h.al = 0x3;
	int86(0x10, &inregs, &outregs);

}

void CGA(void) { //режим CGA
	union REGS inregs, outregs;
	unsigned char i, j, color;
	color = 1;// номер цвета
	inregs.h.ah = 0x00;
	inregs.h.al = 0x04;//установка режима CGA
	int86(0x10, &inregs, &outregs);
	inregs.h.ah = 0x0B;	//выбор палитры
	inregs.h.bh = 0x00; //задание цвета фона
	inregs.h.bl = 0x00;	//номер устанавливаемого цвета фона(bl,gr,r,br)
	int86(0x10, &inregs, &outregs);
		
	while (1) {//отрисовка прямоугольника
		if (kbhit() && getch() == '0')
			break;
		inregs.h.ah = 0x0c; //вывод пикселя
		inregs.h.al = color;//задание цвета линии
		int86(0x10, &inregs, &outregs);
		for (i = 0; i < 100; i++) {
			for (j = 0; j < 100; j++) {
				inregs.x.cx = 0x70 + i;//х координата
				inregs.x.dx = 0x30 + j;// у координата
				int86(0x10, &inregs, &outregs);
			}
		}
		delay(800);
		color++;
		if(color > 3) color = 1;
	}
	inregs.h.ah = 0x00;
	inregs.h.al = 0x03;//переход в текстовый режим
	int86(0x10, &inregs, &outregs);
}

void myPutsXY(char *str, int row, int col)
{
	int i;
	char far* v = (char far *)0xb8000000 + row * 160 + col * 2;
	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		v += 2;
	}
}

void myclear(int row, int col, int len)
{
	int i;
	char far* v = (char far*)0xb8000000 + row * 160 + col * 2;
	for (i = 0; i < len; i++)
	{
		*v = ' ';
		v += 2;
	}
}


void top(void)
{
	char* str0 = "724402-2 Chernyavsky Y.A.";
	char* str1 = "L7 Rabota Videopodsistem";
	char* str2 = "1 - CGA (Color Graphics Adapter) mod 4 |320x200|";
	char* str3 = "2 - EGA (Enhanced Graphics Adapter) mod E |640x200|";
	char* str4 = "0 - Exit";
	myPutsXY(str0, 0, 0);
	myPutsXY(str1, 1, 0);
	myPutsXY(str2, 2, 0);
	myPutsXY(str3, 3, 0);
	myPutsXY(str4, 4, 0);
}