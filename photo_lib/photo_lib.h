// Note: For all of these functions, the caller is expected to fork before calling, as they all exit at end of running.

// Caller provides path to the src file and dst file
// Function generates a copy of the image that's 10% of the size
// Returns 0 on success, -1 on failure
int generate_thumbnail(char* src_file, char* dst_file);

// Caller provides path to the src file and dst file
// Function generates a copy of the image that's 25% of the size
// Returns 0 on success, -1 on failure
int generate_medium_image(char* input_filename, char* medium_filename);

// Caller provides path to an image file
// Function displays the image
// Returns 0 on success, -1 on failure
int display(char* filen);

// Caller provides path to image and one of (cw, ccw, or no)
// Function rotates the image in place (90deg clockwise for cw, 90deg counter-clockwise for ccw, none for no)
// Returns 0 on success, -1 on failure
int rotate(char* file, char* rotation);
