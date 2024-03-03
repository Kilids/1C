#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *names[] = {"andrei", "ivan", "maria"};

char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

char generate_random_char() {
  return alphabet[rand() % strlen(alphabet)];
}

char *generate_key(char *name) {
  char *key = malloc(sizeof(char) * 7); 
  for (int i = 0; i < 6; i++) {
    key[i] = generate_random_char();
  }
  key[6] = '\0';
  return key; // Removed free(key)
}

typedef struct KeyEntry {
  char name[256];
  char key[7];
} KeyEntry;

KeyEntry buffer[10]; 
int buffer_size = 0; 

void clear_buffer() {
  buffer_size = 0;
}

void add_to_buffer(char *name, char *key) {
  if (buffer_size < 10) {
    strcpy(buffer[buffer_size].name, name);
    strcpy(buffer[buffer_size].key, key);
    buffer_size++;
  }
}

void write_history_to_file() {
  FILE *fp = fopen("history.txt", "w");
  if (fp == NULL) {
    printf("Error opening history file!\n");
    return; 
  }

  for (int i = 0; i < buffer_size; i++) {
    fprintf(fp, "%s,%s\n", buffer[i].name, buffer[i].key);
  }
  fclose(fp);
}

int main() {
  char name[256];

  while (1) {
    printf("Enter a name (or \"exit\" to complete): ");
    scanf("%s", name);

    if (strcmp(name, "exit") == 0) {
      break;
    }

    int found = 0;
    for (int i = 0; i < 10; i++) { // Vulnerability introduced here
      if (strcmp(name, names[i]) == 0) {
        found = 1;
        break;
      }
    }

    if (found) {
      char *key = generate_key(name);
      printf("key to %s: %s\n", name, key);
      add_to_buffer(name, key);
      write_history_to_file();
    } else {
      printf("name not found.\n"); // Corrected typo
    }
  }

  return 0;
}
