/**
 * Soubor: proj2.c
 * Datum: Duben 2017
 * Autor: Marek Kukucka, xkukuc04@stud.fit.vutbr.cz
 * Projekt: 2.Uloha
 * Popis: Program implementujte reseni synchronizacniho problemu Child Care
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>


//definice maximalni hodnoty pro awg,cwg,agt a ctg
#define MAX_TIME 5001


//prototypu funkci
int resources();
void freeResources();
void adultGenerator(int awt);
void childGenerator(int cwt);

//inicializace ukazatelu na sdilenou pamet
int *shm_processNum = NULL,*shm_adultNum = NULL, *shm_childNUM = NULL, *shm_currentNumOfAdult = NULL, *shm_currentNumOfChild = NULL, *shm_allProcesses = NULL, *shm_adultLeft;
//inicializace pro navratove funkce shmget
int shmProcID = 0,shmAdultID = 0, shmChildID = 0, shmCurrentNumOfAdultID = 0, shmCurrentNumOfChildID = 0, shmAllProcessesID = 0, shmAdultLeft = 0;
//inicializuje ukazetele na semafory
sem_t *sem_surplex,*sem_child_wait, *sem_finish,*sem_child_block, *sem_adult_block, *sem_counter;
//inicializace ukazetele na soubor
FILE *document = NULL;

/**
* Ve funkci se otevre soubr proj2.out pro zapis dale
* se zpracovavaji argumenty, pote se provede nacteni
* zdroju(sdilene pameti a semaforu).
* Pote se pomoci fork() vytvori dalsi dva procesy, ktere
* jsou zodpovedne za generovani procesu Child a Adult.
* Nakonec vyuzijeme waitpid abychom pockali az skonci
* vsechny procesy a uvolnime zdroje
*/

