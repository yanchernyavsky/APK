
// 724402-2 Chernyavsky Y.A.
// Laboratornaya rabota 5 "Chasi realnogo vremeni"

#include <dos.h>
#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#pragma warn -sus

int msCounter = 0;  // Schetchik dlya zaderzhki
int x = 45;			// Global'nye peremennye stroki i stolbca
int y = 7;			//  dlya vyvoda na ehkran
char massive[20];	//  massiv dlya vvoda dannyh
int nomer = 0;		// kolichestvo ehlementov v massive
int enter = 0;		// peremennaya dlya klavishi enter
int gran = 0;		// granica dlya backspace
int tik = 0;		//  Schetchik vyzova preryvaniya tajmera

void interrupt far (*oldInt70h)(void);		// Ukazatel' na funkciyu obrabotki preryvanij RTC
void interrupt far NewInt70Handler(void);	// Funkciya obrabotki preryvanij RTC


void CleanScreen(int line, int counts, int column);  // Funkciya ochistki ehkrana
void LockClockUpdate(void);			// Zapret na obnovlenie tajmera
void UnlockClockUpdate(void);		// Razreshenie na obnovlenie tajmera
int BCDToInteger(int bcd);			// Perevod v tip int
unsigned char IntToBCD(int value);  // Perevod v dvoichno-desyatichnyj kod
void GetTime(void);
void SetTime(void);
void CustomDelay(void);		// Funkciya zaderzhki vremeni
void redakttime(void);		// Funkciya redaktirovaniya vremeni
void WaitClockIsFree(void); // Provekrka na vozmozhnost' obrashcheniya k chasam

void interrupt newInt9(void);		// Funkciya obrabotki preryvaniya klaviatury
void interrupt(*oldInt9)(void);		// Ukazatel' na obrabotchik preryvaniya klaviatury
void Vprint(char *str);				// Funkciya vyvoda
void StaticLines(char *str, int line, int column); // Funkciya vyvoda
void inittimer(void);				// Inicializaciya tajmera
void inittimerstop(void);			// Zamena tajmera na standartnyj
void cleanmassive(void);			// Funkciya ochistki massiva

void interrupt TimerHandler(void);		// Funkciya obrabotki preryvanij sistemnogo tajmera
void interrupt(*oldTimerHandler)(void); // Ukazatel' na funkciyu obrabotki preryvanij sistemnogo tajmera

void main()
{
	char c;
	clrscr();
	StaticLines("724402-2 Chernyavsky Y.A. Laboratornaya rabota 5 'Chasi realnogo vremeni'", 0, 0);
	StaticLines("Press 1 - Show time", 1, 0);
	StaticLines("Press 2 - Set time", 2, 0);
	StaticLines("Press 3 - Delay time", 3, 0);
	StaticLines("Press Esc - quit", 4, 0);
	oldTimerHandler = getvect(0x1c);

	while(c != 27)
	{
		c = getch();
		switch(c)
		{
			case '1':
				inittimer();
				GetTime();
				StaticLines("Input  to continue", 8, 0);
				getch();
				CleanScreen(6, 720, 0);
				inittimerstop();
				break;
			case '2':
				inittimer();
				SetTime();
				break;
			case '3':
				inittimer();
				CustomDelay();
				break;
			case 27:
				inittimerstop();
				return;
		}
		c = 0;
	}


}
void inittimer() {
	disable();                     // Preryvanie tajmera
	setvect(0x1c, TimerHandler);   // возникает 18,2 раза в секунду
	enable();
}
void inittimerstop() {
	disable();
	setvect(0x1c, oldTimerHandler);   // Установка старого обработчика
	enable();
}
void interrupt newInt9()	// Функция обработки прерывания клавиатуры
{
	char str6[9];
	unsigned char value = 0;
	value = inp(0x60);    // Poluchenie znacheniya iz porta 60h
	if (value == 0x03)   // if 2
	{
		itoa(2, str6, 10);
		Vprint(str6);
		massive[nomer] = '2';
		nomer++;
	}
	if (value == 0x02)  // if 1
	{
		itoa(1, str6, 10);
		Vprint(str6);
		massive[nomer] = '1';
		nomer++;
	}
	if (value == 0x04)   // if 3
	{
		itoa(3, str6, 10);
		Vprint(str6);
		massive[nomer] = '3';
		nomer++;
	}
	if (value == 0x05)   // if 4
	{
		itoa(4, str6, 10);
		Vprint(str6);
		massive[nomer] = '4';
		nomer++;
	}
	if (value == 0x06)   // if 5
	{
		itoa(5, str6, 10);
		Vprint(str6);
		massive[nomer] = '5';
		nomer++;
	}
	if (value == 0x07)   // if 6
	{
		itoa(6, str6, 10);
		Vprint(str6);
		massive[nomer] = '6';
		nomer++;
	}
	if (value == 0x08)   // if 7
	{
		itoa(7, str6, 10);
		Vprint(str6);
		massive[nomer] = '7';
		nomer++;
	}
	if (value == 0x09)   // if 8
	{
		itoa(8, str6, 10);
		Vprint(str6);
		massive[nomer] = '8';
		nomer++;
	}
	if (value == 0x0A)   // if 9
	{
		itoa(9, str6, 10);
		Vprint(str6);
		massive[nomer] = '9';
		nomer++;
	}
	if (value == 0x0B)   // if 0
	{
		itoa(0, str6, 10);
		Vprint(str6);
		massive[nomer] = '0';
		nomer++;
	}
	if (value == 0x0E)   // if backspace
	{
		if (x != gran) {
			x--;
			Vprint(" ");
			x--;
			massive[nomer] = '\0';
			nomer--;
			massive[nomer] = '\0';
		}
	}
	if (value == 0x1C)   // if Enter
	{
		enter = 1;
	}
	outp(0x20, 0x20);   // Sbros kontrollera preryvaniya
}

