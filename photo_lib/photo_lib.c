#include <stdlib.h>
#include <unistd.h>

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