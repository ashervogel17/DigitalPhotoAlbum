#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "../photo_lib/photo_lib.h"
#include "./user_input.h"
#include "./validate_params.h"
#include "./album_building.h"

#ifndef ROTATE_RESPONSE_LEN
#define ROTATE_RESPONSE_LEN 10
#endif

#ifndef CAPTION_RESPONSE_LEN
#define CAPTION_RESPONSE_LEN 100
#endif

#define MAX_FILENAME_LEN 50


int main(int argc, char *argv[]) {
    // Validate parameters
    int params_status = validate_params(argc, argv);
    if (params_status != 0) {
      return params_status;
    }

    // Ensure write priveledge to current directory
    int write_status = validate_write_access(".");
    if (write_status != 0) {
      return write_status;
    }

    // Filenames
    char* input_filename = malloc(sizeof(char) * MAX_FILENAME_LEN);
    strcpy(input_filename, argv[1]);
    char* thumbnail_filename = "thumb.jpg";
    char* medium_filename = "med.jpg";

    // Create two pipes for rotate input and caption input
    int rotate_fd[2];
    int caption_fd[2];
    if (pipe(rotate_fd) < 0) {
      printf("Error: Couldn't create pipe for rotation messages.\n");
      exit(1);
    }
    if (pipe(caption_fd) < 0) {
      printf("Error: Couldn't create pipe for caption messages.\n");
      exit(1);
    }

    pid_t pids[7];
    pids[0] = getpid(); // main pid


    // Process 1: Generate Thumbnail
    pids[1] = fork();
    if (pids[1] == 0) {
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      close(caption_fd[0]);
      close(caption_fd[1]);
      return generate_thumbnail(input_filename, thumbnail_filename);
    }


    // Process 2: Generate Medium-Sized Image
    pids[2] = fork();
    if (pids[2] == 0) {
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      close(caption_fd[0]);
      close(caption_fd[1]);
      return generate_medium_image(input_filename, medium_filename);
    }


    // Process 3: Display Thumbnail
    waitpid(pids[1], NULL, 0); // Wait for generation of thumbnail
    pids[3] = fork();
    if (pids[3] == 0) {
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      close(caption_fd[0]);
      close(caption_fd[1]);
      return display(thumbnail_filename);
    }


    // Process 4: Get User Input for Rotation
    waitpid(pids[1], NULL, 0); // Wait for generation of thumbnail
    pids[4] = fork();
    if (pids[4] == 0) {
      int status = get_user_input_for_rotation_and_write_to_pipe(input_filename, rotate_fd[1]);
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      close(caption_fd[0]);
      close(caption_fd[1]);
      return status;
    }


    // Process 5: Get User Input for Caption
    waitpid(pids[4], NULL, 0); // Wait for termination of process that gets user input for rotation
    pids[5] = fork();
    if (pids[5] == 0) {
      int status = get_user_input_for_caption_and_write_to_pipe(input_filename, caption_fd[1]);
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      close(caption_fd[0]);
      close(caption_fd[1]);
      return status;
    }

    // Process 6: Rotate Thumbnail and Medium Sized Images
    waitpid(pids[2], NULL, 0); // Wait for generaiton of medium-sized image
    waitpid(pids[5], NULL, 0); // Wait for user input on rotation
    pids[6] = fork();
    if (pids[6] == 0) {
      char rotation_dir[ROTATE_RESPONSE_LEN];
      read(rotate_fd[0], rotation_dir, ROTATE_RESPONSE_LEN);
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      close(caption_fd[0]);
      close(caption_fd[1]);

      pid_t rotation_pids[2];

      rotation_pids[0] = fork();
      if (rotation_pids[0] == 0) {
        return rotate(thumbnail_filename, rotation_dir);
      }

      rotation_pids[1] = fork();
      if (rotation_pids[1] == 0) {
        return rotate(medium_filename, rotation_dir);
      }
      
      int status = 0;
      for (int i = 0; i < 2; i++) {
        waitpid(rotation_pids[i], &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
          printf("Error with rotations./n");
          return -1;
        }
      }
      return 0;
    }
    
    
    free(input_filename);
    
    waitpid(pids[6], NULL, 0);
    kill(pids[3], 42);

    return 0;
}