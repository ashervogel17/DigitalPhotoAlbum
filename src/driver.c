#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include "./validate_params.h"
#include "../photo_lib/photo_lib.h"

#define STRING_LEN  32
#define MAX_FILENAME_LEN 50


// prompt the user with message, and save input at buffer
// (which should have space for at least len bytes)
int input_string(char *message, char *buffer, int len) {

  int rc = 0, fetched, lastchar;

  if (NULL == buffer)
    return -1;

  if (message)
    printf("%s: ", message);

  // get the string.  fgets takes in at most 1 character less than
  // the second parameter, in order to leave room for the terminating null.  
  // See the man page for fgets.
  fgets(buffer, len, stdin);
  
  fetched = strlen(buffer);


  // warn the user if we may have left extra chars
  if ( (fetched + 1) >= len) {
    fprintf(stderr, "warning: might have left extra chars on input\n");
    rc = -1;
  }

  // consume a trailing newline
  if (fetched) {
    lastchar = fetched - 1;
    if ('\n' == buffer[lastchar])
      buffer[lastchar] = '\0';
  }

  return rc;
}

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
    const char* thumbnail_filename = "thumb.jpg";
    const char* medium_filename = "med.jpg";

    int rotate_input_len = 10;
    int caption_input_len = 100;

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
      execlp("convert", "convert", input_filename, "-resize", "10%", thumbnail_filename, (char*)NULL);
      exit(-1);
    }

    // Process 2: Generate Medium-Sized Image
    pids[2] = fork();
    if (pids[2] == 0) {
      execlp("convert", "convert", input_filename, "-resize", "25%", medium_filename, (char*)NULL);
      exit(-1);
    }

    // Process 3: Display Thumbnail
    waitpid(pids[1], NULL, 0); // Wait for generation of thumbnail
    pids[3] = fork();
    if (pids[3] == 0) {
      execlp("display", "display", thumbnail_filename);
      exit(-1);
    }

    // Process 4: Get User Input for Rotation
    waitpid(pids[1], NULL, 0); // Wait for generation of thumbnail
    pids[4] = fork();
    if (pids[4] == 0) {

      // Build prompt
      char rotate_message[100];
      sprintf(rotate_message, "Rotate %s? [cw/ccw/no]", input_filename);
      char* try_again_message = "Invalid response.";
      
      // Gather input from user. Repeat until valid reponse (cw, cww, or no)
      char* rotate_buffer = malloc(sizeof(char) * rotate_input_len);
      int status = input_string(rotate_message, rotate_buffer, rotate_input_len);
      while (strcmp(rotate_buffer, "cw") != 0 && strcmp(rotate_buffer, "ccw") != 0 && strcmp(rotate_buffer, "no") != 0) {
        free(rotate_buffer);
        rotate_buffer = malloc(sizeof(char) * rotate_input_len);
        printf("%s ", try_again_message);
        status = input_string(rotate_message, rotate_buffer, rotate_input_len);
      }

      // Write user response to pipe
      close(rotate_fd[0]);
      write(rotate_fd[1], rotate_buffer, sizeof(rotate_buffer));
      close(rotate_fd[1]);

      // Clean up
      free(rotate_buffer);
      return status;
    }

    // Process 5: Get User Input for Caption
    waitpid(pids[4], NULL, 0); // Wait for termination of process that gets user input for rotation
    pids[5] = fork();
    if (pids[5] == 0) {
      char caption_message[100];
      sprintf(caption_message, "Enter a caption for %s", input_filename);
      char* caption_buffer = malloc(sizeof(char) * caption_input_len);
      int status = input_string(caption_message, caption_buffer, caption_input_len);
      close(caption_fd[0]);
      write(caption_fd[1], caption_buffer, sizeof(caption_buffer));
      close(caption_fd[1]);
      free(caption_buffer);
      return status;
    }

    // Process 6: Rotate Thumbnail and Medium Sized Images
    waitpid(pids[2], NULL, 0); // Wait for generaiton of medium-sized image
    waitpid(pids[5], NULL, 0); // Wait for user input on rotation
    pids[6] = fork();
    if (pids[6] == 0) {
      char rotation_dir[rotate_input_len];
      close(rotate_fd[1]);
      read(rotate_fd[0], rotation_dir, 100);
      close(rotate_fd[0]);

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

      exit(0);
    }
    
    
    free(input_filename);
    
    waitpid(pids[6], NULL, 0);
    kill(pids[3], 42);
    wait(0);


    return 0;
  
}

/* iterate over photos
  generate thumbnail
  display thumbnail to user
  ask user to rotate
  rotate
  ask user for caption
  generate 25% photo
  build HTML file
*/

/*
Processes:
  - generate thumbnail (no waiting)
  - generate medium (no waiting)
  - get user input (waiting on thumbnail to be ready for display)
  - rotate (waiting on all other processes)

*/