#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("%d", argc);
    
    
    char userInput[100]; // Assuming a maximum input length of 100 characters

    printf("Enter something: ");
    fgets(userInput, sizeof(userInput), stdin);

    printf("You entered: %s", userInput);

    return 0;
}
