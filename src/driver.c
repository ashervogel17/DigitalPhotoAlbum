#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <X11/Xlib.h>
#include "../photo_lib/photo_lib.h"
#include "user_input.h"
#include "validate_params.h"
#include "write_html.h"

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
    char* html_filename = "index.html";

    // Create two pipes for rotate input and caption input
    int rotate_fd[2];
    if (pipe(rotate_fd) < 0) {
      printf("Error: Couldn't create pipe for rotation messages.\n");
      exit(1);
    }

    // Array to keep track of process id's
    pid_t pids[7];
    pids[0] = getpid(); // main pid at index 0

    write_html_header(html_filename);

    // Process 1: Generate Thumbnail
    pids[1] = fork();
    if (pids[1] == 0) {
      // Rotation pipe not needed
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      return generate_thumbnail(input_filename, thumbnail_filename);
    }

    // Process 2: Generate Medium-Sized Image
    pids[2] = fork();
    if (pids[2] == 0) {
      // Rotation pipe not needed
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      return generate_medium_image(input_filename, medium_filename);
    }

    // Process 3: Display Thumbnail
    waitpid(pids[1], NULL, 0); // Wait for generation of thumbnail
    pids[3] = fork();
    if (pids[3] == 0) {
      // Rotation pipe not needed
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      return display(thumbnail_filename);
    }

    // Process 4: Get User Input for Rotation
    waitpid(pids[1], NULL, 0); // Wait for generation of thumbnail
    pids[4] = fork();
    if (pids[4] == 0) {
      // Prompt to user
      char rotate_message[100];
      sprintf(rotate_message, "Rotate %s? [cw/ccw/no]", input_filename);

      // Gather input from user. Repeat until valid reponse (cw, cww, or no)
      char* rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN);
      int status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
      while (strcmp(rotate_buffer, "cw") != 0 && strcmp(rotate_buffer, "ccw") != 0 && strcmp(rotate_buffer, "no") != 0) {
        free(rotate_buffer);
        rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN);
        printf("%s ", "Invalid response.");
        status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
      }

      // Write user response to pipe
      write(rotate_fd[1], rotate_buffer, sizeof(rotate_buffer));

      // Clean up
      free(rotate_buffer);
      close(rotate_fd[0]);
      close(rotate_fd[1]);
      return status;
    }

    // Process 5: Rotate Thumbnail and Medium Sized Images
    waitpid(pids[2], NULL, 0); // Wait for generaiton of medium-sized image
    waitpid(pids[4], NULL, 0); // Wait for user input on rotation
    pids[5] = fork();
    if (pids[5] == 0) {
      char rotation_dir[ROTATE_RESPONSE_LEN];
      read(rotate_fd[0], rotation_dir, ROTATE_RESPONSE_LEN);
      close(rotate_fd[0]);
      close(rotate_fd[1]);

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

    // Process 6: Get User Input for Caption and Write to HTML
    waitpid(pids[5], NULL, 0); // Wait for image rotation
    pids[6] = fork();
    if (pids[6] == 0) {
      // Rotation pipe not needed
      close(rotate_fd[0]);
      close(rotate_fd[1]);

      // Prompt to user
      char caption_message[100];
      sprintf(caption_message, "Enter a caption for %s", input_filename);

      // Spin of child process to get caption from user and write to pipe
      int caption_fd[2];
      int status = pipe(caption_fd);
      if (status < 0) {
        printf("Couldn't create caption pipe.\n");
        return status;
      }
      pid_t caption_input_pid = fork();
      if (caption_input_pid == 0) {
        char* caption_buffer = calloc(CAPTION_RESPONSE_LEN, sizeof(char));
        int status = input_string(caption_message, caption_buffer, CAPTION_RESPONSE_LEN);
        if (status != 0) {
          printf("Error reading caption from user.\n");
          free(caption_buffer);
          return status;
        }
        write(caption_fd[1], caption_buffer, sizeof(char)*(strlen(caption_buffer) + 1));
        free(caption_buffer);
        close(caption_fd[0]);
        close(caption_fd[1]);
        return 0;
      }

      // Wait for user caption from user
      waitpid(caption_input_pid, NULL, 0);

      // Read caption from pipe and write to HTML file with images
      char* caption_buffer = calloc(CAPTION_RESPONSE_LEN, sizeof(char));
      read(caption_fd[0], caption_buffer, CAPTION_RESPONSE_LEN);
      add_html_image(html_filename, thumbnail_filename, medium_filename, caption_buffer);

      // Clean up
      free(caption_buffer);
      close(caption_fd[0]);
      close(caption_fd[1]);
      return status;
    }
    
    // Clean up
    waitpid(pids[6], NULL, 0);
    free(input_filename);
    kill(pids[3], 42);
    write_html_footer(html_filename);

    return 0;
}