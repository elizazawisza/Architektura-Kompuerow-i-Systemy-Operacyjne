#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LSH_ROZMIAR 1024
#define LSH_OGRANICZENIA_TEKSTU " \t\r\n"

/*
	Nazwy wbudowanych funkcji
*/
char* lsh_wbudowane_nazwy[]={"cd", "exit"};


int lsh_wbudowane_funkcje_rozmiar = (sizeof(lsh_wbudowane_nazwy) / sizeof(char*));


/*
	implementacja wbudowanej funkcji zmiany katalogu
	@param args Lista args, gdzie args[0] = 'cd', args[1] = 'nazwa katalogu'
*/
int lsh_wbudowane_cd(char** args){
	if(args[1]==NULL)
		fprintf(stderr, "lsh: cd potrzebuje parametr, by zmienić katalog \n");
	else{
		if(chdir(args[1])!=0)
			perror("lsh");
	}
	return 1;
}

/*
	implementacja wbudowanej funkcji zamykania lsh
	@param args Lista args 
*/
int lsh_wbudowane_wyjscie(char** args){
	exit(EXIT_SUCCESS);
}

/*
	Tablica wbudowanych funkcji
*/

int (*lsh_wbudowane_funkcje[])(char **) = {
  &lsh_wbudowane_cd,
  &lsh_wbudowane_wyjscie
};


/*
  zabezpiecznie przed zakończeniem działania programu spowodowanym 
  wywołaniem sygnału SIGINT
*/
void ctrl_C_handler(int dummy){
	char location[1024];
	getcwd(location, 1024);
	printf("\n");
	printf("\033[1;38;5;198meliza@lsh\033[0m:\033[1;38;5;51m%s\033[0m$ ", location);

	fflush(STDIN_FILENO);
}



char** lsh_dzielimy(char* linia, char* ograniczniki){
	int rozmiar = LSH_ROZMIAR;
	char * ciag;
	char ** tymczasowa_komenda;
	char ** cala_komenda = malloc(sizeof(char*) * rozmiar); //zajmujemy miejsce w pamięci


	if(!cala_komenda){
		perror("lsh:blad w zapisywaniu wielkosci");
    exit(EXIT_FAILURE);
	}

	ciag = strtok(linia, ograniczniki);
	int pozycja = 0;
	while(ciag != NULL){
		cala_komenda[pozycja] = ciag;
		pozycja++;

		if(pozycja>=rozmiar){ //gdy mamy za dużo
			tymczasowa_komenda = cala_komenda;
			rozmiar += LSH_ROZMIAR; //zmiana wielkości zajmowanego miejsca w pamięci
			cala_komenda = realloc(cala_komenda, sizeof(char*) * rozmiar); 
			if(!cala_komenda) {
        perror("lsh:blad w zmianie wielkosci");
        free(tymczasowa_komenda);
        exit(EXIT_FAILURE);
      }		
		}
		ciag = strtok(NULL, ograniczniki);	
	}
	//dodawanie pustego znaku na koncu komendy 
	cala_komenda[pozycja] = NULL;
	return cala_komenda;
}


/*
	Funkcja odpowiedzialna za obsluge poleceń, wypisywanie z pliku, wpisywanie, bledy
*/

pid_t obsluga_polecen(char** args, int input, int output, int error){
	pid_t pid;
	if(fork()==0){
		if(input != STDIN_FILENO){ //wypisywanie cat <
			dup2(input, STDIN_FILENO);
			close(input);
		}
		if(error != STDERR_FILENO){
			dup2(error, STDERR_FILENO);
			close(error);
		}
		if(output != STDOUT_FILENO){ //zapisywanie cat <
			dup2(output, STDOUT_FILENO);
			close(output);
		}
		if(execvp(args[0], args) == -1){ //proces dziecko
			fprintf(stderr, "lsh: %s Nie znaleziono polecenia \n", args[0]); //błędne podanie polecenia
		}
			exit(EXIT_FAILURE);
	}
	return pid;
}
/*
	Funkcja odpowiedzialna za uruchamianie procesów oraz obsługę strumienia danych
	@param komendy lista komend
	@param ilosc ilosc komend
	@param dziala_w_tle działające w tle
	@param przekierowanie_typ typ przekierowania	strumienia danych
	@param przekierowanie_plik plik przekierowania strumienia danych
*/

