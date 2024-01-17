// Code to collect input from the user
// Caller provides
//  - message: prompt to display to console
//  - buffer: character array to store the string. Caller is required to malloc and free this memory.
//  - len: maximimum length of the input. Make sure buffer mallocs at least this much memory.
// Written by: adam salem, s.w. smith
// updated sep 2020
int input_string(char *message, char *buffer, int len);

// input_string wrapper to get rotation input (either cw, ccw, or no) from user
// Writes this input to pipe, who's write file descriptor is passed as second argument
// Asks user for input until response matches cw, ccw, or no
int get_user_input_for_rotation_and_write_to_pipe(char* input_filename, int rotate_write_fd);

// input_string wrapper to get caption input from user
// Writes this input to pipe, who's write file descriptor is passed as second argument
int get_user_input_for_caption_and_write_to_pipe(char* input_filename, int caption_write_fd);