int main(int argc, char const *argv[])
{
  //otevreni souboru a kontrola zda byl soubor korektne otevren
  if((document = fopen("proj2.out", "w+")) == NULL)
  {
    fprintf(stderr, "Nezdarilo se otevrit soubor proj2.out.\n");
    freeResources();
    exit(2);
  }
  //nastaveni bufferu
  setbuf(document, NULL);
  //nastaveni generatoru pseudonahodnych cisel
  srand(time(0));

  long adult, child, agt, cgt, awt, cwt;
  char *endptr[6];
  errno = 0;

  if(argc == 7)
  {
    adult = strtol(argv[1],&endptr[0],10);
    child = strtol(argv[2],&endptr[1],10);
    agt = strtol(argv[3],&endptr[2],10);
    cgt = strtol(argv[4],&endptr[3],10);
    awt = strtol(argv[5],&endptr[4],10);
    cwt = strtol(argv[6],&endptr[5],10);

//Zde dochazi ke kontrole, ze jako argumenty byly zadany platne hodnoty
//V pripade zadani neplatnych argumentu, dojde k preruseni a vypisu chyboveho hlaseni
    if(adult <= 0 || child <= 0 || agt < 0 || agt >= MAX_TIME || cgt < 0 || cgt >= MAX_TIME || awt < 0 || awt >= MAX_TIME || cwt < 0 || cwt >= MAX_TIME)
    {
      fprintf(stderr, "Nejaky argument je spatne zadan\n");
      return 1;
    }
    for (int i = 0; i < 6; i++)
    {
      if(endptr[i] == argv[i + 1])
      {
        fprintf(stderr, "%d.argument %s neni cislo.\n", i+1, argv[i+1]);
        freeResources();
        exit(1);
      }
    }
    if (errno != 0 && errno == ERANGE)
    {
      perror("strtol");
      freeResources();
      exit(1);
    }
  }
  else
  {
    fprintf(stderr, "Spatny pocet argumentu\n");
    freeResources();
    exit(1);
  }
//kontrola spravneho alokovani pameti a semaforu
  int resource_check = resources();
  if(resource_check == 1)
  {
    fprintf(stderr, "Nezdarilo se alokovat pamet!\n");
    freeResources();
    exit(2);
  }
  else if(resource_check == 2)
  {
    fprintf(stderr, "Nezdarilo se vytvorit semafory!\n" );
    freeResources();
    exit(2);
  }
 //deklarace promennych
  pid_t pidAdult, pidChild;
  pid_t pid_arrayAdult[adult],pid_arrayChild[child];
  int wait_to_gen_adult = 0;
  int wait_to_gen_child = 0;
  *shm_allProcesses = adult + child;
  *shm_adultLeft = adult;
  // provede se fork, pokud by selhal vypise se zprava a proces se ukonci
  if((pidAdult = fork()) < 0)
  {
    perror("fork");
    freeResources();
    exit(2);
  }
  //v teto vetvi mame proces zodpovedny za generovani procesu Adult
  //pomoci cyklu for je vygenerovany pocet procesu Adultu, zadany argumentem
  //v pripade ze argument agt != 0, dojde k uspani generovani procesu na nahodnou
  //hodnotu v rozmezi (0 az agt) +1
  if(pidAdult == 0)
  {
    for(int i = 0; i < adult; i++)
    {

      if((pidAdult = fork()) < 0)
      {
        perror("fork");
        freeResources();
        exit(2);
      }

      if(pidAdult == 0)
      {
        if(agt > 0)
        {
          wait_to_gen_adult = (rand() % agt) + 1;
        }
        //usleep uspava v mikrosekundach proto je zapotrebi jej vynasobit 1000
        //abychom ziskali mikrosekundy
        usleep(wait_to_gen_adult * 1000);
        adultGenerator(awt);
      }
      else if(pidAdult > 0)
      {
        pid_arrayAdult[i] = pidAdult;
      }
      else
      {
        fprintf(stderr, "Nezdarilo se vytvorit proces Adult\n");
        freeResources();
        exit(2);
      }
    }
  }
  //v teto vetvi mame proces zodpovedny za generovani procesu Child
  //pomoci cyklu for je vygenerovany pocet procesu Child, zadany argumentem
  //v pripade ze argument cgt != 0, dojde k uspani generovani procesu na nahodnou
  //hodnotu v rozmezi (0 az cgt) +1
  else if (pidAdult > 0)
  {
    pidChild = pidAdult;
    for(int i = 0; i < child; i++)
    {
      if((pidChild = fork()) < 0)
      {
        perror("fork");
        freeResources();
        exit(2);
      }
      if(cgt > 0)
      {
        wait_to_gen_child = (rand() % cgt) + 1;
      }
      else
      {
        wait_to_gen_child = 0;
      }
      usleep(wait_to_gen_child * 1000);
      if(pidChild == 0)
      {
        //usleep uspava v mikrosekundach proto * 1000 abychom dostali ms
        childGenerator(cwt);
      }
      else if (pidChild > 0)
      {
        pid_arrayChild[i] = pidChild;
      }
      else
      {
        fprintf(stderr, "Nezdarilo se vytvorit proces Child\n");
        freeResources();
        exit(2);
      }
    }
  }
  //cekani dokud nedobehnout vsechny procesy
  for(int i = 0; i < child; i++)
  {
    waitpid(pid_arrayChild[i], NULL, 0);
  }
  for(int i = 0; i < adult; i++)
  {
    waitpid(pid_arrayAdult[i], NULL, 0);
  }

  freeResources();
  return 0;
}

/**
 * V teto funkci probiha vytvoreni sdielene pameti a vytvoreni a inicializace semaforu
 */
