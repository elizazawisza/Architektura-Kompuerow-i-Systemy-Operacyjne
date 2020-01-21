#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void signal_handler(int dummy){
  printf("Sygnał złapany: %d\n", dummy);
}

int main(){
  for(int i=1; i<=62; i++){
    signal(i, signal_handler);
  }

  for(int i=1; i<=62; i++){
    printf("Wysyłany sygnał: %d\n", i);
    kill(getpid(), i);
    sleep(1);
  }
  return 0;
}
