#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_PASS_LENGTH 10
#define DOWNLOADS_FOLDER "/storage/emulated/0/Download/"

// Function to check if a file exists
int file_exists(const char *file_path) {
    return access(file_path, F_OK) == 0;
}

// Function to check if a ZIP file is password protected
int is_password_protected(const char *zip_file) {
    char command[300];
    sprintf(command, "unzip -t \"%s\" 2>&1 | grep -q 'incorrect password'", zip_file);
    return (system(command) == 0);
}

// Function to attempt password cracking
int try_password(const char *zip_file, const char *pass) {
    char command[300];
    sprintf(command, "unzip -o -P %s \"%s\" > /dev/null 2>&1", pass, zip_file);
    return (system(command) == 0);
}

// Function to generate passwords recursively (Numbers, Letters, Mixed)
void generate_passwords(char *attempt, int index, int max_length, const char *zip_file, const char *charset) {
    if (index == max_length) {
        attempt[index] = '\0';
        printf("Trying: %s\r", attempt);
        fflush(stdout);
        if (try_password(zip_file, attempt)) {
            printf("\nPassword found: %s\n", attempt);
            exit(0);
        }
        return;
    }
    
    for (int i = 0; charset[i] != '\0'; i++) {
        attempt[index] = charset[i];
        generate_passwords(attempt, index + 1, max_length, zip_file, charset);
    }
}

// Function to brute-force ZIP file
void brute_force(const char *zip_file) {
    char attempt[MAX_PASS_LENGTH + 1];
    const char *charsets[] = { "0123456789", "abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyz0123456789" };
    
    printf("Starting brute-force attack on: %s\n", zip_file);
    
    for (int i = 1; i <= MAX_PASS_LENGTH; i++) {
        for (int j = 0; j < 3; j++) {
            printf("\nChecking passwords of length %d with charset: %s\n", i, charsets[j]);
            generate_passwords(attempt, 0, i, zip_file, charsets[j]);
        }
    }
    printf("\nPassword not found!\n");
}

// Function to scan Downloads folder for ZIP files
void scan_downloads_folder() {
    struct dirent *entry;
    DIR *dp = opendir(DOWNLOADS_FOLDER);
    if (dp == NULL) {
        printf("Error: Unable to open Downloads folder.\n");
        return;
    }
    
    printf("Scanning Downloads folder for ZIP files...\n");
    char found_files[50][256];
    int count = 0;
    
    while ((entry = readdir(dp)) != NULL) {
        if (strstr(entry->d_name, ".zip")) {
            snprintf(found_files[count], sizeof(found_files[count]), "%s%s", DOWNLOADS_FOLDER, entry->d_name);
            printf("[%d] %s\n", count + 1, entry->d_name);
            count++;
        }
    }
    closedir(dp);
    
    if (count == 0) {
        printf("No ZIP files found in Downloads.\n");
        return;
    }
    
    int choice;
    printf("\nSelect a file to brute-force (Enter number): ");
    scanf("%d", &choice);
    
    if (choice < 1 || choice > count) {
        printf("Invalid selection.\n");
        return;
    }
    
    char *selected_file = found_files[choice - 1];
    printf("\nChecking if %s is password protected...\n", selected_file);
    if (!is_password_protected(selected_file)) {
        printf("The selected file is not password protected.\n");
        return;
    }
    
    brute_force(selected_file);
}

int main() {
    scan_downloads_folder();
    return 0;
}
