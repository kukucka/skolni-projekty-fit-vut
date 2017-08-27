/**
 * Soubor:	proj1.c
 * Datum: 	Říjen 2016
 * Autor:	Marek Kukučka, xkukuc04@fit.vutbr.cz
 * Projekt: Prace s textem
 * Popis:	Program formátuje binární data do textové podoby nebo textovou podobu dat převádí do binární podoby. V případě převodu binárních dat na text bude výstupní formát obsahovat adresy vstupních bajtů, hexadecimální kódování a textovou reprezentaci obsahu. V případě převodu textu do binární podoby je na vstupu očekáváné hexadecimální kódování bajtů.
 */
 
#include <stdio.h>
#include <stdlib.h>  //funkce strtol
#include <ctype.h>   //funkce isblanc, isprint
#include <limits.h>  //INT_MAX

void binarni_do_textu(long int start, long int max_delka);
void binarni_posloupnost(long int delka_retezce);
void do_hexa();
int reverse();
int porovnat(char *arg);
int porovnat2(char *arg1, char *arg2);
void napoveda();

/**
 * Ve funkci main dochází k porovnání argumentů programu a k následnému výpisu 
 * v závislosti na počtu argumentů programu se zjišťuje o jaký argument se může jednat
 * pokud, žádný z argumentů funkce neodpovídá argumentu pro typ funkce je vypsána nápověda
 * pokud odpovídá argument, ale neodpovídá určovací hodnota argumentu je vypsána chyba
 * pro získání číselné hodnoty z argumentů je použita funkce strtol
 */

int main(int argc, char *argv[])
{	
	int typ_funkce;
	long int delka_retezce;  //argument určuje hodnotu pro min delku textového řetězce
	long int max_delka;  //argument určuje hodnotu pro maximalní délku
	long int start;  //argument určuje, na kterém znaku  začne výpis
	char * string, * konecCisla, * string2, * konecCisla2; //proměnné pro funkci strtol
	
	konecCisla = NULL;
	konecCisla2 = NULL;
	string = argv[2];
	string2 = argv[4];
	
	if(argc == 1)
		binarni_do_textu(0, 0);
	else if(argc == 2)
	{
		typ_funkce = porovnat(argv[1]); //zkontroluje jestli argv[1] obsahuje argument pro nějaký typ funkce
		
		if(typ_funkce == 1)
			do_hexa();	
		else if(typ_funkce == 2)
		{
			if(reverse() == EXIT_FAILURE)
				return EXIT_FAILURE;
			else
				reverse();
		}
	}
	else if(argc == 3)
	{
		typ_funkce = porovnat(argv[1]);	
		
		if (typ_funkce == 3)
		{
			start = strtol(string, &konecCisla, 10);  //funkce strtol převádí hodnotu argv[2] na long int
			
			if((*konecCisla != '\0') || (start < 0))  //pokud není start nazáporné číslo
			{
				fprintf(stderr, "Neplatná hodnota argumentu M.\n");
				return EXIT_FAILURE;			
			}
			else if(start > INT_MAX) 		//brání přetečení
			{	
				fprintf(stderr, "Chyba: Overflow, zadejte číslo menší než 2147483647.");
				return EXIT_FAILURE;
			}
			else
  			binarni_do_textu(start, 0);
		}	
		else if(typ_funkce == 4)
		{
			max_delka = strtol(string, &konecCisla, 10);
			
			if((*konecCisla != '\0') || (max_delka <= 0))   //pokud není max_delka kladné číslo
			{
				fprintf(stderr, "Neplatná hodnota argumentu N.\n");
				return EXIT_FAILURE;
			}
			else if(max_delka > INT_MAX)
			{
				fprintf(stderr, "Chyba: Overflow, zadejte číslo menší než 2147483647.");
				return EXIT_FAILURE;
			}
			else
    		binarni_do_textu(0 , max_delka);
		}
		else if(typ_funkce == 5)
		{
			delka_retezce = strtol(string, &konecCisla, 10);

			if((*konecCisla != '\0') || (delka_retezce <= 0) || (delka_retezce >= 200)) //pokud delka_retezce neleží v intervalu (0-200)
			{
				fprintf(stderr, "Neplatná hodnota argumentu N.");
				return EXIT_FAILURE;
			}
			else				
				binarni_posloupnost(delka_retezce);
		}
	}
	else if(argc == 5)	
	{
		typ_funkce = porovnat2(argv[1],argv[3]);
		
		if(typ_funkce == 6)
		{
			start = strtol(string, &konecCisla, 10);
			max_delka = strtol(string2, &konecCisla2, 10);	
			
			if((*konecCisla != '\0') || (start < 0) || (*konecCisla2 != '\0') || (max_delka <= 0)) //pokud max_delka není kladné číslo nebo start není nezaporné číslo
			{	
				fprintf(stderr, "Neplatná hodnota argumentu N a/nebo M.\n");
				return EXIT_FAILURE;			
			}
			else if((start > INT_MAX) || (max_delka > INT_MAX)) //brání přetečení
			{
				fprintf(stderr, "Chyba: Overflow, zadejte číslo menší než 2147483647.");
				return EXIT_FAILURE;		
			}		
			else
				binarni_do_textu(start, max_delka);	
		}
		else if(typ_funkce == 7)
		{			
			string = argv[4];
			string2 = argv[2];
			
			start = strtol(string, &konecCisla, 10);			
			max_delka = strtol(string2, &konecCisla2, 10);	
			
			if((*konecCisla != '\0') || (start < 0) || (*konecCisla2 != '\0') || (max_delka <= 0)) 
			{
				fprintf(stderr, "Neplatná hodnota argumentu N a/nebo M.\n");
				return EXIT_FAILURE;
			}
			else if((start > INT_MAX) || (max_delka > INT_MAX))
			{
				fprintf(stderr, "Chyba: Overflow, zadejte číslo menší než 2147483647.");
				return EXIT_FAILURE;
			}
			else
				binarni_do_textu(start, max_delka);				
		}
	}
	else
		napoveda();	

	return EXIT_SUCCESS;
}

