#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./validate_params.h"
#include "../photo_lib/photo_lib.h"


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

    // Fork to handle gathering captions

    // Fork to handle generating thumbnails

    // Fork to handle generating medium-sized
    
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