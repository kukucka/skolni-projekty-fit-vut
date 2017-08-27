/**
 * Soubor: proj3.c
 * Datum: Prosinec 2016
 * Autor: Marek Kukučka, xkukuc04@fit.vutbr.cz
 * Projekt: Jednoduchá shluková analýza
 * Popis: Program provádí jednoduchou shlukovoou anályzu pomocí principu nejvzdálenějšího souseda.
 */

/**
 * Kostra programu pro 3. projekt IZP 2016/17
 *
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */
#include <stdio.h>
#include <stdlib.h> // strtol
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    // TODO
		c->size = 0;
		c->capacity = cap;
		c->obj = malloc(sizeof(struct obj_t) * cap);
		
		if(c->obj == NULL) //kontorla zda se povedlo alokovat paměť
		{
			c->capacity = 0;
			fprintf(stderr,"Chyba: Nepovedlo se alokovat pamet");
		}
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
		free(c->obj);

    c->size = 0;
		c->capacity = 0;
		c->obj = NULL; 
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // TODO
    if(c->size < c->capacity)
    {
    	c->obj[c->size] = obj;
    	c->size++;
    }
    else if (c->size >= c->capacity)
    {
    	resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    	c->obj[c->size] = obj;
    	c->size++;
    }
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    // TODO
    int available_space;
    int space_needed;
    int req_space = c2 -> size;
    int x = 0;
    
    available_space = c1->capacity - c1->size; //výpočet kolik je dostupného místa
    space_needed = req_space - available_space; //výpočet kolik je potřeba místa
    
    if(space_needed > 0) //pokuď je potřeba více místa než máme, poté se provede rozšíření pomocí resize
   	{	
   		resize_cluster(c1, space_needed);
		}
	
		while (x < c2 -> size) //přidává hodnoty c2 do shhluku c1
		{
   		append_cluster(c1, c2 -> obj[x]); 
			x++;    	
    }
    
    sort_cluster(c1);  //seřadí shluky v arr
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    // TODO
 
  	int x = idx;
	
		clear_cluster(&carr[x]);	    
    
    for (x = idx; x < narr - 1; x++)
    {
    	carr[x] = carr[x + 1];
		}
	 
	  return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    // TODO
    float result;
    float x = o1 -> x - o2 -> x; 
    float y = o1 -> y - o2 -> y;
        
    result = sqrtf((x *x)+(y*y)); //výsledek euklidovské rovnice
		
		return result;
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // TODO
    float distance;
    float test_distance;
    int x;
    int y;
    
    distance = obj_distance(&c1 -> obj[0], &c2 -> obj[0]);  //výpočet vzdálenosti prvního prvku sama od sebe
   
    for(x = 0; x < c1->size; x++)
    {	
    	for(y = 0; y < c2->size; y++)
    	{
    		test_distance = obj_distance(&c1 -> obj[x], &c2 -> obj[y]); //výpočet vzdáleností jednotlivých shluků

    		if(test_distance > distance) //pokuď je nová vzdálenost větší než stará, tak se nahradí
   				distance = test_distance;
   		}
   	}
   		
   	return distance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // TODO
    float distance = cluster_distance(&carr[0],&carr[1]); //výpočet vzdáleonsti mezi prvním a druhým shlukem
    float test_distance;
    int x;
    int y;
		
    for (x = 0; x < narr; x++)
    	for(y = x + 1; y < narr; y++)
    	{
    		test_distance = cluster_distance(&carr[x], &carr[y]); //výpočet vzdálenosti mezi jednotlivími shluky
    		 
    		if(distance >= test_distance) //pokud se jedná o nejmenší vzdálenost mezi dvěma shluky
    		{
    			distance = test_distance;
    			*c1 = x;
    			*c2 = y;
    		}
    	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    // TODO
	  int size;
		int id;
		int i, j;
		float x;
		float y;
		struct obj_t temp;
		
		FILE* fr = fopen(filename, "r"); //otevřeme svtupní soubor pro čtení
		
		if(fr == NULL) //kontorluje zda se povedlo otevřít soubor
		{
			fprintf(stderr, "Chyba: Nezdařilo se otevřít soubor");			 
			return 0;
		}
		
		if(fscanf(fr, "count=%d", &size) != 1) //zkontroluje jestli se načetla hodnota pro počet shluků a zjistí počet shluků
		{
			fprintf(stderr, "Chyba: Nebyla nalezena hodnota určující počet shluků");
			fclose(fr);
			return 0;
		} 

		if((size > INT_MAX)||(size <= 0))
		{
			fprintf(stderr,"Chyba: Počet shluků musí být v rozmezi (0,2147483647).");	
			fclose(fr);
			
			return 0;
		}
		
		*arr = malloc(sizeof(struct cluster_t)*size); // alokuje potřebnou paměť; *arr = ukazatel na první člen v poli o velikosti size
		
		for(i = 0; i < size; i++)
		{
			if(fscanf(fr, "%d %f %f", &id, &x, &y) == 3) //kontroluje jestli se načetli hodnoty pro id, x, y každého shluku
			{
				temp.id = id;
				temp.x = x;
				temp.y = y;

				init_cluster((*arr)+i, 1);
				append_cluster((*arr)+i, temp);
			}
			else
			{
				fprintf(stderr, "Chyba: Chybné zadání shluků v souboru");
				
				for(j = 0; j < i; j++) //uvolňuje místo, které bylo zabráno před tím než se načítání souboru přerušilo
					clear_cluster(&(*arr)[j]);
				
				free(*arr);
				*arr = NULL;
				fclose(fr);
				
				return 0;
			}
		}
			
		fclose(fr);
		 
		return size;	//vrací počet shluků
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    // TODO
    int c1, c2, i;
    char *endOfNumber;
    int num_of_clus; //do kolika shluků se mají sloučit
    int size; //počet shluků
    
    endOfNumber = NULL;
		
		if(argc == 2) // v případě, že nebude zadán počet argumentů, je předpoklad, že num_of_clas = 1
		{
			num_of_clus = 1;
			size = load_clusters(argv[1], &clusters);
			
			if(size == 0)
				return EXIT_FAILURE;
			
			while(size > num_of_clus) //pokuď je počet shluků větší než kolik je požadováno ve výstupu
			{
				find_neighbours(clusters, size, &c1, &c2);
				merge_clusters(&clusters[c1], &clusters[c2]);
				remove_cluster(clusters, size, c2);
			
				size--;
			}			
			
			print_clusters(clusters, size);
    	
    	for(i = 0; i < size; i++) //uvolňování paměti
    		clear_cluster(&clusters[i]);
    
    	free(clusters);
		}
		else if(argc == 3)
		{
			num_of_clus = strtol(argv[2],&endOfNumber, 10);  

			if(*endOfNumber != '\0')
			{
				fprintf(stderr, "Chyba: Zadána neplatná hodnota pro počet shluků [N].");

				return EXIT_FAILURE;
			}
			else if((num_of_clus > INT_MAX) || (num_of_clus < 1)) //brání přetečení
			{
				fprintf(stderr, "Chyba: Hodnota pro argument [N] musí byt v rozmezi <1,2147483647).");

				return EXIT_FAILURE;
			}
		
			else if((size = load_clusters(argv[1], &clusters)) == 0)
				return EXIT_FAILURE;
						
			else
			{
				while(size > num_of_clus) //pokuď je počet shluků větší než kolik je požadováno ve výstupu
				{
					find_neighbours(clusters, size, &c1, &c2);
					merge_clusters(&clusters[c1], &clusters[c2]);
					remove_cluster(clusters, size, c2);
				
					size--;
				}
	
  	  	print_clusters(clusters, size);

  	  	for(i = 0; i < size; i++) //uvolňování paměti
    			clear_cluster(&clusters[i]);
    
  	  	free(clusters);
  	  }
		}
		else
		{
			fprintf(stderr, "Chyba: Byly zadány neplatné argumenty. \nObecne spusteni: ./proj3 file_name [N]\nfile_name reprezentuje jmeno souboru, z ktereho nacitame a [N] reprezentuje do kolika shluku se maji jednotlive shluky sloucit");			
    	return EXIT_FAILURE;
    }
    
    return 0;    
}

