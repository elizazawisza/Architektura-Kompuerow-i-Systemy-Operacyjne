#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  int pid = 0;
  if (argc > 1) {
    for (int i = 0; i < strlen(argv[1]); i++) {
      pid = argv[1][i] - '0' + pid;
      pid = pid *10;
    }
    pid = pid / 10;
    for (int i = 0; i < 1000; i++) {
      kill(pid, SIGINT);
    }
  }
  return 0;
}
