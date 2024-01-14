#include <stdio.h>

int main(int argc, char *argv[]) {
    // Ensure user passes at least one parameter
    if (argc == 1) {
      printf("Incorrect Usage. Please pass raw images filenames. Space-separated filenames or wildcard notation are acceptable.\n");
      printf("Correct usage: %s <images>\n", argv[0]);
      return 1;
    }
    
    // Iterate over provided photos
    // For now: assume no wildcard expressions
    for (int i = 1; i < argc; i++) {
      char* filename = *(&argv[i]);
      printf("%s\n", filename);
    }
    
    

    
    
    // char userInput[100]; // Assuming a maximum input length of 100 characters

    // printf("Enter something: ");
    // fgets(userInput, sizeof(userInput), stdin);

    // printf("You entered: %s", userInput);

    return 0;
}
