//724402-2 Chernyavsky Y.A.

//Labaratornaia rabota 6: Funkcii BIOS

#include <bios.h>

#include <time.h>

#include <stdlib.h>

#include <stdio.h>

#include <memory.h>

#include <conio.h>

#include <dos.h>

int bcd1bin(char *bcd) //BCD to BIN -> 1 byte

{

return (((*bcd) & 0x0f) + 10 * (((*bcd) & 0xf0) >> 4));

}

int bcd2bin(char *bcd) // BCD to BIN -> 2 byte

{

return (bcd1bin(bcd) + 100 * bcd1bin(bcd + 1));

}

int bin1bcd(int bin, char *bcd) // BIN to BCD -> 1 byte

{

int i;

i = bin / 10;

*bcd = (i << 4) + (bin - (i * 10));

return i;

}

void indidication(void)

{

printf("724402-2 Chernyavsky Y.A.\nLabaratornaia rabota 6 Funkcii BIOS\n");

printf(" 1 - Calling the INT 11h (PC config) \n 2 - Calling the INT 12h (RAM)\n 3 - Calling the INT 16h (TXT Input)\n 4 - Calling the INT 1Ah (Set data)\n ESC - exit\n"); //1/read list of equipment 2/size of continuous memory 3/keyboard input output 4/time and data

}

typedef struct _HDWCFG

{

unsigned HddPresent : 1; //0 diskovod

unsigned NpuPresent : 1; //1 matematicheskiy soprotsessor

unsigned AmountOfRAM : 2; //2-3 razmer OZU

unsigned VideoMode : 2; //4-5 aktivnyy videorezhim

unsigned NumberOfFdd : 2; //6-7 chislo obnaruzhennykh NGMD

unsigned DmaPresent : 1; //8 nalichie kontrollera DMA

unsigned NumberOfCom : 3; //9-11 chislo COM-portov

unsigned GamePresent : 1; //12 igrovoy adapter

unsigned JrComPresent : 1; //13 rezerv

unsigned NumberOfLpt : 2; //14-15 chislo LPT-portov

} HDWCFG;

char* tobin(char* str, unsigned char t)

{

int mask, i;

for (i = 0, mask = 0x80; mask != 0; mask >>= 1, i++)

{

str[i] = t & mask ? '1' : '0';

}

str[8] = '\0';

return str;

}

char* tohex(char* str, unsigned char t)

{

char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };

str[2] = '\0';

str[1] = hex[t & 0xF];

str[0] = hex[t >> 4];

return str;

}

void showWord(unsigned int uword)

{

char high[9];

char low[9];

tohex(high, uword >> 8);

tohex(low, uword & 0x00FF);

printf("\nAX: hex(%s%s)", high, low);

tobin(high, uword >> 8);

tobin(low, uword & 0x00FF);

printf("\tbin(%s%s)\n", high, low);

}

void main()

