#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define ZIP_FILE "file.zip"

void *brute_mixed(void *arg) {
    char pass[4], command[100];
    for (char c1 = 'a'; c1 <= 'z'; c1++) {
        for (char c2 = 'a'; c2 <= 'z'; c2++) {
            for (char c3 = '0'; c3 <= '9'; c3++) {  // Last character is a number
                snprintf(pass, 4, "%c%c%c", c1, c2, c3);
                snprintf(command, sizeof(command), "unzip -o -P %s %s > /dev/null 2>&1", pass, ZIP_FILE);
                printf("Trying: %s\n", pass);
                if (system(command) == 0) {
                    printf("\n[+] Password found: %s\n", pass);
                    exit(0);
                }
            }
        }
    }
    return NULL;
}

int main() {
    pthread_t t1;
    pthread_create(&t1, NULL, brute_mixed, NULL);
    pthread_join(t1, NULL);
    printf("\n[-] Password not found!\n");
    return 0;
}
