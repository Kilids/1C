#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Массив имен
char *names[] = {"andrei", "ivan", "maria"};
// Алфавит
char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

// Функция генерации случайного символа
char generate_random_char() {
  return alphabet[rand() % strlen(alphabet)];
}

// Функция генерации ключа
char *generate_key(char *name) {
  char *key = malloc(sizeof(char) * 7); // 6 символов + '\0'
  for (int i = 0; i < 6; i++) {
    key[i] = generate_random_char();
  }
  key[6] = '\0';
  return key;
}

// Структура для хранения записи в буфере
typedef struct KeyEntry {
  char name[256];
  char key[7];
} KeyEntry;

// Буфер для хранения истории
KeyEntry buffer[10]; // Буфер на 10 записей
int buffer_size = 0; // Размер буфера

// Функция очистки буфера
void clear_buffer() {
  buffer_size = 0;
}

// Функция добавления записи в буфер
void add_to_buffer(char *name, char *key) {
  if (buffer_size < 10) {
    strcpy(buffer[buffer_size].name, name);
    strcpy(buffer[buffer_size].key, key);
    buffer_size++;
  }
}

// Функция записи истории в файл
void write_history_to_file() {
  FILE *fp = fopen("history.txt", "w");
  if (fp == NULL) {
    printf("Error opening history file!\n");
    return; // Возвращаемся из функции, чтобы не продолжать запись в файл
  }

  for (int i = 0; i < buffer_size; i++) {
    fprintf(fp, "%s,%s\n", buffer[i].name, buffer[i].key);
  }
  fclose(fp);
}

int main() {
  char name[256];

  // Цикл для ввода имен и генерации ключей
  while (1) {
    // Ввод имени
    printf("Enter a name (or \"exit\" to complete): ");
    scanf("%s", name);

    // Проверка команды выхода
    if (strcmp(name, "exit") == 0) {
      break;
    }

    // Поиск имени в массиве
    int found = 0;
    for (int i = 0; i < 10; i++) {
      if (strcmp(name, names[i]) == 0) {
        found = 1;
        break;
      }
    }

    // Генерация ключа
    if (found) {
      char *key = generate_key(name);
      printf("Ключ для %s: %s\n", name, key);

      // **Исправление:** освобождение памяти после использования ключа
      free(key);

      // Добавление записи в буфер
      add_to_buffer(name, key);

      // Запись истории в файл
      write_history_to_file();
    } else {
      printf("Имя не найдено.\n");
    }
  }

  return 0;
}