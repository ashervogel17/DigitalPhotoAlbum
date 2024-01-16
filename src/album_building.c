#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/wait.h>
#include "user_input.h"

#ifndef ROTATE_RESPONSE_LEN
#define ROTATE_RESPONSE_LEN 10
#endif

#ifndef CAPTION_RESPONSE_LEN
#define CAPTION_RESPONSE_LEN 100
#endif

int generate_thumbnail(char* input_filename, char* thumbnail_filename) {
  execlp("convert", "convert", input_filename, "-resize", "10%", thumbnail_filename, (char*) NULL);
  return -1;
}

int generate_medium_image(char* input_filename, char* medium_filename) {
  execlp("convert", "convert", input_filename, "-resize", "25%", medium_filename, (char*) NULL);
  return -1;
}

int display(char* filename) {
  execlp("display", "display", filename, (char*) NULL);
  return -1;
}

int get_user_input_for_rotation_and_write_to_pipe(char* input_filename, int rotate_write_fd) {
  char rotate_message[100];
  sprintf(rotate_message, "Rotate %s? [cw/ccw/no]", input_filename);
  char* try_again_message = "Invalid response.";
      
  // Gather input from user. Repeat until valid reponse (cw, cww, or no)
  char* rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN);
  int status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
  while (strcmp(rotate_buffer, "cw") != 0 && strcmp(rotate_buffer, "ccw") != 0 && strcmp(rotate_buffer, "no") != 0) {
    free(rotate_buffer);
    rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN);
    printf("%s ", try_again_message);
    status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
  }

  // Write user response to pipe
  write(rotate_write_fd, rotate_buffer, sizeof(rotate_buffer));

  // Clean up
  free(rotate_buffer);
  return status;
}

int get_user_input_for_caption_and_write_to_pipe(char* input_filename, int caption_write_fd) {
  char caption_message[100];
  sprintf(caption_message, "Enter a caption for %s", input_filename);
  char* caption_buffer = malloc(sizeof(char) * CAPTION_RESPONSE_LEN);
  int status = input_string(caption_message, caption_buffer, CAPTION_RESPONSE_LEN);
  if (status != 0) {
    printf("Error reading caption from user.\n");
    free(caption_buffer);
    return -1;
  }
  status = write(caption_write_fd, caption_buffer, sizeof(caption_buffer));
  free(caption_buffer);
  if (status != 0) {
    printf("Error writing caption to pipe.\n");
    return -1;
  }
  
  return status;
}

int perform_rotations(char* thumbnail_filename, char* medium_filename, int rotate_read_fd, int caption_read_fd) {
  char rotation_dir[ROTATE_RESPONSE_LEN];
  read(rotate_read_fd, rotation_dir, ROTATE_RESPONSE_LEN);

  pid_t thumbnail_rotation_pid, caption_rotation_pid;
  
  thumbnail_rotation_pid = fork();
  if (thumbnail_rotation_pid == 0) {
    if (strcmp(rotation_dir, "cw") == 0) {
      execlp("convert", "convert", thumbnail_filename, "-rotate", "90", thumbnail_filename, (char*)NULL);
    }
    else if (strcmp(rotation_dir, "ccw") == 0) {
      execlp("convert", "convert", thumbnail_filename, "-rotate", "-90", thumbnail_filename, (char*)NULL);
    }
    exit(-1);
  }

  caption_rotation_pid = fork();
  if (caption_rotation_pid == 0) {
    if (strcmp(rotation_dir, "cw") == 0) {
      execlp("convert", "convert", medium_filename, "-rotate", "90", medium_filename, (char*)NULL);
    }
    else if (strcmp(rotation_dir, "ccw") == 0) {
      execlp("convert", "convert", medium_filename, "-rotate", "-90", medium_filename, (char*)NULL);
    }
    exit(-1);
  }
  int status = 0;
  waitpid(thumbnail_rotation_pid, &status, 0);
  if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
    printf("Error with thumbnail rotation./n");
    return -1;
  }
  waitpid(caption_rotation_pid, &status, 0);
  if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
    printf("Error with medium rotation.\n");
    return -1;
  }
  return 0;
}