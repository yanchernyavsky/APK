//724402-2  L4  Chernyavsky Y.A.  Podsistema prerivaniy
#include <dos.h> 
#include <stdio.h>
#include <conio.h> 
#include <stdlib.h>

char far *v = (char far *)0xB8000000;		   // Указатель на память видеобуфера
int quitFlag = 0, timerCounter = 0,			   // Флаг выхода, счётчик времени
colorId = 0, posX = 0, posY = 11;			   // ID цвета, координаты x и y
void interrupt far(*oldInt9)(void);			   // Указатель на обработчик прерывани Int9
void interrupt newInt9(void);  				   // Функция обработки прерывани Int9
void interrupt far(*oldInt8)(void);			   // Указатель на обработчик прерывани Int8
void interrupt newInt8(void);				   // Функция обработки прерывани Int8
void interrupt far(*oldInt0)(void);			   // Указатель на обработчик прерывания Int0
void interrupt newInt0(void);  				   // Функция обработки прерывани Int0
void print(char*, int);  	 	 			   // Функция вывода в консоль через видеопамять
void printTo(char*, int, int);				   // Функция вывода в консоль через видеопамять в произвольное место
void printToWithColor(char*, int, int, int);   // Функция вывода в консоль через видеопамять в произвольное место с заданным цветом
void setColorFor(int, int, int);			   // Функции установки цвета на ограниченную область
void intToHex(int, char*); 					   // Функция конвертации в 16-ричную систему счисления
void intToBin(int, char*); 	  	 			   // Функция конвертации в 2-ичную систему счисления
void movePointer(void);  					   // Функция смещения указателя на видеопамять
void setPointer(int, int); 					   // Функция установки указателя на видеопамять
void scroll(void); 	 	 	 			       // Функция прокрутки экрана
void clearWholeScreen(void);  				   // Функция очистки всего экрана
void printRegisters(void); 	 			 	   // Функция вывода на экран регистров
void fillWithZeroes(int, char*); 			   // Функция дополнения десятичного числа нулями
void loading(void); 	 	 	 	 		   // Функция анимации загрузки
void clearScreen(void);  	 	 			   // Функция очистки экрана от скан-кодов
void printDivByZero(void); 	 	 			   // Функция вывода ошибки при делении на 0


