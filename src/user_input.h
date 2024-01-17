int get_user_input_for_rotation_and_write_to_pipe(char* input_filename, int rotate_write_fd);

int get_user_input_for_caption_and_write_to_pipe(char* input_filename, int caption_write_fd);

// code to collect input from the user

// sometimes, when you want both single-char input and longer text input,
// using scanf and getc and such, one fetch may leave unconsumed characters
// in stdin that confuses subsequent fetches.  
// trailing newlines also can cause trouble.
//
// should these things cause you problems in proj1, this code may
// prove helpful

// adam salem, s.w. smith
// updated sep 2020
int input_string(char *message, char *buffer, int len);