/**
 * Soubor: proj2.c
 * Datum: Listopad 2016
 * Autor: Marek Kukučka, xkukuc04@fit.vutbr.cz
 * Projekt: Iterační výpočty
 * Popis: Program vypočítává přirozený logaritmus a exponenciální funkci ze zadaných argumentů
 */


#include <stdio.h>
#include <stdlib.h> //strtol, strtod
#include <string.h> //strcmp
#include <math.h> //log(x), exp(x)
#include <limits.h> //UINT_MAX
#include <float.h> //DBL_MAX

double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);
double taylor_pow(double x, double y, unsigned int n);
double taylorcf_pow(double x, double y, unsigned int n);

/**
 * Ve funkci main dochází k porovnání argumentů programu a k následnému výpisu
 * v závislosti na počtu argumentů programu se zjišťuje jestli se může jednat o funkci, která výpíše logaritmus nebo exponenciální funkci
 * pokud jsou zadány nesprávné argumenty dojde k výpisu chyby
 * pro získání číselné hodnoty z argumentu používáme funkce strtol a strtod
 */

int main(int argc, char *argv[])
{
	double x; //hodnota pro logaritmus
	double y; //hodnota pro x v exponenciální funkci
	long int iter; //číslo, které nám určí počet iterací, přetypujeme ho na unsigned int n
	unsigned int n;	//pocet iteraci

	//hodnoty pro strtol a strtod
	char *stringDoubleX;
	char *stringDoubleY;
	char *stringLongInt;
	char *endOfNumberN;
	char *endOfNumberX;
	char *endOfNumberY;
	
	endOfNumberN = NULL;
	endOfNumberX = NULL;
	endOfNumberY = NULL;
	
	stringDoubleX = argv[2];
	stringLongInt = argv[3];
	stringDoubleY = argv[3]; //pro exponencíálni funkci
	
	if (argc == 4)
	{
		if (strcmp(argv[1], "--log") == 0)
		{												
			iter = strtol(stringLongInt, &endOfNumberN, 10);
			x = strtod(stringDoubleX, &endOfNumberX);	
			n = iter;  //přetypování long int na unsigned int
			
			if (*endOfNumberN != '\0' || *endOfNumberX != '\0') //kontroluje jestli jsou argumenty argv[2] a argv[3] čísla
			{
				fprintf(stderr, "Invalid value of variable or iteration.\n");
				return EXIT_FAILURE;
			}
			else if (iter <= 0)
			{
				fprintf(stderr,"Number of iterations must be greater then 0.\n"); //kontroluje jestli je hodnota pro iterace větší než 0 
				return EXIT_FAILURE;
			}
			else if (iter > UINT_MAX) //brání přetečení hodnoty pro iterace
			{
				fprintf(stderr, "ERROR: Overflow - for iterations insert number smaller than 4 294 967 295.\n");
				return EXIT_FAILURE;
			}
			else
			{			
				printf("       log(%g) = %.12g\n", x, log(x));
				printf("    cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
				printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
			}
		}
		else
		{
			fprintf(stderr,"Incorrect arguments, please try again.\n");	//pokud budou zadány neplatné argumenty
			return EXIT_FAILURE;
		}
	}
	else if (argc == 5)
	{			
		if (strcmp(argv[1], "--pow") == 0)
		{
			stringLongInt = argv[4]; //v případě exponenciální funkce je počet iterací až na 4 místě
			
			iter = strtol(stringLongInt, &endOfNumberN, 10);
			x = strtod(stringDoubleX, &endOfNumberX);
			y = strtod(stringDoubleY, &endOfNumberY);	
			n = iter;  //přetypování long int na unsigned int
			
			if (*endOfNumberN != '\0' || *endOfNumberX != '\0' || *endOfNumberY != '\0') //kontroluje jestli jsou argumenty argv[2], argv[3] a argv[4] čísla
			{
				fprintf(stderr, "Invalid value of variable or iteration.\n");
				return EXIT_FAILURE;
			}
			else if (iter <= 0)
			{			
				fprintf(stderr,"Number of iterations must be greater then 0.\n"); //kontroluje jestli je hodnota pro iterace větší než 0 
				return EXIT_FAILURE;
			}
			else if (iter > UINT_MAX) //brání přetečení hodnoty pro iterace
			{
				fprintf(stderr, "ERROR: Overflow - for iterations insert number smaller than 4 294 967 296.\n");
				return EXIT_FAILURE;
			}
			else
			{
				printf("         pow(%g,%g) = %.12g\n", x, y,	pow(x, y));
				printf("  taylor_pow(%g,%g) = %.12g\n", x, y, taylor_pow(x, y, n));
				printf("taylorcf_pow(%g,%g) = %.12g\n", x, y,	taylorcf_pow(x, y, n));
			}
		}
		else //pokud budou zadány neplatné argumenty
		{
			fprintf(stderr,"Incorrect arguments, please try again.\n"); 
			return EXIT_FAILURE; 
		}
	}
	else //pokud budou zadány neplatné argumenty
	{
		fprintf(stderr,"Incorrect arguments, please try again.\n");
		return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}

/**
 * Funkce počítá logaritmus implementací vzorce pro zřetězené zlomky
 * @param x určuje hodnotu, z které se vypočítá logaritmus
 * @param n určuje počet iterací
 */

double cfrac_log(double x, unsigned int n)
{
	double dividend = 2*(x-1)/(x+1);  //je to dělenec pro funkci zřetězených zlomků
	double component = (x-1)/(x+1)*(x-1)/(x+1);  
	double result;  //uloží se do ní výsledek, který poté vrací hodnotu funkce přes return
	double frac = 1; //počáteční hodnota dělitele pro funkci zřetězených zlomků

//definiční obor pro hodnotu x
	if (x == 0)  
		result = -INFINITY;
	else if (x < 0)
		result = NAN;
	else if (x == INFINITY) 
		result = INFINITY;
	else if(x > 0)
	{
		for(;n;n--) 
		{	
			frac = (1+(2*n-2)) - (n*n*component) / frac; //výpočet dělitele pro vzorec zřetězených zlomků
		}	
		
		result =  dividend / frac; //výsledná hodnota logaritmu
	}

		
	return result; //funkce vrací výslednou hodnotu logaritmu
}

/**
 * Funkce počítá logaritmus implementací Taylorova polynomu,
 * v této funkci implementujeme dva vzorce pro výpočet logaritmu 
 * je zde použita mezní hodnota 1, jelikož jeden ze vzorců počítá hodnotu logaritmu v intervalu (0,2) a druhý v intervalu (1/2, +inf)
 * @param x určuje hodnotu, z které se vypočítá logaritmus
 * @param n určuje počet iterací
 */

double taylor_log(double x, unsigned int n) 
{
	double value_of_x  = 1 - x;  //hodnota pro x v případě, že x < 1
	double sum = -value_of_x;  //počáteční hodnota pro x < 1 && x > 0 
	double sum2 = 0;  //počáteční hodnota pro x >= 1
	double add = value_of_x*value_of_x; //hodnota, která se přidává při iteraci k sum
	double add2 = ((x - 1)/x); //hodnota, která se přidává při iteraci k sum2
	double result; //uloží se do ní výsledek, který poté vrací hodnotu funkce přes return
	unsigned int divisor = 2; //počáteční hodnota dělitele x > 0 && x < 1
	unsigned int divisor2 = 1; //počáteční hodnota dělitele pro x >= 1

//definiční obor pro hodnotu x
  if (x == 0)
		result = -INFINITY;
	else if (x < 0)
		result = NAN;
	else if (x == INFINITY) 
		result = INFINITY;
	else if ((x < 1) && (x > 0)) //výpočet logaritmu pro x (0,1)
	{
		for (;n;n--) //výpočet logaritmu v závislosti na počtu iterací
		{
			sum -= add/divisor; 
			add *= value_of_x;
			
			divisor++; 
		}
		
		result = sum; 
	}	
	else if (x >= 1)  //výpočet logaritmu pro x > 1
	{
		for(;n;n--) //výpočet logaritmu v závislosti na počtu iterací
		{
			sum2 += add2/divisor2;
			add2 *=((x - 1)/x);
			
			divisor2++;
		}	
		
		result = sum2;
	}
	
	return result; //funkce vrací hodnotu logaritmu
}

/**
 * V této funkci implementujeme vzorec pro výpočet exponenciální funkce (1 + x*ln(a)/1! + ...)
 * používáme hodnotu přirozeného logaritmu vypočtenou pomocí taylorova polynomu
 * @param x určuje hodnotu, z které se vypočítá logaritmus ln(a)
 * @param y určuje hodnotu pro x v exponenciální funkci
 * @param n určuje počet iterací
 */

double taylor_pow(double x, double y, unsigned int n)
{
	double value_taylor_log = taylor_log(x, n); //hodnota logaritmu
	unsigned int factorial = 2; //počáteční hodnota pro faktorial
	double result = 1;  //počáteční hodnota result, return poté vrací výslednou hodnotu
	double add = y * taylor_log(x, n);  ///hodnota, která se přidává při iteraci k result
  
//definiční obor pro hodnoty x a y
  if (y == -INFINITY)
  	result = 0;
  else if (y == 0)
  	result = 1;
	else if (x == 0) 
		result = -INFINITY;
	else if (x < 0 )
		result = NAN;	 
	else if (x == INFINITY)
		result = INFINITY;
	else if (x > 0)
		for(;n;n--) //výpočet exponenciální funkce v závisloti na počtu iterací
		{
			result += add;
			add *= y * value_taylor_log / factorial;
		
			factorial++;
		}
	
	return result; //funkce vrací hodnotu exponenciální funkce
}

/**
 * V této funkci implementujeme vzorec pro výpočet exponenciální funkce (1 + x*ln(a)/1! + ...)
 * používáme hodnotu přirozeného logaritmu vypočtenou pomocí zřetězených zlomků
 * @param x určuje hodnotu, z které se vypočítá logaritmus ln(a)
 * @param y určuje hodnotu pro x v exponenciální funkci
 * @param n určuje počet iterací
 */


double taylorcf_pow(double x, double y, unsigned int n)
{
	double value_cfrac_log = cfrac_log(x, n);  //hodnota logaritmu
	unsigned int factorial = 2;  //počáteční hodnota pro faktoriál
	double result = 1;  //počáteční hodnota result, return poté vrací výslednou hodnotu
	double add = y * cfrac_log(x, n);  ///hodnota, která se přidává při iteraci k result

 //definiční obor pro hodnoty x a y
  if (y == -INFINITY)
  	result = 0;
  else if (y == 0)
  	result = 1;
	else if (x == 0)
		result = -INFINITY;
	else if (x < 0 )
		result = NAN;	
	else if (x == INFINITY || y == INFINITY)
		result = INFINITY; 	
 	else if (x > 0)
		for(;n;n--) //výpočet exponenciální funkce v závisloti na počtu iterací
		{
			result += add;
			add *= y * value_cfrac_log / factorial;
		
			factorial++;
		}
			
	return result; //funkce vrací hodnotu exponenciální funkce
}