int resources()
{

  //Inicializace sdielene pameti
  //shm_processNum - cislo aktualni operace
  //shm_adultNum - slouzi k urceni toho o jakeho Adult se jedna
  //shm_childNUM - slouzi k urceni toho o jake Child se jedna
  //shm_currentNumOfAdult - pocet Adult pritomnych v centru
  //shm_currentNumOfChild - pocet Child pritomnych v centru
  //shm_allProcesses - je v nem ulozen pocet procesu, ktere se maji vykonat a
  //                   po vykovani procesu (po akci leave) se snizi o jedna
  //shm_adultLeft - udava pocet procesu Adult, kteri jeste neprosli akci leave
  int check = 0;
  if(check == 0)
  {
    if((shmProcID = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_processNum = (int *) shmat(shmProcID, NULL, 0)) == NULL)
    {check = 1;}

    if((shmAdultID = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_adultNum = (int *) shmat(shmAdultID, NULL, 0)) == NULL)
    {check = 1;}

    if((shmChildID = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_childNUM = (int *) shmat(shmChildID, NULL, 0)) == NULL)
    {check = 1;}

    if((shmCurrentNumOfAdultID = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_currentNumOfAdult = (int *) shmat(shmCurrentNumOfAdultID, NULL, 0)) == NULL)
    {check = 1;}

    if((shmCurrentNumOfChildID = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_currentNumOfChild = (int *) shmat(shmCurrentNumOfChildID, NULL, 0)) == NULL)
    {check = 1;}

    if((shmAllProcessesID = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_allProcesses = (int *) shmat(shmAllProcessesID, NULL, 0)) == NULL)
    {check = 1;}

    if((shmAdultLeft = shmget(IPC_PRIVATE,sizeof(int), IPC_CREAT | IPC_EXCL | 0666)) == -1)
    {check = 1;}
    if((shm_adultLeft = (int *) shmat(shmAdultLeft, NULL, 0)) == NULL)
    {check = 1;}
  }

  //Zde se vytvari semafory
  //sem_surplex - vyuziva se pri synchronizaci akci trying to leave, leave a waiting u Adult
  //              a pro akci enter, waiting, trying to leave a leave u Child
  //sem_child_wait - vyuziva se k pri osetrovani, toho aby nedochazelo k poruseni podminek pro enter
  //                 u Child a pro leave u Adult
  //sem_finish - zajisti aby doslo ke vsem vypisum finished az pote co vsechny procesy provedou akci leave
  //sem_child_block - zajistuje aby vstupujici Child neporusoval podminky vstupu
  //sem_adult_block - zajistuje aby nedoslo k vypisu leave u Adult a nebo enter u Child, dokud nebudou
  //                  splneny podminky, ktere tyto akce umozni
  //sem_counter - zajistuje, aby pokazde kdyt se provede akce, se tez provedly vsechny aritmeticke operace
  //              s ni vazane
  if((sem_surplex = mmap(NULL,sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
  {check = 2;}
  if((sem_child_wait = mmap(NULL,sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
  {check = 2;}
  if((sem_finish = mmap(NULL,sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
  {check = 2;}
  if((sem_child_block = mmap(NULL,sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
  {check = 2;}
  if((sem_adult_block = mmap(NULL,sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
  {check = 2;}
  if((sem_counter = mmap(NULL, sizeof(sem_t),PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0)) == MAP_FAILED)
  {check = 2;}

  //Zde probiha inicializace semaforu
  if(check == 0)
  {
    if(sem_init(sem_surplex, 1, 1) == -1)
    {check = 2;}
    if(sem_init(sem_child_wait, 1, 1) == -1)
    {check = 2;}
    if(sem_init(sem_finish, 1, 0) == -1)
    {check = 2;}
    if(sem_init(sem_child_block, 1, 1) == -1)
    {check = 2;}
    if(sem_init(sem_adult_block, 1, 1) == -1)
    {check = 2;}
    if(sem_init(sem_counter, 1, 1) == -1)
    {check = 2;}
  }

  return check;
}


/**
 * Uvolnuje sdilenou pamet, nici semafory a uzavira soubor;
 */
void freeResources()
{
  int check = 0;
  //Uvolneni alokovane pameti
  if(shmctl(shmProcID, IPC_RMID, NULL) == -1)
  {check = 1;}
  if(shmctl(shmAdultID, IPC_RMID, NULL) == -1)
  {check = 1;}
  if(shmctl(shmChildID, IPC_RMID, NULL) == -1)
  {check = 1;}
  if(shmctl(shmCurrentNumOfAdultID, IPC_RMID, NULL) == -1)
  {check = 1;}
  if(shmctl(shmCurrentNumOfChildID, IPC_RMID, NULL) == -1)
  {check = 1;}
  if(shmctl(shmAllProcessesID, IPC_RMID, NULL) == -1)
  {check = 1;}
  if(shmctl(shmAdultLeft, IPC_RMID, NULL) == -1)
  {check = 1;}

  if(check == 1)
  {
    fprintf(stderr, "Uvolneni pameti se nezdarilo\n");
  }
  //Zniceni semaforu
  if(sem_destroy(sem_surplex) == -1)
  {check = 2;}
  if(sem_destroy(sem_adult_block) == -1)
  {check = 2;}
  if(sem_destroy(sem_finish) == -1)
  {check = 2;}
  if(sem_destroy(sem_child_wait) == -1)
  {check = 2;}
  if(sem_destroy(sem_child_block) == -1)
  {check = 2;}
  if(sem_destroy(sem_counter) == -1)
  {check = 2;}

  if(check == 2)
  {
    fprintf(stderr, "Nezdarilo se zniceni semaforu >_<' !\n");
  }
  //Uzavreni souboru
  if(fclose(document) == EOF)
  {check = 3;}

  if(check == 3)
  {
    fprintf(stderr, "Nezdarilo se zavrit dokument >_<' !\n");
  }
}

/**
 * @param awt horni hranice pro generovani casu k uspani(na simulovani prace)
 * teto funkci dochazi k tisku nekolika hlaseni, jejichz vypis je
 * usmernovan pomoci semaforu a sdilene pameti.
 * Pred kazdym vypisem se zvysi hodnota *shm_processNum o jedna (tato
 * hodnota reprezentuje cislo vypisu)
 */
void adultGenerator(int awt)
{
  //nastaveni generatoru pseudonahodnych cisel
  srand(time(0));
  int working = 0;
  //hodnota pritomneho procesu adult se zvysi o jedna a preda se do promenne currentAdult
  sem_wait(sem_counter);
  *shm_adultNum += 1;
  int currentAdult = *shm_adultNum;
  fprintf(document, "%d : A %d : started\n", *shm_processNum += 1, currentAdult);
  fflush(document);
  sem_post(sem_counter);
  //hodnota reprezentujici pocet dospelich v centru (*shm_currentNumOfAdult) se zvysi o jedna
  sem_wait(sem_surplex);
  sem_wait(sem_adult_block);
  sem_wait(sem_counter);
  fprintf(document, "%d : A %d : enter\n", *shm_processNum += 1, currentAdult);
  *shm_currentNumOfAdult += 1;
  fflush(document);
  sem_post(sem_counter);
  sem_post(sem_adult_block);
  sem_post(sem_surplex);
  //pokud je awt > 0 uspi proces, simuluje tim praci
  if(awt > 0)
  {
    working = (rand() % awt) + 1;
  }
  usleep(working * 1000);

  sem_wait(sem_surplex);
  sem_wait(sem_adult_block);
  sem_wait(sem_counter);
  fprintf(document, "%d : A %d : trying to leave\n", *shm_processNum += 1, currentAdult);
  fflush(document);
  sem_post(sem_counter);
  //podminka kontorluje zda muze byt vypsano leave a nebo zda bude vypsano waiting
  sem_wait(sem_counter);
  if(*shm_currentNumOfChild > 0)
  {
    if (((*shm_currentNumOfChild <= *shm_currentNumOfAdult*3) && (*shm_currentNumOfChild > 3*(*shm_currentNumOfAdult - 1)))||(*shm_currentNumOfChild >= *shm_currentNumOfAdult*3)||(*shm_currentNumOfAdult == 1 && *shm_currentNumOfChild > 0))
    {
      fprintf(document, "%d : A %d : waiting : %d : %d\n",*shm_processNum += 1, currentAdult, *shm_currentNumOfAdult, *shm_currentNumOfChild);
      fflush(document);
      sem_post(sem_counter);
      sem_post(sem_adult_block);
      sem_post(sem_surplex);
      //zde se vzdy po sepnuti semaforu kontroluje zda je splne podminka,
      //ktera by umoznovala prechod na vypis leave
      while(1)
      {
          sem_wait(sem_child_wait);
          if(!(((*shm_currentNumOfChild <= *shm_currentNumOfAdult*3) && (*shm_currentNumOfChild > 3*(*shm_currentNumOfAdult - 1))) || (*shm_currentNumOfChild >= *shm_currentNumOfAdult*3) || (*shm_currentNumOfAdult == 1 && *shm_currentNumOfChild > 0)))
          {
            sem_wait(sem_adult_block);
            sem_wait(sem_counter);
            break;
          }
          sem_post(sem_child_wait);
      }
      //provede se vypis leave, pocet pritomnych Adult v centru(*shm_currentNumOfAdult) se
      //snizi o jedna, pote se snizi celkovy pocet procesu(*shm_allProcesses) o jedna
      //a nakonec se snizi pocet Adultu, kteri jeste neprosli pres vypis leave(*shm_adultLeft) o jedna
      fprintf(document, "%d : A %d : leave\n", *shm_processNum += 1, currentAdult);
      *shm_currentNumOfAdult -= 1;
      *shm_allProcesses -= 1;
      *shm_adultLeft -= 1;
      fflush(document);
      sem_post(sem_counter);
      sem_post(sem_adult_block);
      sem_post(sem_child_wait);

    }
    else
    {

      fprintf(document, "%d : A %d : leave\n", *shm_processNum += 1, currentAdult);
      *shm_currentNumOfAdult -= 1;
      *shm_allProcesses -= 1;
      *shm_adultLeft -= 1;
      fflush(document);
      sem_post(sem_counter);
      sem_post(sem_adult_block);
      sem_post(sem_surplex);
    }
  }
  else
  {
    fprintf(document, "%d : A %d : leave\n", *shm_processNum += 1, currentAdult);
    *shm_currentNumOfAdult -= 1;
    *shm_allProcesses -= 1;
    *shm_adultLeft -= 1;
    fflush(document);
    sem_post(sem_counter);
    sem_post(sem_adult_block);
    sem_post(sem_surplex);
  }
  //pokud je pocet vsech procesu nula, sepne se semafor a vypise se finished
  if(*shm_allProcesses == 0)
  {
    sem_post(sem_finish);
  }
  sem_wait(sem_finish);
  fprintf(document, "%d : A %d : finished\n",*shm_processNum += 1, currentAdult);
  sem_post(sem_finish);

  exit(0);
}

/**
 * @param cwt horni hranice pro generovani casu k uspani(na simulovani prace)
 * teto funkci dochazi k tisku nekolika hlaseni, jejichz vypis je
 * usmernovan pomoci semaforu a sdilene pameti.
 * Pred kazdym vypisem se zvysi hodnota *shm_processNum o jedna (tato
 * hodnota reprezentuje cislo vypisu)
 */

void childGenerator(int cwt)
{
  //nastaveni generatoru pseudonahodnych cisel
  srand(time(0));
  int working = 0;
  //hodnota pritomneho procesu child se zvysi o jedna a preda se do promenne currentChild
  sem_wait(sem_counter);
  *shm_childNUM += 1;
  int currentChild = *shm_childNUM;
  fprintf(document, "%d : C %d : started\n", *shm_processNum += 1, currentChild);
  fflush(document);
  sem_post(sem_counter);
  //zde dochazi ke kontrole zda muze probehnout vypis enter a nebo zda
  //probehne vypis waiting
  sem_wait(sem_child_wait);
  sem_wait(sem_surplex);
  sem_wait(sem_adult_block);
  sem_wait(sem_child_block);
  sem_wait(sem_counter);
  if(*shm_adultLeft == 0)
  {
    //po vypisu se zvedne pocet deti centru(*shm_currentNumOfChild) o jedna
    fprintf(document, "%d : C %d : enter\n", *shm_processNum += 1, currentChild);
    *shm_currentNumOfChild += 1;
    fflush(document);
    sem_post(sem_counter);
    sem_post(sem_child_block);
    sem_post(sem_adult_block);
    sem_post(sem_surplex);
    sem_post(sem_child_wait);
    //pokud je cwt > 0 uspi proces, simuluje tim praci
    if(cwt > 0)
    {
      working = (rand() % cwt) + 1;
    }
    usleep(working * 1000);

  }
  else
  {
    //kontrola podminky zda se ma vypsat waiting, pokud selze vypise se enter
    if((((*shm_currentNumOfAdult*3 <= *shm_currentNumOfChild) && *shm_adultLeft > 0) || ((*shm_adultLeft > 0 && *shm_currentNumOfAdult == 0))))
    {
      fprintf(document, "%d : C %d : waiting : %d : %d\n",*shm_processNum += 1, currentChild, *shm_currentNumOfAdult, *shm_currentNumOfChild);
      sem_post(sem_counter);
      sem_post(sem_child_block);
      sem_post(sem_adult_block);
      sem_post(sem_surplex);
      sem_post(sem_child_wait);
      //zde se vzdy po sepnuti semaforu kontroluje zda je splne podminka,
      //ktera by umoznovala prechod na vypis enter
      while(1)
      {
        sem_wait(sem_child_wait);
        if((!((*shm_currentNumOfAdult*3 <= *shm_currentNumOfChild) && *shm_adultLeft > 0) && !((*shm_adultLeft > 0 && *shm_currentNumOfAdult == 0))) || *shm_adultLeft == 0)
        {
          sem_wait(sem_surplex);
          sem_wait(sem_adult_block);
          sem_wait(sem_child_block);
          sem_wait(sem_counter);
          break;
        }
        sem_post(sem_child_wait);
      }
      fprintf(document, "%d : C %d : enter\n", *shm_processNum += 1, currentChild);
      *shm_currentNumOfChild += 1;
      fflush(document);
      sem_post(sem_counter);
      sem_post(sem_child_block);
      sem_post(sem_adult_block);
      sem_post(sem_surplex);
      sem_post(sem_child_wait);

      if(cwt > 0)
      {
        working = (rand() % cwt) + 1;
      }
      usleep(working * 1000);
    }
    else
    {
      fprintf(document, "%d : C %d : enter\n", *shm_processNum += 1, currentChild);
      *shm_currentNumOfChild += 1;
      fflush(document);
      sem_post(sem_counter);
      sem_post(sem_child_block);
      sem_post(sem_adult_block);
      sem_post(sem_surplex);
      sem_post(sem_child_wait);

      if(cwt > 0)
      {
        working = (rand() % cwt) + 1;
      }
      usleep(working * 1000);
    }
  }

  sem_wait(sem_surplex);
  sem_wait(sem_counter);
  fprintf(document, "%d : C %d : trying to leave\n", *shm_processNum += 1, currentChild);
  fflush(document);
  sem_post(sem_counter);

  //po vypisu leave se snizi pocet deti v centru(*shm_currentNumOfChild) o jedna a
  // pote se snizi o jedna celkovy pocet procesu(*shm_allProcesses)
  sem_wait(sem_counter);
  fprintf(document, "%d : C %d : leave\n", *shm_processNum += 1, currentChild);
  *shm_currentNumOfChild -= 1;
  *shm_allProcesses -= 1;
  fflush(document);
  sem_post(sem_counter);
  sem_post(sem_surplex);
  //pokud je pocet vsech procesu nula, sepne se semafor a vypise se finished
  if(*shm_allProcesses == 0)
  {
    sem_post(sem_finish);
  }
  sem_wait(sem_finish);
  fprintf(document, "%d : C %d : finished\n",*shm_processNum += 1, currentChild);
  sem_post(sem_finish);


  exit(0);
}
