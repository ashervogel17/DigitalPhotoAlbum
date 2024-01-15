#include <stdio.h>
#include "../photo_lib/photo_lib.h"


int main(int argc, char *argv[]) {
    // Ensure user passes at least one parameter
    if (argc == 1) {
      printf("Incorrect Usage. Please pass raw images filenames. Space-separated filenames and wildcard notation are acceptable.\n");
      printf("Correct usage: %s <images>\n", argv[0]);
      return 1;
    }
    
    // Iterate over provided photos
    // For now: assume individual files
    for (int i = 1; i < argc; i++) {
      char* filename = *(&argv[i]);
      printf("Calling display from main.\n");
      display(filename);
      printf("Done with call to display.\n");
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