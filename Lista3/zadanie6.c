#include<stdio.h>
int main(int argc, char *argv[]){
	int k;
	for(int i = 0; i < 16; i++)
		for(int j = 0; j < 16; j++){
			k = 16 * i + j;
			printf("\033[38;5;%dmHello World!\n\033[0m" ,k);	
		}



	return 0;
}
