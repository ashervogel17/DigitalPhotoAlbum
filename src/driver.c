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

#define THUMBNAIL_GEN_PID_INDEX 0
#define MEDIUM_GEN_PID_INDEX 1
#define DISPLAY_PID_INDEX 2
#define ROTATE_THUMBNAIL_PID_INDEX 3
#define ROTATE_MEDIUM_PID_INDEX 4

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
  pid_t pid = fork();
  if (pid == 0) {
    printf("Verifying access to X server...\n");
    execlp("convert", "convert", "-resize", "100%", argv[1], argv[1]);
  }
  int status;
  waitpid(pid, &status, 0);
  if (status != 0) {
    return status;
  }

  // Set up HTML file and write header
  char* html_filename = "index.html";
  write_html_header(html_filename);
  
  // Nested array to store child pids
  pid_t pids[argc-1][5];

  // Store index of current image being processed. Note, that pids are 0-indexed while arguments are 1-indexed.
  int image_index = 0;
  
  // Kick off generation of first thumbmail
  pids[image_index][THUMBNAIL_GEN_PID_INDEX] = fork();
  if (pids[image_index][THUMBNAIL_GEN_PID_INDEX] == 0) {   
    char dst[20];
    char* extension = strrchr(argv[image_index + 1], '.'); // file extension 
    sprintf(dst, "thumb%d%s", image_index, extension);
    return generate_thumbnail(argv[image_index+1], dst);
  }

  // Kickoff generation of first medium image
  pids[image_index][MEDIUM_GEN_PID_INDEX] = fork();
  if (pids[image_index][MEDIUM_GEN_PID_INDEX] == 0) {
    char dst[20];
    char* extension = strrchr(argv[image_index + 1], '.'); // file extension 
    sprintf(dst, "med%d%s", image_index, extension);
    return generate_medium_image(argv[image_index+1], dst);
  }

  // Iterate though all images. Kick off generation of next thumbnail and medium before processing current image.
  const int num_images = argc - 1;
  char* extension;
  char thumbnail_filename[20];
  char medium_filename[20];
  while (image_index < num_images) {
    // Fork to display current thumbnail
    waitpid(pids[image_index][THUMBNAIL_GEN_PID_INDEX], NULL, 0); // Wait for thumbnail to finish generating
    pids[image_index][DISPLAY_PID_INDEX] = fork();
    if (pids[image_index][DISPLAY_PID_INDEX] == 0) {
      char* extension = strrchr(argv[image_index+1], '.');
      char thumbnail_filename[20];
      sprintf(thumbnail_filename, "thumb%d%s", image_index, extension);
      return display(thumbnail_filename);
    }
    
    // Generate thumbnail and medium-sized for next index.
    if (image_index + 1 < num_images) {
      pids[image_index+1][THUMBNAIL_GEN_PID_INDEX] = fork();
      if (pids[image_index+1][THUMBNAIL_GEN_PID_INDEX] == 0) {
        waitpid(pids[image_index][THUMBNAIL_GEN_PID_INDEX], NULL, 0); // wait for previous thumbnail to finish generating
        char dst[20];
        char* extension = strrchr(argv[image_index + 2], '.'); // file extension 
        sprintf(dst, "thumb%d%s", image_index+1, extension);
        return generate_thumbnail(argv[image_index+2], dst);
      }
      pids[image_index+1][MEDIUM_GEN_PID_INDEX] = fork();
      if (pids[image_index+1][MEDIUM_GEN_PID_INDEX] == 0) {
        waitpid(pids[image_index][MEDIUM_GEN_PID_INDEX], NULL, 0); // wait for previous medium to finish generating
        char dst[20];
        char* extension = strrchr(argv[image_index + 2], '.'); // file extension 
        sprintf(dst, "med%d%s", image_index+1, extension);
        return generate_medium_image(argv[image_index+2], dst);
      }
    }

    // Prompt user for input on rotation
    char* input_filename = argv[image_index+1]; // Filename of current image
    char rotate_message[100]; // Prompt for rotation input
    char* rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN); // Stores user input on rotation
    sprintf(rotate_message, "Rotate %s? [cw/ccw/no]", input_filename);
    status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
    // Handle error on input
    if (status != 0) {
      fprintf(stderr, "Error reading rotation input from user.\n");
      free(rotate_buffer);
      return status;
    }
    // Handle invalid input. User must enter cw, ccw, or no.
    while (strcmp(rotate_buffer, "cw") != 0 && strcmp(rotate_buffer, "ccw") != 0 && strcmp(rotate_buffer, "no") != 0) {
      free(rotate_buffer);
      rotate_buffer = malloc(sizeof(char) * ROTATE_RESPONSE_LEN);
      printf("%s ", "Invalid response.");
      status = input_string(rotate_message, rotate_buffer, ROTATE_RESPONSE_LEN);
    }


    // Build filename for thumbnail and medium
    extension = strrchr(argv[image_index+1], '.');
    sprintf(thumbnail_filename, "thumb%d%s", image_index, extension);
    sprintf(medium_filename, "med%d%s", image_index, extension);

    // Fork to perform rotation on thumbnail
    pids[image_index][ROTATE_THUMBNAIL_PID_INDEX] = fork();
    if (pids[image_index][ROTATE_THUMBNAIL_PID_INDEX] == 0) {
      status = rotate(thumbnail_filename, rotate_buffer);
      if (status != 0) {
        fprintf(stderr, "Issue rotating thumbnail.\n");
        return status;
      }
      return 0;
    }

    // Fork to perform rotation on medium
    pids[image_index][ROTATE_MEDIUM_PID_INDEX] = fork();
    if (pids[image_index][ROTATE_MEDIUM_PID_INDEX] == 0) {
      status = rotate(medium_filename, rotate_buffer);
      if (status != 0) {
        fprintf(stderr, "Issue rotating medium-sized image.\n");
        return status;
      }
      return 0;
    }


    // Prompt user for caption
    char caption_message[100]; // Prompt for caption input
    sprintf(caption_message, "Enter a caption for %s", argv[image_index+1]);
    char* caption_buffer = malloc(CAPTION_RESPONSE_LEN * sizeof(char));
    status = input_string(caption_message, caption_buffer, CAPTION_RESPONSE_LEN);
    // Handle error on input
    if (status != 0) {
      fprintf(stderr, "Error reading caption input from user.\n");
      free(caption_buffer);
      return status;
    }


    // Kill display
    kill(pids[image_index][DISPLAY_PID_INDEX], SIGKILL); 
    
    // Wait for 2 rotations to terminate
    waitpid(pids[image_index][ROTATE_THUMBNAIL_PID_INDEX], NULL, 0);
    waitpid(pids[image_index][ROTATE_MEDIUM_PID_INDEX], NULL, 0);

    // Write to HTML file
    add_html_image(html_filename, thumbnail_filename, medium_filename, caption_buffer);

    // Clean up
    free(caption_buffer);
    free(rotate_buffer);
    image_index ++;
    }

  // Finish HTML file
  write_html_footer(html_filename);
  return 0;

  }