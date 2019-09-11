//724402-2. Chernyavsky Y. L3. Kontroller klaviaturi
#include <stdio.h>
#include <locale.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>

int x = 0;																																									// Глобальные переменные строки и столбца
int y = 10;																																									// для вывода на экран
int count = 0;																																							// Счетчик символов на экране
char header[] = {"724402-2. Chernyavsky Y. L3. Kontroller klaviaturi"};													// Шапка
char menu[] = {"0) Exit \n1) Clean \n2) Blink \n3) Stop blinking "}; // Меню
char regStatus[] = {"Status of registers:"};																								// 'Состояние регистров'

void PrintToPoint(char *str, int line, int column);	//Функция вывода
void ClearScreen(int line, int counts);							 //Функция очистки экрана
void PrintToField(char *str);												 //Функция вывода
void interrupt newInt9(void);												 //Функция обработки прерывания
void interrupt (*oldInt9)(void);										 //Указатель на обработчик прерывания
void Indicators(unsigned char mask);								 //Функция управления индикаторами
void Blink(void);																		 //Функция мигания индикаторами
void ReadingReg(void);															 //Функция вывода состояния регистров
void Scroll(void);																	 //Функция смещения строк
void myitoa(int number, char *destination, int base); //Функция перевода чисел в заданную систему счисления


int isResend = 1;		//Флаг ошибки
int quitFlag = 0;		//Флаг выхода из программы
int blinkingON = 0; //Флаг мигания индикаторами

void main()
{
	clrscr();
	PrintToPoint(header, 0, 0);		 // Вывод шапки
	PrintToPoint(regStatus, 6, 0); //Вывод состояния регистров
	PrintToPoint(menu, 1, 0);			 // Вывод меню
	oldInt9 = getvect(9);					 //Сохраняем указатель на старый обработчик
	setvect(9, newInt9);					 //Меняем на новый
	while (!quitFlag)
	{
		if (blinkingON)
			Blink();
	}
	setvect(9, oldInt9); //Восстанавливаем старый
	clrscr();
	return;
}
void interrupt newInt9() //Функция обработки прерывания
{
	char str6[9];
	unsigned char value = 0;
	ReadingReg(); //Вывод функции вывода состояния регистров
	if (count >= 1199)
	{
		Scroll();			//Вызов функции смещения строк
		y = 24;				//Установка курсора в 24 строку
		x = 0;				//0 позицию строки
		count = 1120; //счетчик символов (-80)
	}
	value = inp(0x60);									//Получение значения из порта 60h
	if (value == 0x8b || value == 0x01) //Если 0 или ESC, то устанавливаем флаг выхода
		quitFlag = 1;											//Установка флага выхода
	if (value == 0x02)									//Если 1 , то производим очистку экрана
	{
		ClearScreen(10, 1279); //Очистка экрана
		y = 10;								 //Установка курсора в начальную позцию
		x = 0;
		count = 0; //Счетчик смволов 0
	}
	if (value == 0x03 && blinkingON == 0) //Если 2 , то поставить или снять флаг мигания
		blinkingON = 1;
	else if (value == 0x02 && blinkingON == 1)
		blinkingON = 0;
	if (value != 0xFA && blinkingON == 1) //Если нет подтверждения успешного выполнения
		isResend = 1;
	else
		isResend = 0; //то устанавливаем флаг повторной передачи
	if (value == 0x04)
		blinkingON = 0;
	
	myitoa(value, str6, 16); //Перевод скан-кода в шестнадцатиричную систему счисления
	PrintToField(str6);			//Вывод скан-кода символа
	outp(0x20, 0x20);				//Сброс контроллера прерывания
}

