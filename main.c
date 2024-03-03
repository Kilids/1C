// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 256
#define MAX_KEY_LENGTH 7

// Error handling macros
#define CHECK_MALLOC(ptr) do { if ((ptr) == NULL) { fprintf(stderr, "Error: Memory allocation failed!\n"); exit(1); } } while (0)
#define CHECK_FOPEN(fp) do { if ((fp) == NULL) { fprintf(stderr, "Error: File open failed!\n"); exit(1); } } while (0)

// Custom deleter for allocated memory
void delete_key_data(void* data) {
    free(data);
}

// Function to generate a unique key
char* generate_key() {
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* key = malloc(sizeof(char) * MAX_KEY_LENGTH);
    CHECK_MALLOC(key);
    for (int i = 0; i < MAX_KEY_LENGTH - 1; i++) {
        key[i] = alphabet[rand() % strlen(alphabet)];
    }
    key[MAX_KEY_LENGTH - 1] = '\0';
    return key;
}

// Data structure for storing key entries
typedef struct KeyEntry {
    char name[MAX_NAME_LENGTH];
    char* key;
} KeyEntry;

// Function to add a new entry to the list
void add_entry(KeyEntry* entries, int* entries_size, const char* name) {
    if (*entries_size >= 11) {
        return;
    }

    // Allocate memory for the new entry
    KeyEntry* new_entry = malloc(sizeof(KeyEntry));
    CHECK_MALLOC(new_entry);

    // Copy name and generate key
    strcpy(new_entry->name, name);
    new_entry->key = generate_key();

    // Add the new entry to the list
    entries[*entries_size] = *new_entry;
    (*entries_size)++;
}

// Function to display the history of entries
void display_history(const KeyEntry* entries, int entries_size) {
    for (int i = 0; i < entries_size; i++) {
        printf("Name: %s\n", entries[i].name);
        printf("Key: %s\n\n", entries[i].key);
    }
}

// Function to write the history to a file
void write_history_to_file(const KeyEntry* entries, int entries_size) {
    FILE* fp = fopen("history.txt", "w");
    CHECK_FOPEN(fp);

    for (int i = 0; i < entries_size; i++) {
        fprintf(fp, "%s,%s\n", entries[i].name, entries[i].key);
    }

    fclose(fp);
}

int main() {
    // Initialize entries list
    KeyEntry entries[11];
    int entries_size = 0;

    char name[MAX_NAME_LENGTH];
    while (1) {
        printf("Введите имя (или \"выйдите\" для завершения): ");
        scanf("%s", name);

        if (strcmp(name, "выход") == 0) {
            break;
        }

        int found = 0;
        for (int i = 0; i < entries_size; i++) {
            if (strcmp(name, entries[i].name) == 0) {
                found = 1;
                printf("Ключ для %s: %s\n", name, entries[i].key);
                break;
            }
        }

        if (!found) {
            add_entry(entries, &entries_size, name);
            printf("Ключ для %s сгенерирован и добавлен в историю.\n", name);
        }
    }

    // Write history to file
    write_history_to_file(entries, entries_size);

    // Free allocated memory
    for (int i = 0; i < entries_size; i++) {
        free(entries[i].key);
    }

    return 0;
}