/**
 * Funkce porovnává jednotlivé znaky argumentu arg[] a tak zjišťuje, jestli byl zadán argument určující typ funkce.
 * Pokuď objeví argument, který odpovídá jedné z variant přiřadí jeho návratovou hodnotu proměné porovnat. 
 */

int porovnat(char *arg)
{
	char c;
	if((c = arg[0]) == '-')
	{
		if(((c = arg[1]) == 'x') && ((c = arg[2]) == '\0'))
			return 1;
		else if(((c = arg[1]) == 'r') && ((c = arg[2]) == '\0'))
			return 2;
		else if(((c = arg[1]) == 's') && ((c = arg[2]) == '\0'))
			return 3;
		else if(((c = arg[1]) == 'n') && ((c = arg[2]) == '\0'))
			return 4;
		else if(((c = arg[1]) == 'S') && ((c = arg[2]) == '\0'))
			return 5;		
		else
			napoveda();
	}
	else
		napoveda();		
	return 0;
}

/**
 * Funkce porovnává jednotlivé znaky argumentů arg1[] a arg2[] a tak zjišťuje, jestli byly zadány argumenty určující typ funkce. Pokuď objeví argumenty které odpovídají jedné z variant přiřadí jeho návratovou hodnotu proměné porovnat2 
 */

int porovnat2(char *arg1, char *arg2)
{
	char c;
	char ch;
	if(((c = arg1[0]) == '-') && ((ch = arg2[0] == '-')))
	{
		if(((c = arg1[1]) == 's') && ((ch = arg2[1]) == 'n'))
		{
			if(((c = arg1[2]) == '\0') && ((ch = arg2[2]) == '\0'))	
				return 6;					
		}
		if(((c = arg1[1]) == 'n') && ((ch = arg2[1]) == 's'))
		{
			if(((c = arg1[2]) == '\0') && ((ch = arg2[2]) == '\0'))	
				return 7;
		}
		else
			napoveda();
	}
	else
		napoveda();
		
	return 0;
}

