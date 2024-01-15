#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include "../photo_lib/photo_lib.h"

int validate_params(int argc, char *argv[]) {
  // Ensure user passes at least one parameter
  if (argc == 1) {
    printf("Incorrect Usage. Please pass image filenames as argument. Space-separated filenames and wildcard notation are acceptable.\n");
    printf("Correct usage: %s <images>\n", argv[0]);
    return 1;
  }

  // Ensure all files have valid extension
  const char* valid_extensions[] = {".jpg", ".jpeg", ".png", ".gif"};
  // Iterate over arguments
  for (int i = 1; i < argc; i++) {
    // Ensure filename contains .
    if (strstr(argv[i], ".") == NULL) {
      printf("Error: Filename '%s' missing extension\n", argv[i]);
      return 1;
    }
    // Validate file extension
    bool valid = false;
    char* extension = strrchr(argv[i], '.');
    for (int j = 0; j < 4; j++) {
      if (strcmp(extension, valid_extensions[j]) == 0) {
        valid = true;
        break;
      }
    }
    // Return with error if invalid extension
    if (!valid) {
      printf("Error: File '%s' is of invalid type.\n", extension);
      printf("Valid file types are: ");
      for (int j = 0; j < 3; j++) {
        printf("%s, ", valid_extensions[j]);
      }
      printf("%s\n", valid_extensions[3]);
      return 1;
    }
  }

  // Ensure all parameters correspond to valid files that we have read access to. This includes expanding wildcard expressions.
  // Iterate over params
  for (int i = 1; i < argc; i++) {
    glob_t wildcard_matches;
    int glob_status = glob(argv[i], 0, NULL, &wildcard_matches);
    if (glob_status == 0) {
      // Iterate over matching files
      for (size_t j = 0; j < wildcard_matches.gl_pathc; j++) {
        char* current_file = wildcard_matches.gl_pathv[j];
        if (access(current_file, R_OK) != 0) {
          printf("Error: Read access not permitted to file '%s'\n", current_file);
          return 1;
        }
      }
    }
    else {
      printf("Error: Couldn't find any matching files for argument %s'\n", argv[i]);
      return 1;
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
    // Validate parameters
    int params_status = validate_params(argc, argv);
    if (params_status != 0) {
      return params_status;
    }
    
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