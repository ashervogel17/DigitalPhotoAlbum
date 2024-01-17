#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <glob.h>
#include "../photo_lib/photo_lib.h"
#include "user_input.h"
#include "validate_params.h"
#include "write_html.h"

#ifndef ROTATE_RESPONSE_LEN
#define ROTATE_RESPONSE_LEN 30
#endif

#ifndef CAPTION_RESPONSE_LEN
#define CAPTION_RESPONSE_LEN 150
#endif

int main(int argc, char *argv[]) {
  // Validate parameters
  printf("Validating parameters...\n");
  int params_status = validate_params(argc, argv, ".");
  if (params_status != 0) {
    return params_status;
  }

  // Test access to X server before proceeding
  // pid_t pid = fork();
  // if (pid == 0) {
  //   printf("Verifying access to X server...\n");
  //   execlp("convert", "convert", "-resize", "100%", argv[1], argv[1]);
  // }
  // waitpid(pid, NULL, 0);

  // Set up HTML file
  char* html_filename = "index.html";
  write_html_header(html_filename);
  
  // Organize management of child processes
    // pids[i][0]: generation of ith thumbnail
    // pids[i][1]: generation of ith medium
    // pids[i][2]: display ith thumbnail
    // pids[i][3]: Get rotation and caption input from user. Perform rotation. Write to HTML.
  pid_t pids[argc-1][4]; // pids[i][j] corresponds to the jth process of the (i+1)th image
  
  // Kick off generation of first thumbmail
  int image_index = 0;
  pids[image_index][0] = fork();
  if (pids[image_index][0] == 0) {   
    char dst[20];
    char* extension = strrchr(argv[image_index + 1], '.'); // file extension 
    sprintf(dst, "thumb%d%s", image_index, extension);
    return generate_thumbnail(argv[image_index+1], dst);
  }

  // Kickoff generation of first medium image
  pids[image_index][1] = fork();
  if (pids[image_index][1] == 0) {
    char dst[20];
    char* extension = strrchr(argv[image_index + 1], '.'); // file extension 
    sprintf(dst, "med%d%s", image_index, extension);
    return generate_medium_image(argv[image_index+1], dst);
  }

  // Iterate though all images. Kick off generation of next thumbnail and medium before processing current image.
  while (image_index < argc - 1) {
    if (image_index > 0) {
      waitpid(pids[image_index-1][3], NULL, 0);
    }
    printf("Image index: %d. Argc: %d. Number of photos: %d.\n", image_index, argc, argc-1);
    // Generate thumbnail and medium-sized for next index.
    if (image_index + 1 < argc - 1) {
      pids[image_index+1][0] = fork();
      if (pids[image_index+1][0] == 0) {
        waitpid(pids[image_index][0], NULL, 0); // wait for previous thumbnail to finish generating
        char dst[20];
        char* extension = strrchr(argv[image_index + 2], '.'); // file extension 
        sprintf(dst, "thumb%d%s", image_index+1, extension);
        return generate_thumbnail(argv[image_index+2], dst);
      }
      pids[image_index+1][1] = fork();
      if (pids[image_index+1][1] == 0) {
        waitpid(pids[image_index][1], NULL, 0); // wait for previous medium to finish generating
        char dst[20];
        char* extension = strrchr(argv[image_index + 2], '.'); // file extension 
        sprintf(dst, "med%d%s", image_index+1, extension);
        return generate_medium_image(argv[image_index+2], dst);
      }
    }

    // Fork to display current thumbnail
    waitpid(pids[image_index][0], NULL, 0); // Wait for thumbnail to finish generating
    if (image_index > 0) {
      waitpid(pids[image_index-1][2], NULL, 0); // Wait for previous thumbnail display to terminate
    }
    pids[image_index][2] = fork();
    if (pids[image_index][2] == 0) {
      char* extension = strrchr(argv[image_index+1], '.');
      char thumbnail_filename[20];
      sprintf(thumbnail_filename, "thumb%d%s", image_index, extension);
      return display(thumbnail_filename);
    }

    // Fork get user input on rotation and caption. Perform rotation. Write to HTML.
    waitpid(pids[image_index][0], NULL, 0); // Wait for thumbnail to finish generating
    pids[image_index][3] = fork();
    if (pids[image_index][3] == 0) {
      printf("Collecting rotation and caption on index %d\n", image_index);
      int status = 0; // Keep track of status. Exit if nonzero
      char* input_filename = argv[image_index+1];
      char rotate_message[100]; // Prompt for rotation input
      char* rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN); // Stores user input on rotation
      
      // Gather rotation input from user. Repeat until valid reponse (cw, cww, or no)
      sprintf(rotate_message, "Rotate %s? [cw/ccw/no]", input_filename);
      status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
      if (status != 0) {
        fprintf(stderr, "Error reading rotation input from user.\n");
        free(rotate_buffer);
        return status;
      }
      while (strcmp(rotate_buffer, "cw") != 0 && strcmp(rotate_buffer, "ccw") != 0 && strcmp(rotate_buffer, "no") != 0) {
        free(rotate_buffer);
        rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN);
        printf("%s ", "Invalid response.");
        status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
      }

      // Get caption from user
      char caption_message[100]; // Prompt for caption input
      sprintf(caption_message, "Enter a caption for %s", argv[image_index+1]);
      char* caption_buffer = malloc(CAPTION_RESPONSE_LEN * sizeof(char));
      status = input_string(caption_message, caption_buffer, CAPTION_RESPONSE_LEN);
      if (status != 0) {
        fprintf(stderr, "Error reading caption input from user.\n");
        free(caption_buffer);
        return status;
      }

      printf("Rotation: %s\n", rotate_buffer);
      printf("Caption: %s\n", caption_buffer);

      kill(pids[image_index][2], SIGKILL);

      // Perform rotations
      char* extension = strrchr(argv[image_index+1], '.');
      char thumbnail_filename[20];
      sprintf(thumbnail_filename, "thumb%d%s", image_index, extension);
      status = rotate(thumbnail_filename, rotate_buffer);
      if (status != 0) {
        fprintf(stderr, "Issue rotating thumbnail.\n");
        return status;
      }

      char medium_filename[20];
      extension = strrchr(argv[image_index + 1], '.'); // file extension 
      sprintf(medium_filename, "med%d%s", image_index, extension);
      status = rotate(medium_filename, rotate_buffer);
      if (status != 0) {
        fprintf(stderr, "Issue rotating medium-sized image.\n");
        return status;
      }

      // Write to HTML file
      add_html_image(html_filename, thumbnail_filename, medium_filename, caption_buffer);

      // Clean up
      free(caption_buffer);
      free(rotate_buffer);
      
      return 0;
    }
    
    waitpid(pids[image_index][0], NULL, 0);
    waitpid(pids[image_index][1], NULL, 0);
    waitpid(pids[image_index][2], NULL, 0);
    waitpid(pids[image_index][4], NULL, 0);
    image_index++;

  }

  // Finish HTML file
  write_html_footer(html_filename);

  return 0;
}