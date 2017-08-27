/**
 * @mainpage
 * Kostra hlavickoveho souboru 3. projekt IZP 2016/17
 * a pro dokumentaci Javadoc.
 * @author Marek Kukucka (xkukuc04)
 * @date Prosinec 2016
 */

/**
 * @defgroup Datove_typy Datove typy
 * Pouzite datove typy.
 * @{
 */
/**
 * Struktura objektu
 */
struct obj_t {
///identifikator promenne
    int id;
///souradnice promenne
    float x;
///souradnice promenne
    float y;
};
/**
 * Shluk objektu
 */
struct cluster_t {
    ///pocet objektu ve shluku
    int size;
    ///kapacita shluku
    int capacity;
    ///ukazatel na pole shluku
    struct obj_t *obj;
};
/**
 * @}
 */
/**
 * @defgroup Zakladni_funkce Zakladni funkce
 * Zakladni funkce pro praci se shlukem.
 * @{
 */
/**
 * Inicializuje shluk. Alokuje pamet pro kapacitu objektu.
 * @param c   ukazatel na shluk objektu
 * @param cap hodnota pro kapacitu
 * @pre 'c' nesmi byt ukazatel na NULL
 * @pre cap musi by vetsi nebo rovno 0
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * Odstrani vsechny objekty shluku a inicializuje ho na prazdny shluk
 * @param c ukazatel a shluk objektu
 */
void clear_cluster(struct cluster_t *c);

///Hodnota doporucena pro reallokaci
extern const int CLUSTER_CHUNK;
/**
 * Zmena kapacity shluku na kapacitu novou kapacitu
 * @param  c       ukazatel na shluk objektu
 * @param  new_cap hodnota pro novou kapacitu
 * @return         vraci novou hodnotu kapacity
 * @pre c->capacity musi byt >= 0
 * @pre new_cap musi byt >= 0
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * Prida objekt na konec shluku. Pokud se do nej objekt nevejde,
 * rozsiri shluk.
 * @param c   ukazatel na shluk objektu
 * @param obj hodnoty pro promenne obj_t
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 *Do shluku prida objekty z jineho shluku. Pokud bude nutne rozsiri
 *shluk. Objekty ve shluku budou serazeny vzestupne podle
 *identifikacniho cisla.
 * @param c1 ukazatel na shluk objektu
 * @param c2 ukazatel na shluk objektu
 * @pre c1 nesmi byt ukazatel na NULL
 * @pre c2 nesmi byt ukazatel na NULL
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @}
 * @defgroup Funkce_pro_praci_s_polem Funkce pro praci s polem
 * Zde jsou funkce, ktere pracuji s polem shluku
 * @{
 */
/**
 * Odstrani shluk z pole shluku
 * @param  carr pole shluku
 * @param  narr pocet polozek v poli shluku
 * @param  idx  index, na kterem se nachazi shluk pro odstraneni
 * @return      vraci novy pocet shluku v poli
 * @pre idx < narr
 * @pre narr > 0
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty (shluky)
 * @param  o1 ukazatel na strukturu objektu
 * @param  o2 ukazatel na strukturu objektu
 * @return    vraci vzdalenost mezi dvema objekty
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * Hleda nejvetsi vzdalenost mezi dvema shluku.
 * @param  c1 ukazatel na shluk objektu
 * @param  c2 ukazatel na shluk objektu
 * @return    vraci nejvetsi vzdalenost mezi dvema
 * @pre c1 nesmi byt ukazatel na NULL
 * @pre c1->size > 0
 * @pre c2 nesmi byt ukazatel na NULL
 * @pre c2->size > 0
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * Funkce najde dva nejblizsi shluky v poli shluku
 * pomoci metody nejvzdalenejsich sousedu.
 * @param carr pole shluku
 * @param narr pocet polozek v poli shluku
 * @param c1   adresa pro ukladani nalezeneho shluku
 * @param c2   adresa pro ukladani nalezeneho shluku
 * @pre narr > 0
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * Radi objekty ve shluku vzestupne podle jejich identifikatoru.
 * @param c ukazatel na shluk objektu
 * @post seradi pole shluku vzestupne podle ID
 */
void sort_cluster(struct cluster_t *c);

/**
 * Tisk shluku na stdout.
 * @param c ukazatel na shluk objektu
 */
void print_cluster(struct cluster_t *c);

/**
 * Ze souboru nacte objekty. Pro kazdy objekt vytvori shluk a
 * ulozi ho do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatele
 * na prvni polozku pole ulozi do pameti.
 * @param  filename ukazatel na soubor
 * @param  arr      ukazatel na prvni polozku pole
 * @return          vraci pocet nactenych objektu
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * Vytiskne objekty do pozadovaneho poctu shluku.
 * @param carr ukazatel na prvni shluk
 * @param narr pocet tisknutych shluku
 * @pre 'filename' musi existovat
 * @post  ulozi nactene objekty do pole
 */
void print_clusters(struct cluster_t *carr, int narr);

/**
 * @}
 */
