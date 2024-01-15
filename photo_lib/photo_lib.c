#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h> 
#include <sys/wait.h>

int resize(char* src, char* dst, int percent) {
  int rc;
  char* factor = malloc(sizeof(char) * 4);

  sprintf(factor, "%d%%", percent);

  rc = fork();

  if (0 == rc) {
    execlp("convert", "-resize", factor, src, dst);
    exit(0);
  }

  else {
    wait(&rc);
    free(factor);
  }
  
  
  return 0;
}

int rotate(char* src, char* dst, int degrees) {
  int rc;
  char* factor = malloc(sizeof(char) * 4);

  sprintf(factor, "%d%%", degrees);

  rc = fork();

  if (0 == rc) {
    execlp("convert", "-rotate", factor, src, dst);
    exit(0);
  }
  
  else {
    wait(&rc);
    free(factor);
  }

  return 0;
}

int display(char* filename) {
  printf("In display. Pid: %d\n", getpid());
  printf("Forking. Pid: %d\n", getpid());
  int rc = fork();

  if (0 == rc) {
    execlp("display", "display", filename, NULL);
    exit(0);
  }

  else {
    wait(&rc);
  }

  return 0;
}