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