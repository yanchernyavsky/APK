//724402-2
//Chernyavsky YA
//LR-2 System timer

#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>

struct note {
	int tone;
	int delay;
};

void PlayNote(struct note n);
void ResetScreen(void);
void GetStateWord(void);
void setTonality(int freq);
void onsound(void);
void offsound(void);
char* fillBin(char* in);

int main()
{
	struct note song_notes[50] = {

		{392, 500},//g4
		{262, 500},//c4
		{330, 250},//e4
		{349, 250},//f4
		{392, 500},//g4
		{262, 500},//c4
		{330, 250},//e4
		{349, 250},//f4

		{392, 500},//g4
		{262, 500},//c4
		{311, 250},//ds4
		{349, 250},//f4
		{294, 500},//d4
		{196, 500},//g3
		{233, 250},//as3
		{262, 250},//c4

		{294, 500},//d4
		{196, 500},//g3
		{233, 250},//as3
		{262, 250},//c4
		{294, 500},//d4
		{196, 500},//g3
		{233, 250},//as3
		{262, 250},//c4

		{294, 500},//d4
		{196, 500},//g3
		{233, 250},//as3
		{262, 250},//c4
		{294, 1000},//d4
		{349, 1000},//f4
		{233, 1000},//as3
		{311, 250},//ds4

		{294, 250},//d4
		{349, 1000},//f4
		{233, 1000},//as3
		{311, 250},//ds4
		{294, 250},//d4
		{262, 500},//c4
		{208, 250},//gs3
		{233, 250},//as3

		{262, 500},//c4
		{175, 500},//f3
		{208, 250},//gs3
		{233, 250},//as3
		{262, 500},//c4
		{175, 500},//f3
		{208, 250},//gs3
		{233, 250},//as3

		{262, 500},//c4
		{175, 500},//f3


	};

	int i, length = sizeof(song_notes) / sizeof(song_notes[0]);
	char wait[4] = "-\\|/";
	ResetScreen();

	while (1)	//Меню программы
	{
		printf("1. Get status word\n2. Play sound\n3. Exit\n");
		switch (getch())	//Считывание символа введенного с клавиатуры
		{
		case '1':          	
			GetStateWord();	//Вывод регистра словосостояния и 61h порта
			getch();
			ResetScreen();	//обновление экрана
			break;
		case '2':
			GetStateWord();
			for (i = 0; i < length; i++)//Проигрывание мелодии
			{
				printf("(%i|%-4i)%c\b", song_notes[i].tone, song_notes[i].delay, wait[i % 4]); //Вывод частоты и длительности ноты
				onsound();					//Включение динамика
				PlayNote(song_notes[i]);	//Проигрывание ноты из массива
				offsound();					//Выключение динамика
			}
			GetStateWord();
			printf("press key...\n");
			getch();
			ResetScreen();	//Обновление экрана
			break;
		case '3': case 27:	//Считан сивол 3 или Esc
			return 0;		//Выход из программы
		default:			//Считан любой другой символ
			printf("Wrong code!\n");
			getch();
			ResetScreen();
			break;
		}
	}
}
void ResetScreen() {	//Функция обновления экрана
	system("cls");
	printf("%60s", "724402-2. Chernyavsky Y.A. Lab02 - System timer\n\n");
}
void setTonality(int freq) {	//установка тональности и проигрывание звука
	short value = 1193180 / freq;		//Частота таймера равна 1193180 Гц
	outp(0x42, (char)value);
	outp(0x42, (char)(value >> 8));
}
void onsound() {	//Включение динамика
	char port61;
	port61 = inp(0x61);
	port61 = port61 | 3;
	outp(0x61, port61);
}
void offsound() {	//Выключение динамика
	char port61;
	port61 = port61 & 0xFFFC;
	outp(0x61, port61);
}
void PlayNote(struct note n) {	//Функция проигрывания ноты
	setTonality(n.tone);
	delay(n.delay);
}

void GetStateWord() {	//Функция вывода регистра словосостояния и регистра порта 61h
	unsigned i;
	unsigned char temp;
	char* fill = (char*)calloc(9, sizeof(char));
	char* fill61 = (char*)calloc(9, sizeof(char));
	
	//считывание словосостояния канала 0
	outp(0x43, 0xe2);
	temp = inp(0x40);
	itoa(temp, fill, 2); //перевод словосостояния в двоичную систему
	fill = fillBin(fill);
	//вывод словосостояния канала 0
	printf("\nChannel 0 state word: %02.2X(h) | %8s(b)", inp(0x40), fill);

	//считывание словосостояния канала 1
	outp(0x43, 0xe4);
	temp = inp(0x41);
	itoa(temp, fill, 2);
	fill = fillBin(fill);
	//вывод словосостояния канала 1
	printf("\nChannel 1 state word: %02.2X(h) | %8s(b)", inp(0x41), fill);

	
	//считывание словосостояния канала 2
	outp(0x43, 0xe8);
	temp = inp(0x42);
	itoa(temp, fill, 2);
	temp = inp(0x61);	//считывание регистра порта h61
	itoa(temp, fill61, 2);
	fill = fillBin(fill);
	fill61 = fillBin(fill61);
	//вывод словосостояния канала 2
	printf("\nChannel 2 state word: %02.2X(h) | %8s(b)", inp(0x42), fill);
	//вывод регистра 61h порта
	printf("\nPort 61 register:     %02.2X(h) | %8s(b)\n", inp(0x61), fill61);
	free(fill);
}

char* fillBin(char* in) {	//Функция дополнения двоичных чисел нулями
	char* out = (char*)calloc(9, sizeof(char));
	int zamount = 8 - strlen(in), j;
	for (j = 0; j < zamount; j++) {
		out[j] = '0';
	}
	strcat(out, in);
	return out;
}