#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int global;

void cntl_c_handler(int dummy) {
  signal(SIGINT, cntl_c_handler);
  global++;
  printf("Recive SIGINT number   %d\n", global);
}

int main(int argc, char const *argv[]) {
  global = 0;
  signal(SIGINT, cntl_c_handler);
  printf("%d\n", getpid());
  for (int j = 0; j < 20000; j++) {
    for (int i = 0; i < 1000000; i++) {
    }
  }

  return 0;
}