void WaitClockIsFree()
{
	do
	{
		outp(0x70, 0x0A);
	} 
	while( inp(0x71) & 0x80 ); // proverka na 1 v 7 bite
}
void interrupt TimerHandler() // Funkciya obrabotki preryvaniya tajmera
{
	char str6[9];
	disable();
	tik++;             // Uvelichivaem kolichestvo tikov
	if (!(tik % 18))   // esli kratno 18,to proshla 1 sekunda
	{
		CleanScreen(6, 79, 0);
		GetTime();     // Vyvod vremeni i daty
	}
	outp(0x20, 0x20);  // Sbros kontrollera preryvaniya
	enable();
}
void GetTime()
{
	unsigned char value;
	char str6[9];
	StaticLines("Time", 6, 0);
	WaitClockIsFree(); // Proverka , svobodnyj li tajmer
	outp(0x70, 0x04); // Schityvanie dannyh dlya chasov
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		StaticLines("0", 6, 6);
		StaticLines(str6, 6, 7);
	}
	else {
		StaticLines(str6, 6, 6);
	}
	StaticLines(":", 6, 8);
	if (BCDToInteger(value) > 23) {
		redakttime();
		StaticLines("00", 6, 6);
	}

	WaitClockIsFree();
	outp(0x70, 0x02); // Schityvanie dannyh dlya minut
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		StaticLines("0", 6, 9);
		StaticLines(str6, 6, 10);
	}
	else {
		StaticLines(str6, 6, 9);
	}
	StaticLines(":", 6, 11);

	WaitClockIsFree();
	outp(0x70, 0x00); // Schityvanie dannyh dlya secund
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		StaticLines("0", 6, 12);
		StaticLines(str6, 6, 13);
	}
	else {
		StaticLines(str6, 6, 12);
	}

	StaticLines("Date", 6, 18);
	WaitClockIsFree();
	outp(0x70, 0x07); // Schityvanie dannyh o dne
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		StaticLines("0", 6, 24);
		StaticLines(str6, 6, 25);
	}
	else {
		StaticLines(str6, 6, 24);
	}
	StaticLines(".", 6, 26);

	WaitClockIsFree();
	outp(0x70, 0x08); // Schityvanie dannyh o mesyace
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		StaticLines("0", 6, 27);
		StaticLines(str6, 6, 28);
	}
	else {
		StaticLines(str6, 6, 27);
	}
	StaticLines(".", 6, 29);

	WaitClockIsFree();
	outp(0x70, 0x09); // Schityvanie dannyh o gode
	value = inp(0x71); 
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		StaticLines(str6, 6, 33);
		StaticLines("200", 6, 30);
	}
	else {
		StaticLines(str6, 6, 32);
		StaticLines("20", 6, 30);
	}
	
	WaitClockIsFree();
	outp(0x70, 0x06); // Schityvanie dannyh o dne nedeli
	value = inp(0x71);
	
	switch(BCDToInteger(value))
	{
		case 1:StaticLines("Sunday", 6, 36);  break;
		case 2:StaticLines("Monday", 6, 36);  break;
		case 3:StaticLines("Tuesday", 6, 36);  break;
		case 4:StaticLines("Wednesday", 6, 36);   break;
		case 5:StaticLines("Thursday", 6, 36);  break;
		case 6:StaticLines("Friday", 6, 36);   break;
		case 7:StaticLines("Saturday", 6, 36);   break;
		default:StaticLines("Undefined day of week", 6, 36);  break;
	}
	
}

