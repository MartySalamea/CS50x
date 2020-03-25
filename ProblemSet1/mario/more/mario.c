#include <cs50.h>
#include <stdio.h>

void printHashes(length) {
    for (int hashes = 0; hashes < length; hashes++) {
        printf("#");
    }
}

// Print left side spaces
void printSpaces(length) {
    for (int spaces = 0; spaces < length; spaces++) {
        printf(" ");
    }
}

int main(void)
{
    int size = -1;

    // Request an appropriate size between 1 and 8
    while (size < 1 || size > 8) {
        size = get_int("Enter a size: ");
    }

    for(int i = 1; i <= size; i++) {

        // Print left side spaces
        printSpaces(size - i);

        // Print left side hashes
        printHashes(i);

        // Print a space in the middle
        printf(" ");

        // Print right side hashes
        printHashes(i);

        // End of line
        printf("\n");
    }
}
