#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define MEMORY_MAX 256

struct memory_opt{
    uint8_t data[MEMORY_MAX][MEMORY_MAX];
};

void bench_time(struct timeval,struct timeval,char*);

// Transpozycja macierzy
void transpozition(uint8_t**,int);

// Funkcja pomocnicza do mull3
void smaller_mull(struct memory_opt*,struct memory_opt*,struct memory_opt*);

// Zwykłe mnożenie macierzy
void mull0(uint8_t**,uint8_t**,int);

//Mnożenie macierzy z wykorzystaniem transpozycji
void mull1(uint8_t**,uint8_t**,int);

//Mnożenie macierzy z wykorzystaniem transpozycji
// i podziałem na mniejsze części
void mull2(uint8_t**,uint8_t**,int);

// Funkcja do obliczania czasu i wypisywanie wyniku
void bench_time(struct timeval,struct timeval,char*);

int main(int argc,char **argv){

    // Pobranie danych z lini polecen
    if(argc<2){
        perror("Not enough arguments!\n");
        exit(1);
    }else if(argc>2){
        perror("Too much arguments!\n");
        exit(1);
    }
    int n;
    sscanf(argv[1],"%d",&n);
    printf("%d\n",n);

    // Inicjalizacja macierzy do mnożenia
    uint8_t **A,**B;
    A=(uint8_t**)malloc(sizeof(uint8_t*)*n);
    for (int i=0;i<n;i++)
        A[i]=(uint8_t*)malloc(sizeof(uint8_t)*n);

    B=(uint8_t**)malloc(sizeof(uint8_t*)*n);
    for (int i=0;i<n;i++)
        B[i]=(uint8_t*)malloc(sizeof(uint8_t)*n);

    // Wypełnienie losowymi danymi
    srand(time(NULL));
    for (int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            A[i][j]=rand();
            B[i][j]=rand();
        }
    }

   
    if(n<=3000) 
        mull0(A,B,n);

    mull1(A,B,n);
    mull2(A,B,n);
    return 0;
}

///Implementacja funkcji

void bench_time(struct timeval beg,struct timeval end,char *message){
    suseconds_t b=end.tv_usec-beg.tv_usec;
    time_t      a=end.tv_sec-beg.tv_sec;
    if(b<0){
        b+=1000000;
        a--;
    }
    printf("%s: %lds%ldus\n",message,a,b);
}

void transpozition(uint8_t **A,int n){
    uint8_t temp;
    for (int i=0;i<n;i++){
        for (int j=i+1;j<n;j++){
            temp=A[i][j];
            A[i][j]=A[j][i];
            A[j][i]=temp;
        }
    }
}

void smaller_mull(struct memory_opt *A,struct memory_opt *B,struct memory_opt *C){
    static uint8_t ans=0;
    for (int i=0;i<MEMORY_MAX;i++)
        for (int j=0;j<MEMORY_MAX;j++){
            ans=0;
            for (int k=0;k<MEMORY_MAX;k++)
                ans+=A->data[i][k]*B->data[j][k];
            C->data[i][j]=ans;
        }
}


void mull0(uint8_t **A,uint8_t **B,int n){
    struct timeval beg,end;
    uint8_t **C=(uint8_t**)malloc(sizeof(uint8_t*)*n);
    for (int i=0;i<n;i++)
        C[i]=(uint8_t*)calloc(n,sizeof(uint8_t));
    gettimeofday(&beg,NULL);
    for (int i=0;i<n;i++)
        for (int j=0;j<n;j++)
            for (int k=0;k<n;k++)
                C[i][j]+=A[i][k]*B[k][j];
    gettimeofday(&end,NULL);

    // Obliczanie czasu dzialania
    bench_time(beg,end,"time without any opt");

    // sprzątanie
    for (int i=0;i<n;i++)
        free(C[i]);
    free(C);
}

void mull1(uint8_t **A,uint8_t **B,int n){
    struct timeval beg,end;
    uint8_t **C=(uint8_t**)malloc(sizeof(uint8_t*)*n);
    for (int i=0;i<n;i++)
        C[i]=(uint8_t*)calloc(n,sizeof(uint8_t));
    gettimeofday(&beg,NULL);
    transpozition(B,n);
    for (int i=0;i<n;i++)
        for (int j=0;j<n;j++)
            for (int k=0;k<n;k++)
                C[i][j]+=A[i][k]*B[j][k];
    transpozition(B,n);
    gettimeofday(&end,NULL);

    // Obliczanie czasu dzialania
    bench_time(beg,end,"time with trans");

    // sprzątanie
    for (int i=0;i<n;i++)
        free(C[i]);
    free(C);
}

void mull2(uint8_t **A,uint8_t **B,int n){
    struct timeval beg,end;

    uint8_t **C=(uint8_t**)malloc(sizeof(uint8_t*)*n);
    for (int i=0;i<n;i++)
        C[i]=(uint8_t*)calloc(n,sizeof(uint8_t));

    gettimeofday(&beg,NULL);

        // Przygotowanie podziału
    int new_size=n/MEMORY_MAX;

    if(new_size*MEMORY_MAX<n)
        new_size++;
    struct memory_opt ***a[3];

    for (int i=0;i<3;i++)
        a[i]=(struct memory_opt***)malloc(sizeof(struct memory_opt*)*new_size);

    for (int i=0;i<3;i++)
        for (int j=0;j<new_size;j++)
            a[i][j]=(struct memory_opt**)calloc(new_size,sizeof(struct memory_opt));

    for (int i=0;i<3;i++)
        for (int j=0;j<new_size;j++)
            for (int k=0;k<new_size;k++)
                a[i][j][k]=(struct memory_opt*)calloc(1,sizeof(struct memory_opt));

    // Przepisanie danych
    transpozition(B,n);
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            a[0][i/MEMORY_MAX][j/MEMORY_MAX]->data[i%MEMORY_MAX][j%MEMORY_MAX]=A[i][j];
            a[1][i/MEMORY_MAX][j/MEMORY_MAX]->data[i%MEMORY_MAX][j%MEMORY_MAX]=B[i][j];
        }
    }
    transpozition(B,n);

    // Mnożenie
    for (int i=0;i<new_size;i++)
        for (int j=0;j<new_size;j++)
            for (int k=0;k<new_size;k++){
                smaller_mull(a[0][i][k],a[1][j][k],a[2][i][j]);
            }

    // Przepisanie danych
    for (int i=0;i<n;i++)
        for (int j=0;j<n;j++)
            C[i][j]=a[2][i/MEMORY_MAX][j/MEMORY_MAX]->data[i%MEMORY_MAX][j%MEMORY_MAX];

    gettimeofday(&end,NULL);

    // Obliczanie czasu dzialania
    bench_time(beg,end,"time with all opt");

    // sprzątanie
    for (int i=0;i<3;i++)
        for (int j=0;j<new_size;j++)
            free(a[i][j]);

    for (int i=0;i<3;i++)
        free(a[i]);

    for (int i=0;i<n;i++)
        free(C[i]);
    free(C);
}