void SetTime()
{
	int hours = 25, minutes = 60, seconds = 60, weekDay = 8, monthDay = 32, month = 13, year = 21;
	int kolday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31};
	char str[3];
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    // Preryvanie klaviatury
	setvect(9, newInt9);
	enable();
	x = 20;
	gran = x;
	y = 7;
	StaticLines("Enter hours(0-23): ", 7, 0);
	
		while (hours < 0 || hours>23)  // Ustanovka chasov
		{

			if (enter == 1)
			{
				hours = atoi(massive);
				enter = 0;
			}
		}

	cleanmassive();
	StaticLines("Enter minutes(0-59): ", 8, 0);
	x = 20;
	gran = x;
	y++;
	while (minutes < 0 || minutes>59)  // Ustanovka minut
	{

		if (enter == 1)
		{
			minutes = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	StaticLines("Enter seconds(0-59): ", 9, 0);
	x = 20;
	gran = x;
	y++;
	while (seconds < 0 || seconds>59) // Ustanovka secund
	{

		if (enter == 1)
		{
			seconds = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	StaticLines("Enter year(2000-2020): ", 10, 0);
	x = 25;
	gran = x;
	y++;
	while (year < 2000 || year > 2020)  // Ustanovka goda
	{
		if (enter == 1)
		{
			year = atoi(massive);
			enter = 0;
		}
	}
	if (year % 4 != 0 || year % 100 == 0 && year % 400 != 0)
	{}
	else
		kolday[1] = 29;
	year = year - 2000;
	cleanmassive();
	StaticLines("Enter month(1-12): ", 11, 0);
	x = 20;
	gran = x;
	y++;
	 while (month < 1 || month>12) // Ustanovka mesyaca
	 {

		if (enter == 1)
		{
			month = atoi(massive);
			enter = 0;
		}
	 }
	cleanmassive();
	itoa(kolday[month - 1], str, 10);
	StaticLines("Enter day of month 1- ", 12, 0);
	StaticLines(str, 12, 23);
	x = 30;
	gran = x;
	y++;
	while (monthDay < 1 || monthDay>kolday[month-1])  // Ustanovka dn'ya
	{

		if (enter == 1)
		{
			monthDay = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	StaticLines("Enter week day number(1-7): ", 13, 0);
	x = 30;
	gran = x;
	y++;
	while (weekDay < 1 || weekDay>7) // Ustanovka dn'ya nedely
	{
		if (enter == 1)
		{
			weekDay = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();

	setvect(9, oldInt9);

	LockClockUpdate();  // Zapret na obnovlenie
	
	outp(0x70, 0x04);
	outp(0x71, IntToBCD(hours));	
	
	outp(0x70, 0x02);
	outp(0x71, IntToBCD(minutes));	
	
	outp(0x70, 0x00);
	outp(0x71, IntToBCD(seconds));	
	
	outp(0x70, 0x06);
	outp(0x71, IntToBCD(weekDay));	
	
	outp(0x70, 0x07);
	outp(0x71, IntToBCD(monthDay));	
	
	outp(0x70, 0x08);
	outp(0x71, IntToBCD(month));	
	
	outp(0x70, 0x09);
	outp(0x71, IntToBCD(year));

	UnlockClockUpdate(); // Razreshenye na obnovlenie
	
	StaticLines("Input  to continue", 14, 0);
	getch();
	CleanScreen(6, 720, 0);
	inittimerstop();  // stop timer
	
}
void redakttime() 
{
	int hours = 0, weekday = 8, monthday = 32, month = 13, year = 21;
	int kolday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	unsigned char value;
	hours = 0;

	WaitClockIsFree();
	outp(0x70, 0x07); // day
	value = inp(0x71);
	monthday = BCDToInteger(value);

	WaitClockIsFree();
	outp(0x70, 0x08); // month
	value = inp(0x71);
	month = BCDToInteger(value);
	
	WaitClockIsFree();
	outp(0x70, 0x09); // year
	value = inp(0x71);
	year = BCDToInteger(value);
	year = year + 2000;
	if (year % 4 != 0 || year % 100 == 0 && year % 400 != 0)
	{
	}
	else
		kolday[1] = 29;

	WaitClockIsFree();
	outp(0x70, 0x06); // week day
	value = inp(0x71);
	weekday = BCDToInteger(value);

	weekday++;
	if (weekday > 7)
		weekday = 1;
	monthday++;
	if (monthday > kolday[month - 1]) {
		monthday = 1;
		month++;
		if (month > 12) {
			month = 1;
			year++;
		}
	}
	year = year - 2000;

	LockClockUpdate();

	outp(0x70, 0x04);
	outp(0x71, IntToBCD(hours));

	outp(0x70, 0x06);
	outp(0x71, IntToBCD(weekday));

	outp(0x70, 0x07);
	outp(0x71, IntToBCD(monthday));

	outp(0x70, 0x08);
	outp(0x71, IntToBCD(month));

	outp(0x70, 0x09);
	outp(0x71, IntToBCD(year));

	UnlockClockUpdate();
	
}
void LockClockUpdate()
{
	unsigned char value;
	WaitClockIsFree();  // Proverka chasov
	outp(0x70,0x0B);
	value = inp(0x71);  //  ustanovka 1 v 7 bite
	value|=0x80;		// Zapret obnovleniya chasov
	outp(0x70, 0x0B);
	outp(0x71, value);
}

void UnlockClockUpdate()
{
	unsigned char value;
	WaitClockIsFree(); 
	outp(0x70,0x0B);
	value = inp(0x71);	// ustanovka 0 v 7 bite
	value-=0x80;        // razreshenye na obnovleniya chasov
	outp(0x70, 0x0B);   
	outp(0x71, value);
}

void interrupt far NewInt70Handler(void)
{
	msCounter++;

	outp(0x70,0x0C); 
	inp(0x71);       
	
	outp(0x20, 0x20);
	outp(0xA0, 0x20);
}

void CustomDelay()
{
	int delayPeriod=0;
	unsigned char value;
	StaticLines("Delay in ms (input between 1000 and 10000):  ", 7, 0);
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    // Preryvanie klaviatury
	setvect(9, newInt9);
	enable();
	x = 45;
	gran = x;
	y = 7;
		while (delayPeriod < 1000 || delayPeriod > 10000)
		{
			
			if (enter == 1)
			{
				delayPeriod = atoi(massive);
				enter = 0;
			}
		}
	setvect(9, oldInt9);
	cleanmassive();
	setvect(0x1c, oldTimerHandler);
	disable();   
	oldInt70h = getvect(0x70);
	setvect(0x70, NewInt70Handler);
	enable();     

	value = inp(0xA1);
	outp(0xA1, value & 0xFE);  // Chasy zanyaty 
	
	outp(0x70, 0x0B);
	value = inp(0x0B);
	outp(0x70, 0x0B);
	outp(0x71, value & 0x40);  // Vyzov periodicheskogo preryvaniya

	msCounter = 0;
	while (msCounter != delayPeriod);
	StaticLines("End delay", 8, 0);
   
	setvect(0x70, oldInt70h);
	WaitClockIsFree();
	setvect(0x1c,TimerHandler);

	StaticLines("Input  to continue", 10, 0);
	getch();
	CleanScreen(6, 720, 0);
	inittimerstop();
}



int BCDToInteger (int bcd)
{
	return bcd % 16 + bcd / 16 * 10;  // perevod v int
}

unsigned char IntToBCD (int value)
{
	return (unsigned char)((value/10)<<4)|(value%10); // perevod v BCD
}
void Vprint(char *str)  // vyvod 
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
		*v = 0x0F;
	}
	
}

void StaticLines(char *str, int line, int column)  // vyvod 
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
		*v = 7;
	}

}
void CleanScreen(int line, int counts, int column) 
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


void cleanmassive()
{
	int i;
	for (i = 0; i < 20; i++) {
		massive[i] = '\0';
	}
	nomer = 0;
}