/**
 * Funkce převádí veškerá vstupní data do hexadecimální podoby na jeden řádek. 
 * Každému vstupnímu bajtu odpovídá dvouciferné hexadecimální číslo.
 */

void do_hexa()
{
	int c;
	while ((c = getchar()) != EOF)
	{
		printf("%.2x", c);
	}
	
	putchar('\n');
}

/**
 * Funkce  převede vstupní hodnoty z hexadecimální soustavy do desítkové soustavy a vypíše je.
 * Pokuď bude zadaný lichý počet vstupních znaků, je poslední znak v intervalu 0-15.
 */

int reverse()
{
	int c;
	int pole_hxd[2];
	int index_pole = 0;
	
	while ((c = getchar()) != EOF)
	{
		if (isblank(c) || !isprint(c)) //ignoruje bílé znaky a netisknutelne znaky
			continue;
		
		if((index_pole % 2) == 0)
		{
			if(('0' <= c) && (c <= '9'))
				pole_hxd[0] = c - '0';
			else if (('a' <= c) && (c <= 'f'))
				pole_hxd[0] = c - 'a' + 10;
			else if (('A' <= c) && (c <= 'F'))
				pole_hxd[0] = c - 'A' + 10;
			else 	//pokud je zadán znak, jehož hodnota neodpovídá hexadecimálni soustavě
			{
				fprintf(stderr,"Neplatny vstup ");			
				return EXIT_FAILURE;
			}
		}
		
		else
		{
			if(('0' <= c) && (c <= '9'))
				pole_hxd[1] = c - '0';
			else if (('a' <= c) && (c <= 'f'))
				pole_hxd[1] = c - 'a' + 10;
			else if (('A' <= c) && (c <= 'F'))
				pole_hxd[1] = c - 'A' + 10;
			else //pokud je zadán znak, jehož hodnota neodpovídá hexadecimálni soustavě
			{
				fprintf(stderr,"Neplatny vstup ");			
				return EXIT_FAILURE;
			}
							
			putchar(pole_hxd[0]*16 + pole_hxd[1]);   
		}
		
		index_pole++;
	}
		
	if(index_pole % 2 == 1) //v případě lichého počtu znaků, vytiskne poslední znak
		putchar(pole_hxd[0]);	

	return EXIT_SUCCESS;
}

/**
 *Funkce převádí vstupní data do posloupnosti řádků, kde každý řádek popisuje jednu sérii 16 bajtů,
 *každý řádek obsahuje adresu prvního bajtu v daném řádku, a tisknutelnou podobu daných bajtů. 
 *@param start určuje hodnotu, na které začne výpis funkce
 *@param max_delka určuje maximální délku vypsaného řetězce
 */

