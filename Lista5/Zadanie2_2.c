#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<signal.h>
#include<sys/types.h>
int main(int argc, char *argv[]){
kill(1, SIGKILL);
sleep(1);
setuid(0);
kill(1, SIGKILL);
return 0;
}
