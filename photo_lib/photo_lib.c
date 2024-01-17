#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int generate_thumbnail(char* src_file, char* dst_file) {
  execlp("convert", "convert", src_file, "-resize", "10%", dst_file, (char*) NULL);
  return -1;
}

int generate_medium_image(char* src_file, char* dst_file) {
  execlp("convert", "convert", src_file, "-resize", "25%", dst_file, (char*) NULL);
  return -1;
}

int display(char* file) {
  execlp("display", "display", file, (char*) NULL);
  return -1;
}

int rotate(char* file, char* rotation) {
  if (strcmp(rotation, "no") == 0) {
    return 0;
  }

  char rotation_num[5];
  if (strcmp(rotation, "cw") == 0) {
    strcpy(rotation_num, "90");
  }
  else if (strcmp(rotation, "ccw") == 0) {
    strcpy(rotation_num, "-90");
  }
  else {
    printf("Error: Invalid value for rotation %s. Accepeted values are cw, ccw, or no.\n", rotation);
    return -1;
  }
  
  execlp("convert", "convert", file, "-rotate", rotation_num, file, (char*) NULL);
  return -1;
}