void binarni_do_textu(long int start, long int max_delka)
{
	int c;
	int i = 0;
	int j = 0;
	int stav_serie = 0;  //sleduje stav série
	int adresa = start;  //sleduje hodnotu adresy bajtu **
	int vynechat = start;	//hodnota o kterou se posune výpis prvního znaku
	int preskocit = 1;  
	char pole[16];
	int max = max_delka; //určuje maximální délku řetězce, jenž se vypíše
	int pocet = 0;  //sleduje celkový počet již vypsaných znaků

	while((c = getchar()) != EOF)
	{
		if(vynechat >= preskocit) //slouží k přeskočení hodnot < vynechat
			{
				preskocit++;
			}		
		else if (vynechat < preskocit)
		{
			if((pocet < max) || (max == 0)) //bere znaky ze vstupu dokud pocet < max, pro případ, že by nebyl zadán argument -n je argumentu N přiřazena hodnota nula, která značí, že řetězec nemá omezení pro délku výstupního řetězce
			{
				if ((stav_serie < 16) && (stav_serie != EOF))	
				{
					if ((i + vynechat) % 16 == (vynechat % 16))    //tiskne adresu bajtu
					{
						 printf("%.8x  ", adresa);
					}
					printf("%.2x ", c); 
					pole[i] = c;
		
					if(stav_serie == 7)
						putchar(' ');
					
					i++;
					stav_serie++;
					adresa++;
				}
				if(stav_serie == 16) //naplnění série, dojde k výpisu pole
				{
					putchar(' ');
					putchar('|');
					while(j < i) // výpis pole, v případě, že je znak v poli netisknutelny, je nahrazen tečkou
					{	
						if(isprint(pole[j]))
						{
							printf("%c", pole[j]);
							j++;
						}
						else
						{
							putchar('.');
							j++;
						}
					}
					
					putchar('|');
					putchar('\n');
			
					i = j = stav_serie = 0;
				}
			}				
			pocet++;
		} 
	}
	
	if((stav_serie < 16) && (stav_serie != 0)) //v případě k % 16 != 0, vypíše znaky uložené v poli a doplní sérii mezerami
	{
		while(stav_serie < 16) //dojde k doplnění série
		{
			printf("   ");
			
			if(stav_serie == 7)
				putchar(' ');
			
			stav_serie++;
		}
		
		if(stav_serie == 16) //vypíše obsah pole
		{	
			putchar(' ');
			putchar('|');
			
			while(j < i) //vypíše znaky z pole pokud je znak netisknutelny, nahradí jej tečkou
			{
				if(isprint(pole[j]))
					printf("%c", pole[j]);
				else
					putchar('.');
						
				j++;
			}
			while(j < stav_serie) //chybějící znaky v poli nahradí mezerou
			{
				putchar(' ');
				j++;
			}		
					
			putchar('|');
			putchar('\n');
		}
  }
} 
	
/**
 * Funkce vytiskne posloupnosti ze vstupu, které vypadají jako textový řetězec. 
 * Každý řetězec je vytištěn na jeden řádek. 
 * @param delka_retezce - určuje minimální velikost textového řetězce
 */
	
void binarni_posloupnost(long int delka_retezce) 
{	
	int c;
	int min = delka_retezce; //minimalni delka textového řetězce
	int i = 0;
	int j = 0;
	int pocet = 0;  //slouží k monitorování počtu znaků v řetězci
	char pole[min];
	
	while((c = getchar()) != EOF)
	{
		if(isprint(c)) //pokuď je znak tisknutelny, přičte se do aktuálního řetězce
		{	
			if(i < min)
			{
				pole[i] = c;
				
				i++;
				pocet++;
			}
			else
			{
				while(j<i)
				{	
					printf("%c",pole[j]);
					j++;						
				}
				printf("%c", c);
				pocet++;
			}
		}
		else if(i == min) 
		{	
			if(pocet == i) //vytiskne řetězec, který je dlouhý min znaků
				while(j < i)	
				{
					printf("%c",pole[j]);
					j++;
				}
					
			putchar('\n');
			i = j = pocet = 0;
		}
		else  //řetězec menší než min
			i = j = pocet = 0;
	}
	
	if(pocet == min) //pro případ, že by vstup nebyl ukončen znakem, který by značil konec řetězce a jeho délka by byla rovna min znaků
		while(j < i)	
		{
			printf("%c",pole[j]);
			j++;
		}
}

/**
 *  Nápověda se vypíše v případně zadání neplatných argumentů, které mají za úkol určit o jaký typ funkce se jedná.
 */

void napoveda()
{
	printf("Napoveda: \n1. Bez povinných argumentů - výstupní formát se skládá z posloupnosti řádků, kde každý řádek popisuje jednu sérii 16 bajtů \n2. S argumenty -s M - definuje na které adrese má začínat výpis, argument M je nezáporné číslo \n3. S argumenty -n N - definuje maximální délku vstupních bajtů ke zpracování, argument N je kladné číslo \n4. S argumenty -s M -n N - definuje, na které adrese má začínat výpis a maximální délku vstupních bajtů, argumenty mohou být zadány i v opačném pořádí\n5. S argumentem -x - veškerá vstupní databudou převedena do hexadecimální podoby na jeden řádek \n6. S argumentem -r - program očekává na vstupu hexadecimální číslice a ty převádí do binárního formátu \n7. S argumenty -S N - program tiskne textové řetězce, které jsou >= N, N udává celé číslo v intervalu 0<N<200\n");
}	
