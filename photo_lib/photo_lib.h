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