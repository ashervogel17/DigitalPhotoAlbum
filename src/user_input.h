// Code to collect input from the user
// Caller provides
//  - message: prompt to display to console
//  - buffer: character array to store the string. Caller is required to malloc and free this memory.
//  - len: maximimum length of the input. Make sure buffer mallocs at least this much memory.
// Written by: adam salem, s.w. smith
// updated sep 2020
int input_string(char *message, char *buffer, int len);