int lsh_startujemy_proces(char** komendy, int ilosc, int dziala_w_tle, 
int przekierowanie_typ, char * przekierowanie_plik){

	char** komenda_args;
	pid_t pid; //obsługiwany
	pid_t wpid; //oczekujący 
	int status = EXIT_SUCCESS;

	int i=0; //do pęli 

	int fd[2]; //STDIN_FILENO = 0; STDOUT_FILENO = 1; STDERR_FILENO = 2;
	int input = STDIN_FILENO;
	int output = STDOUT_FILENO;
	int error = STDERR_FILENO;

	
	for(i=0; i < ilosc-1; ++i){
		komenda_args = lsh_dzielimy(komendy[i], LSH_OGRANICZENIA_TEKSTU);
		if(pipe(fd)){
			perror("lsh:launch:pipe");
      return 1;		
		}
		
		pid = obsluga_polecen(komenda_args, input, fd[STDOUT_FILENO], error);
		close(fd[STDOUT_FILENO]);

		input = fd[STDIN_FILENO];
	}	
	
	komenda_args = lsh_dzielimy(komendy[i], LSH_OGRANICZENIA_TEKSTU);

	//sprawdzamy czy została wprowadzona pusta komenda
	if(komenda_args[0] == NULL){
		return EXIT_SUCCESS;
	}
 
  //obsługa wbudowanych funkcji 
	for (i = 0; i < lsh_wbudowane_funkcje_rozmiar; i++) {
    if (strcmp(komenda_args[0], lsh_wbudowane_nazwy[i]) == 0) {
      return (*lsh_wbudowane_funkcje[i])(komenda_args);
    }
  }
	
	//obsługa strumieni danych 
	if(przekierowanie_typ >= 0) {
    switch(przekierowanie_typ) {
      case STDOUT_FILENO: // wypisywanie
        output = creat(przekierowanie_plik, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(output < 0) {
          perror("lsh:błąd składni przy nieoczekiwanym znaczniku");
          return 1;
        }
        break;
      case STDERR_FILENO: //błąd
        printf("STDERR to %s\n", przekierowanie_plik);
        error = creat(przekierowanie_plik, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(output < 0) {
          perror("lsh:błąd składni przy nieoczekiwanym znaczniku");
          return 1;
        }
        break;
			case STDIN_FILENO: // wpisywanie
        input = open(przekierowanie_plik, O_RDONLY);
        if(input < 0) {
          perror("lsh:błąd składni przy nieoczekiwanym znaczniku");
          return 1;
        }
        break;
    }
  }

	pid = obsluga_polecen(komenda_args, input, output, error);

	//oczekiwanie na zakonczenie procesu działającego w tle

	if(!dziala_w_tle) {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
	return 1;
} 

/*
	Dzielenie linii komend na poszczególne pojedyncze komendy
	@param linia - polecenia z linii poleceń
	@param ograniczniki - znaki podziału zawarte w linii poleceń
*/



/*
	Funkcja odpowiedzialna za obsługę czytania z linii poleceń
*/

char* lsh_czytamy_linie(){

	int rozmiar = LSH_ROZMIAR;
	char* bufor = malloc(sizeof(char) * rozmiar);
	int znak;
	int pozycja = 0; 

	if(!bufor){
		perror("lsh:blad w czytaniu linii");
    exit(EXIT_FAILURE);
	}
	while(1){
		znak = getchar();
		if(pozycja == 0 && znak == EOF) {
      printf("\nZakończono\n");
      exit(EXIT_SUCCESS);
    } else
     if(znak == '\n' || znak == EOF) {
      bufor[pozycja] = '\0';
      return bufor;
    } else {
      bufor[pozycja] = znak;
    }
    pozycja++;
	}
}

/*
	Funkcja odpowiadająca za działanie powłoki
*/

void lsh_dzialanie_petla(){
	
	char* linia;
	char ** komendy;
	int ilosc;
	int status;
	int dzialanie_w_tle;
	char lokalizacja[1024];

	char* przekierowanie_plik;
	int przekierowanie_typ = -1; //nie ma żadnego przekierowania strumienia 
	//STDIN_FILENO = 0; STDOUT_FILENO = 1; STDERR_FILENO = 2;
	
	char* ampersand = NULL;
	char* przekierowanie = NULL;

	//nieskończona pętla odpowiadająca za pracę powłoki

	do{
		getcwd(lokalizacja, 1024);
		przekierowanie_typ = -1;
		dzialanie_w_tle = 0;
		ampersand = NULL;
	  	przekierowanie = NULL;
		
		printf("\033[1;38;5;198meliza@lsh\033[0m:\033[1;38;5;51m%s\033[0m$ ", lokalizacja);

    linia = lsh_czytamy_linie();
		//zastępujemy ampersant pustym bitem 
		if(ampersand = strchr(linia, '&')){
			*ampersand = '\0'; //pod miejsce w pamięci daje pusty bit 
			dzialanie_w_tle = 1; //uruchamia tryb praca w tle
		}

		//czy będziemy wypisywać < 
		//sprawdza, czy linia zawiera podany znak i zwraca wskaźnik na pierwsze wystąpienie
		//w linii
		if(przekierowanie = strchr(linia,'<')){
			przekierowanie_typ = STDIN_FILENO; 
			*przekierowanie = '\0';
			przekierowanie_plik = przekierowanie +1; //ustawia kolejną pozycję, by od niej zapisyawać do pliku 
			while(*przekierowanie_plik == ' ') { 
        przekierowanie_plik++;
      }
		}
		//czy jest błędne
		if(przekierowanie = strchr(linia,'2')){
			przekierowanie_typ = STDERR_FILENO;
			*przekierowanie = '\0';
			przekierowanie_plik = przekierowanie +2;
			while(*przekierowanie_plik == ' ') { 
        przekierowanie_plik++;
      }
		}
		//czy będziemy zapisywać >
		else if(przekierowanie = strchr(linia,'>')){
			przekierowanie_typ = STDOUT_FILENO;
			*przekierowanie = '\0';
			przekierowanie_plik = przekierowanie +1;
			while(*przekierowanie_plik == ' ') { 
        przekierowanie_plik++;
      }
		}
	//potoki poleceń
	komendy = lsh_dzielimy(linia, "|");
	status = lsh_startujemy_proces(komendy, ilosc, dzialanie_w_tle, przekierowanie_typ,
	przekierowanie_plik); 
	free(linia); 
	}while(1); 
}


int main() 
{
  // obsługa Ctrl + C
  signal(SIGINT, ctrl_C_handler);
	//uruchamianie i praca powłoki
  lsh_dzialanie_petla();

  return 0;
}
