// Function to be used my driver.c's main()
// Pass argc and argv from main
// Returns 0 if all of the following are true
//  - argv is non-empty (argc > 1)
//  - all arguments end in an image file extension (.jpg, .jpeg, .png, .gif)
//  - all arguments correspond to valid files that the program has access to read from. Expands wildcard notation and checks all matches.
//  - write access is granted to working directory
// Returns -1 if any criterion is not met
int validate_params(int argc, char *argv[], char* working_directory);

// Helper function for validate_params
// Returns 0 if argc > 1
// Returns -1 otherwise
int validate_param_count(int argc, char* program);

// Helper function for validate_params
// Returns 0 if all arguments end in one of (.jpg, .jpeg, .png, .gif)
// Returns -1 otherwise
int validate_file_extensions(int argc, char* argv[]);

// Helper function for validate_params
// Returns 0 if all files that pattern-match an argument are accessible to read
// Returns -1 if any of the arguments can't be found in the file system or can't be read by the program
int validate_file_readability(int argc, char* argv[]);

// Helper function for validate_params
// Caller provides the path to a directory (can be relative)
// Returns 0 if the program has access to write to this directory
// Returns -1 if not
int validate_write_access(char* directory);