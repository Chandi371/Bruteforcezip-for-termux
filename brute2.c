#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define ZIP_FILE "file.zip"
#define MAX_LENGTH 10
#define THREADS 4  // Adjust thread count as needed

const char *NUMBERS = "0123456789";
const char *LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *BOTH = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int found = 0;
pthread_mutex_t lock;

int try_password(const char *pass) {
    char command[200];
    sprintf(command, "unzip -o -P %s %s > /dev/null 2>&1", pass, ZIP_FILE);
    int result = system(command);
    return (result == 0);
}

void generate_and_test(const char *charset, char *attempt, int pos, int max_len) {
    if (found) return;
    if (pos == max_len) {
        attempt[pos] = '\0';
        pthread_mutex_lock(&lock);
        printf("Trying: %s\r", attempt);
        fflush(stdout);
        if (try_password(attempt)) {
            printf("\nPassword found: %s\n", attempt);
            found = 1;
        }
        pthread_mutex_unlock(&lock);
        return;
    }
    
    for (int i = 0; charset[i] != '\0' && !found; i++) {
        attempt[pos] = charset[i];
        generate_and_test(charset, attempt, pos + 1, max_len);
    }
}

void *brute_force(void *arg) {
    char attempt[MAX_LENGTH + 1];
    const char *charset = (const char *)arg;

    for (int len = 1; len <= MAX_LENGTH && !found; len++) {
        generate_and_test(charset, attempt, 0, len);
    }

    return NULL;
}

int main() {
    if (access(ZIP_FILE, F_OK) == -1) {
        printf("Error: %s not found!\n", ZIP_FILE);
        return 1;
    }

    printf("Checking if %s is password protected...\n", ZIP_FILE);
    if (system("unzip -t " ZIP_FILE " > /dev/null 2>&1") == 0) {
        printf("The ZIP file is not password protected.\n");
        return 0;
    }

    printf("Starting multi-threaded brute-force attack on %s\n", ZIP_FILE);

    pthread_t threads[THREADS];
    pthread_mutex_init(&lock, NULL);

    const char *charsets[] = {NUMBERS, LETTERS, BOTH};

    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, brute_force, (void *)charsets[i % 3]);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    if (!found) {
        printf("\nPassword not found!\n");
    }

    return 1;
}