{

	long hour, minute, second;

	int key, pick;

	union REGS rg;

	HDWCFG HdwCfg;

	unsigned uword;

	char *month_to_text[] =

	{ "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };

	clrscr();

	indidication();

	while (key != 27)//Poka ne nazhata klavisha ESC

	{

		key = getch();

		if (key == 27) exit(1);

		if (key == 49)//ESli 1, to

		{

			printf("\n\n Calling the INT 11h \n");

			uword = (unsigned int)rg.x.ax; // Poluchaem slovo konfiguracii i sohranyaem

			showWord(uword);

			rg.h.ah = 0x0;// Vyzyvaem preryvanie INT 11h dlya polucheniya

			int86(0x11, &rg, &rg); // slova konfiguratsii komp'yutera

			uword = (unsigned int)rg.x.ax;// Poluchaem slovo konfiguratsii i sokhranyaem

			memcpy(&HdwCfg, &uword, 2);// ego v strukture HdwCfg

			if (HdwCfg.HddPresent)

				printf("\nHDD present");//diskovod

			if (HdwCfg.NpuPresent)

				printf("\nNPU present");//matematicheskiy soprotsessor

			printf("\nRAM banks: %d", HdwCfg.AmountOfRAM); //razmer OZU

			printf("\nVideo Mode: %d", HdwCfg.VideoMode); //aktivnyy videorezhim

			printf("\nNumver of FDD: %d", HdwCfg.NumberOfFdd + 1); //chislo obnaruzhennykh NGMD

			if (HdwCfg.DmaPresent)

				printf("\nDMA present");//nalichie kontrollera DMA

			printf("\nNumber of COM ports: %d", HdwCfg.NumberOfCom); //chislo COM-portov

			if (HdwCfg.GamePresent)

				printf("\nGame Adapter present");//igrovoy adapter

			if (HdwCfg.JrComPresent)

				printf("\nPCjr Com present");//rezerv

			printf("\nNumber of LPT ports: %d", HdwCfg.NumberOfLpt); //chislo LPT-portov

			printf("\nPress Any Key...");

			getch();

			clrscr();

			indidication();

		}

		if (key == 50)//Esli 2 , to

		{

			printf("\n\nCalling the INT 12h \n");

			rg.h.ah = 0x0;// Vyzyvaem preryvanie INT 12h dlya opredeleniya

			int86(0x12, &rg, &rg); // ob'ema osnovnoy operativnoy pamyati

			printf("\nRAM installed: %d Kbytes", (unsigned int)rg.x.ax);

			printf("\nPress Any Key...");

			getch();

			clrscr();

			indidication();

		}

		if (key == 51)//Esli 3 , to

		{

			printf("\n\nCalling the INT 16h \n");

			printf("\n 1 - Input a symbol\n 2 - Input a string\n 3 - Input with waiting\n");

			while (pick < 49 || pick>51)

			{

				pick = getch();

			}

			switch (pick)

			{

			case 49:

				printf("\nInput the symbol: ");

				rg.h.ah = 0;// Vyzyvaem preryvanie INT 16h dlya

				int86(0x16, &rg, &rg);// vvoda simvola

				printf("\nSymbol is %c", rg.h.al);//i vyvoda ego na ekran

				break;

			case 50:

				printf("\nPRESS ESC TO EXIT\nThe string is: ");

				while (rg.h.ah != 1)

				{

					rg.h.ah = 0;// Vyzyvaem preryvanie INT 16h dlya

					int86(0x16, &rg, &rg);// vvoda stroki

					printf("%c", rg.h.al);//i vyvoda ego na ekran

				}

				break;

			case 51:

				printf("PRESS ESC TO EXIT\nInput the string: ");

				while (rg.h.ah != 1)

				{

					rg.h.ah = 0;// Vyzyvaem preryvanie INT 16h dlya

					int86(0x16, &rg, &rg);// vvoda stroki

					delay(1000);

					printf("%c", rg.h.al);//i vyvoda ego na ekran

				}

				break;

			case 27:

				break;

			}

			pick = 0;

			rg.h.ah = 0;

			printf("\nPress Any Key...");

			getch();

			clrscr();

			indidication();

		}

		if (key == 52)//Esli 4, to

		{

			printf("\n\nCalling the INT 1Ah \n");

			printf("\n1.See the current time and date\n2.Set the time\n");

			while (pick < 49 || pick>50)

			{

				pick = getch();

			}

			switch (pick)

			{

			case 49:

				rg.h.ah = 0x02;//Schityvanie vremeni s chasov real'nogo vremeni

				int86(0x1a, &rg, &rg);

				printf("\nTime is: %02.2d:%02.2d:%02.2d\n",

					bcd1bin(&(rg.h.ch)), //znachenie chasov

					bcd1bin(&(rg.h.cl)), //znachenie minut

					bcd1bin(&(rg.h.dh)) //znachenie sekund

				);

				rg.h.ah = 0x04;//Schityvanie daty s chasov real'nogo vremeni

				int86(0x1a, &rg, &rg);

				printf("\nDate is: %d day,%s,%d year.\n",

					bcd1bin(&(rg.h.dl)), //znachenie dnya

					month_to_text[bcd1bin(&(rg.h.dh)) - 1], //znachenie mesyatsa

					bcd2bin(&(rg.h.cl)) //znachenie goda

				);

				break;

			case 50:

				do

				{

					printf("\n Enter hour (0-23): ");

					scanf("%li", &hour);

				} while (hour > 23 || hour < 0);

				do

				{

					printf("\n Enter minute (0-60): ");

					scanf("%li", &minute);

				} while (minute > 59 || minute < 0);

				do

				{

					printf("\n Enter second (0-60): ");

					scanf("%li", &second);

				} while (second > 59 || second < 0);

				rg.h.ah = 0x03; //Ustanovka vremeni

				bin1bcd(hour, &(rg.h.ch)); //znachenie chasov

				bin1bcd(minute, &(rg.h.cl)); //znachenie minut

				bin1bcd(second, &(rg.h.dh)); //znachenie sekund

				int86(0x1a, &rg, &rg);

				break;

			}

			pick = 0;

			printf("\nPress Any Key...");

			getch();

			clrscr();

			indidication();

		}

	}

}