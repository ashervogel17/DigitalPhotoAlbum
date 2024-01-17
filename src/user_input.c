#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef ROTATE_RESPONSE_LEN
#define ROTATE_RESPONSE_LEN 10
#endif

#ifndef CAPTION_RESPONSE_LEN
#define CAPTION_RESPONSE_LEN 100
#endif

#define STRING_LEN  32

// Credit:
// adam salem, s.w. smith
// updated sep 2020
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

