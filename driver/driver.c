#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./validate_params.h"
#include "../photo_lib/photo_lib.h"


int main(int argc, char *argv[]) {
    // Validate parameters
    int params_status = validate_params(argc, argv);
    if (params_status != 0) {
      printf("oops");
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