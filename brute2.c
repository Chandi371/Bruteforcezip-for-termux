#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PASS_LEN 4

int try_password(const char *zip_file, const char *pass) {
    char command[150];
    sprintf(command, "unzip -o -P %s %s > /dev/null 2>&1", pass, zip_file);
    
    return (system(command) == 0);  // Returns 1 if successful
}

int main() {
    char zip_file[100];
    printf("Enter ZIP file name: ");
    scanf("%s", zip_file);

    if (access(zip_file, F_OK) == -1) {
        printf("Error: ZIP file not found!\n");
        return 1;
    }

    char attempt[MAX_PASS_LEN];  // Buffer for password

    printf("Starting brute-force attack on %s\n", zip_file);
    
    for (int i = 0; i <= 999; i++) {
        sprintf(attempt, "%03d", i);  // Convert number to 3-digit string
        
        printf("\rTrying password: %s", attempt);
        fflush(stdout);
        
        if (try_password(zip_file, attempt)) {
            printf("\nPassword found: %s\n", attempt);
            return 0;
        }

        usleep(10000); // Small delay (10ms)
    }

    printf("\nPassword not found!\n");
    return 1;
}
