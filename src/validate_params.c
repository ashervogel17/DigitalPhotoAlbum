#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int validate_param_count(int argc, char* program) {
  if (argc == 1) {
    fprintf(stderr, "Incorrect Usage. Please pass image filenames as argument. Space-separated filenames and wildcard notation are acceptable.\n");
    fprintf(stderr, "Correct usage: %s <images>\n", program);
    return -1;
  }
  return 0;
}

int validate_file_extensions(int argc, char* argv[]) {
  const char* valid_extensions[] = {".jpg", ".jpeg", ".png", ".gif"};
  // Iterate over arguments
  for (int i = 1; i < argc; i++) {
    // Ensure filename contains .
    if (strstr(argv[i], ".") == NULL) {
      fprintf(stderr, "Error: Filename '%s' missing extension\n", argv[i]);
      return -1;
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
      fprintf(stderr, "Error: File '%s' is of invalid type.\n", extension);
      fprintf(stderr, "Valid file types are: ");
      for (int j = 0; j < 3; j++) {
        fprintf(stderr, "%s, ", valid_extensions[j]);
      }
      fprintf(stderr, "%s\n", valid_extensions[3]);
      return -1;
    }
  }

  return 0;
}

int validate_file_readability(int argc, char* argv[]) {
  for (int i = 1; i < argc; i++) {
    char* current_file = argv[i];
    if (access(current_file, R_OK) != 0) {
      fprintf(stderr, "Error: Read access not permitted to file '%s'\n", current_file);
      return -1;
    }
  }
  return 0;
}

int validate_write_access(char* directory) {
  if (access(directory, W_OK) != 0) {
    fprintf(stderr, "Error: Write access to directory %s not granted.\n", directory);
    return -1;
  }

  return 0;
}

int validate_params(int argc, char *argv[], char* working_directory) {
  int status;
  
  // Ensure user passes at least one parameter
  status = validate_param_count(argc, argv[0]);
  if (status != 0) {
    return status;
  }
  
  // Ensure all files have valid extension
  status = validate_file_extensions(argc, argv);
  if (status != 0) {
    return status;
  }
  
  // Ensure all parameters correspond to valid files that we have read access to. This includes expanding wildcard expressions.
  status = validate_file_readability(argc, argv);
  if (status != 0) {
    return status;
  }

  // Ensure write access to working directory
  status = validate_write_access(working_directory);
  
  return status;
}