void intToBin(int scanCode, char *str)// Функция конвертации в 2-ичную систему счисления
{
	int rest, i, pos, value;
	pos = 8;
	value = scanCode;
	while (value > 1)
	{
		rest = value % 2;
		pos--;
		str[pos] = rest + '0';
		value = (value - rest) / 2;
	}
	pos--;
	str[pos] = value + '0';
	for (i = pos - 1; i > -1; i--)
		str[i] = '0'; str[8] = '\0';
}
void intToHex(int scanCode, char *str)// Функция конвертации в 16-ричную систему счисления
{
	char letters[6] = { 'A','B','C','D','E','F' };
	int rest, i, pos, value;
	pos = 2;
	value = scanCode;
	while (value > 15)
	{
		rest = value % 16;
		pos--;
		if (rest > 9)
		   str[pos] = letters[rest - 10];
		else
		   str[pos] = rest + '0';
		value = (value - rest) / 16;
	}
	pos--;
	if (value > 9)
	   str[pos] = letters[value - 10];
	else
	   str[pos] = value + '0';
	for (i = pos - 1; i > -1; i--)
		str[i] = '0';
	str[2] = '\0';
}
void printRegisters()//Функция вывода на экран регистров
{
	int reg;
	char strH[3], strB[9];
	reg = inportb(0x21);
	intToHex(reg, strH);
	intToBin(reg, strB);
	printToWithColor("MASTER:", 10, 0, 6);
	printToWithColor("Register mask (0x21):", 10, 0, 7);
	printToWithColor(strB, 10, 25, 7);
	printToWithColor("(", 10, 34, 7);
	printToWithColor(strH, 10, 35, 7);
	printToWithColor(")", 10, 37, 7);
	outportb(0x20, 0x0A);
	reg = inportb(0x20);
	intToHex(reg, strH);
	intToBin(reg, strB);
	printToWithColor("Register request (0x20):", 10, 0, 8);
	printToWithColor(strB, 10, 25, 8);
	printToWithColor("(", 10, 34, 8);
	printToWithColor(strH, 10, 35, 8);
	printToWithColor(")", 10, 37, 8);
	outportb(0x20, 0x0B);
	reg = inportb(0x20);
	reg = inportb(0x21);
	intToHex(reg, strH);
	intToBin(reg, strB);
	printToWithColor("Register service (0x20):", 10, 0, 9);
	printToWithColor(strB, 10, 25, 9);
	printToWithColor("(", 10, 34, 9);
	printToWithColor(strH, 10, 35, 9);
	printToWithColor(")", 10, 37, 9);
	reg = inportb(0xA1);
	intToHex(reg, strH);
	intToBin(reg, strB);
	printToWithColor("SLAVE :", 10, 42, 6);
	printToWithColor("Register mask (0xA1):", 10, 42, 7);
	printToWithColor(strB, 10, 67, 7);
	printToWithColor("(", 10, 76, 7);
	printToWithColor(strH, 10, 77, 7);
	printToWithColor(")", 10, 79, 7);
	outportb(0xA0, 0x0A);
	reg = inportb(0xA0);
	intToHex(reg, strH);
	intToBin(reg, strB);
	printToWithColor("Register request (0xA0):", 10, 42, 8);
	printToWithColor(strB, 10, 67, 8);
	printToWithColor("(", 10, 76, 8);
	printToWithColor(strH, 10, 77, 8);
	printToWithColor(")", 10, 79, 8);
	outportb(0xA0, 0x0B);
	reg = inportb(0xA0);
	intToHex(reg, strH);
	intToBin(reg, strB);
	printToWithColor("Register service (0xA0):", 10, 42, 9);
	printToWithColor(strB, 10, 67, 9);
	printToWithColor("(", 10, 76, 9);
	printToWithColor(strH, 10, 77, 9);
	printToWithColor(")", 10, 79, 9);
}
void clearScreen()//Функция очистки экрана от скан-кодов
{
	int i, j;
	for (i = 0; i < 80; i++)
	{
		for (j = 24; j > 6; j--)
		{
			v = (char far*)0xB8000000 + j * 160 + i * 2;
			*v = ' ';
		}
	}
	setPointer(0, 11);
}
void clearWholeScreen()//Функция очистки всего экрана
{
	int i, j;  v = (char far *)0xB8000000;
	for (i = 0; i < 25; i++)
	{
		for (j = 0; j < 80; j++)
		{
			v = (char far *)0xB8000000 + i * 160 + j * 2;
			*v = ' ';
			v++;
			*v = 0xf;
		}
	}
	setPointer(posX, posY);
}
void scroll()// Функция прокрутки экрана
{
	int i, j;
	char tmp;
	for (i = 7; i < 24; i++)
	{
		for (j = 0; j < 80; j++)
		{
			v = (char far *)0xB8000000 + (i + 1) * 160 + j * 2;
			tmp = *v;
			v = (char far *)0xB8000000 + i * 160 + j * 2;
			*v = tmp;
		}
	}
	for (j = 0; j < 80; j = j + 1)
	{
		v = (char far *)0xB8000000 + 24 * 160 + j * 2;
		*v = ' ';
	}
}
void setPointer(int _posX, int _posY)// Функция установки указателя на видеопамять
{
	v = (char far *)0xB8000000 + _posY * 160 + _posX * 2;
	posX = _posX;
	posY = _posY;
}
void movePointer()// Функция смещения указателя на видеопамять
{
	posX++;
	if (posX >= 80)
	{
		posX = 0;
		posY = posY + 1;
	}
	if (posY >= 25)
	{
		posY = 24;
		scroll();
	}
	setPointer(posX, posY);
}
void setColorFor(int topBorder, int downBorder, int color)// Функции установки цвета на ограниченную область
{
	int i, j;
	for (i = topBorder; i <= downBorder; i++)
	{
		for (j = 0; j < 80; j++)
		{
			v = (char far *)0xB8000000 + i * 160 + j * 2;
			v++;
			*v = color;
			v--;
		}
	}
	setPointer(posX, posY);
}
void printTo(char *str, int posX_, int posY_)// Функция вывода в консоль через видеопамять в произвольное место
{
	int i;
	v = (char far *)0xb8000000 + posY_ * 160 + posX_ * 2;
	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		v += 2;
	}
	setPointer(posX, posY);
}
void printToWithColor(char *str, int color, int posX_, int posY_)// Функция вывода в консоль через видеопамять в произвольное место с заданным цветом
{
	int i;
	v = (char far *)0xb8000000 + posY_ * 160 + posX_ * 2;
	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		v++;
		*v = color;
		v--;
		v += 2;
	}
	setPointer(posX, posY);
}
void print(char *str, int color)// Функция вывода в консоль через видеопамять
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		v++;
		*v = color;
		v--;
		movePointer();
	}
}
void fillWithZeroes(int value_, char *str)// Функция дополнения десятичного числа нулями
{
	int rest, i, pos = 3, value = value_;
	while (value >= 10)
	{
		rest = value % 10;
		pos--;
		str[pos] = rest + '0';
		value = (value - rest) / 10;
	}
	pos--;
	str[pos] = value + '0';
	for (i = pos - 1; i >= 0; i = i - 1)
	{
		str[i] = '0';
	}
	str[3] = '\0';
}
void interrupt newInt0()// Функция обработки прерывани Int0
{
	disable();
	printDivByZero();
	printRegisters();
	outportb(0x20, 0x20);
	enable();
	delay(1000);
	setvect(9, oldInt9);
	setvect(8, oldInt8);
	setvect(0, oldInt0);
    quitFlag = 1;
}
void interrupt newInt8()// Функция обработки прерывани Int8
{
	char str[3];
	int value;
	disable();
	value = timerCounter / 18;
	timerCounter = timerCounter + 1;
	printToWithColor("Timer", 0x0B, 70, 0);
	fillWithZeroes(value, str);
	printToWithColor(str, 0x0B, 76, 0);
	printRegisters();
	outportb(0x20, 0x20);
	enable();
}
void interrupt newInt9()// Функция обработки прерывани Int9
{
	char str[3];
	int scanCode, zero = 0;
	int colors[6] = { 0x0F,0x0E,0x0D,0x0C,0x0B,0x0A };
	disable();
	scanCode = inportb(0x60);
	if (scanCode == 0x05)
		quitFlag = 1;
	if (scanCode == 0x04)
	{
		printDivByZero();
		zero = zero / zero;
	}
	if (scanCode == 0x02)
		clearScreen(); 
	if (scanCode == 0x03)
	{
		colorId++;
		if (colorId >= 6)
			colorId = 0;
		setColorFor(7, 25, colors[colorId]);
	}
	intToHex(scanCode, str); 
	print(str, colors[colorId]);
	print(" ", 0x0F); 
	printRegisters(); 
	outportb(0x20, 0x20);
	enable();
} 
int main()
{
	clearWholeScreen();// Функция очистки всего экрана
	printToWithColor("724402-2  L4  Chernyavsky Y.A.  Podsistema prerivaniy", 0x05, 0, 0);// Функция вывода в консоль через видеопамять в произвольное место с заданным цветом
	printToWithColor("1. Clear screen", 0x0E, 0, 1);									
	printToWithColor("2. Change scancode color", 0x0E, 0, 2);
	printToWithColor("3. Divide by zero", 0x0E, 0, 3);
	printToWithColor("4. Exit", 0x0E, 0, 4);
	setColorFor(5, 8, 0x04); // Функции установки цвета на ограниченную область
	printRegisters();// Функция вывода на экран регистров
	oldInt9 = getvect(9);
	setvect(9, newInt9);
	oldInt8 = getvect(8); 
	setvect(8, newInt8);  	
	oldInt0 = getvect(0);  	
	setvect(0, newInt0);  	
	while (quitFlag != 1) 
	{
		loading();// Функция анимации загрузки
	}
	setvect(9, oldInt9);
	setvect(8, oldInt8);
	setvect(0, oldInt0);  	
	clearWholeScreen();// Функция очистки всего экрана
	return;
}
void loading()// Функция анимации загрузки
{
	int i;
	int colors[6] = { 0x02,0x03,0x04,0x05,0x06,0x07 };
	for (i = 0; i < 6; i++)
	{
		printToWithColor("|", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		printToWithColor("/", colors[i], 76, 1);
		delay(100);
		if (quitFlag)
			return;
		printToWithColor("-", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		printToWithColor("\\", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		printToWithColor("|", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		printToWithColor("/", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		printToWithColor("-", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		printToWithColor("\\", colors[i], 76, 1);
		if (quitFlag)
			return;
		delay(100);
		if (i == 5)
			i = 0;
	}
}
void printDivByZero()// Функция вывода ошибки при делении на 0
{
	short value;
	char port61;
	clearScreen();
	printToWithColor("DIVISION BY ZERO", 0x04, 0, 13);
	value = 1193180 / 600;  
	outp(0x42, (char)value); 
	outp(0x42, (char)(value >> 8)); 
	port61 = inp(0x61);  	
	port61 |= 3;  
	outp(0x61, port61);
	delay(500);
	port61 &= 0xFFFC; 
	outp(0x61, port61);
}