void Indicators(unsigned char mask) //Функция управления индикаторами
{
	isResend = 1;
	while (isResend)
	{
		while ((inp(0x64) & 0x01) != 0x00)
			;
		outp(0x60, 0xED);
		delay(50);
	}
	isResend = 1;
	while (isResend)
	{
		while ((inp(0x64) & 0x01) != 0x00)
			;
		outp(0x60, mask);
		delay(50);
	}
}
void Blink() // Функция мигания индикаторами
{
	Indicators(0x02);
	delay(50);
	Indicators(0x01);
	delay(30);
	Indicators(0x04);
	delay(50);
	Indicators(0x00);
	delay(20);
	Indicators(0x06);
	delay(30);
	Indicators(0x07);
	delay(50);
}
void PrintToPoint(char *str, int line, int column) //Функция вывода в опеределенную позицию
{
	int i;
	char far *start = (char far *)0xb8000000; //Начальный адрес видео буфера
	char far *v;
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == '\n') //Если в строке находится символ \n
		{
			line++;			//Переход на следующую строку
			column = 0; //Установка курсора в начало строки
			continue;		//Пропуск вывода символа \n
		}
		v = start + line * 160 + column * 2;
		column++;
		*v = str[i]; //Посимвольный вывод
		v++;
	}
}
void ClearScreen(int line, int counts) //Функция очистки экрана
{
	int i;
	int column = 0;
	char far *start = (char far *)0xb8000000; //Начальный адрес видео буфера
	char far *v;
	for (i = 0; i < counts; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		*v = ' ';
		v++;
	}
}
void PrintToField(char *str) //Функция вывода в поле для вывода
{
	int i;
	char far *start = (char far *)0xb8000000; //Начальный адрес видео буфера
	char far *v;
	for (i = 0; str[i] != '\0'; i++)
	{
		v = start + y * 160 + x * 2;
		x++;
		*v = str[i]; //Посимвольный вывод
		v++;
		count++; //Увеличение счетчика количества символов
	}
	v = start + y * 160 + x * 2;
	x++;
	*v = ' ';
	v++;
	count++;
}
void ReadingReg() //Функция вывода состояния регистров
{
	unsigned char temp;
	char str[4];
	char str1[9];
	temp = inp(0x64); // Получение регистра 64 порта
	myitoa(temp, str, 16);
	myitoa(temp, str1, 2);
	PrintToPoint("(64h)", 7, 0);
	PrintToPoint(str, 7, 6);
	PrintToPoint("000", 7, 9);
	PrintToPoint(str1, 7, 12);

	temp = inp(0x60); // Получение регистра 60 порта
	myitoa(temp, str, 16);
	myitoa(temp, str1, 2);
	PrintToPoint("(60h)", 7, 21);
	PrintToPoint(str, 7, 27);
	PrintToPoint(str1, 7, 30);

	temp = inp(0x61); // Получение регистра 61 порта
	myitoa(temp, str, 16);
	myitoa(temp, str1, 2);
	PrintToPoint("(61h)", 8, 0);
	PrintToPoint(str, 8, 6);
	PrintToPoint("00", 8, 9);
	PrintToPoint(str1, 8, 11);
}
void Scroll() //Функция прокрутки экрана
{
	int i;
	int column = 0;
	char *str = malloc(1280);									//Выделение памяти для данных
	char far *start = (char far *)0xb8000000; //Начальный адрес видео буфера
	char far *v;
	int line = 11; //Номер строки , откуда начнется считывание
	for (i = 0; i < 1200; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		str[i] = *v; //Считывание блока данных
	}
	PrintToPoint(str, line - 1, 0); //Перезапись блока данных
	ClearScreen(24, 80);						//Очистка последней строки
	free(str);
}
void myitoa(int number, char *destination, int base) //Функция перевода в заданную систему счисления
{
	int count = 0;
	int i;
	do
	{
		int digit = number % base;
		destination[count++] = (digit > 9) ? digit - 10 + 'A' : digit + '0';
	} while ((number /= base) != 0);
	destination[count] = '\0';

	for (i = 0; i < count / 2; ++i)
	{
		char symbol = destination[i];
		destination[i] = destination[count - i - 1];
		destination[count - i - 1] = symbol;
	}
}