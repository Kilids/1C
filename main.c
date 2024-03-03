#include <stdio.h>
#include <stdlib.h>

int main() {
  char *filename = malloc(100);
  printf("Введите имя файла: ");
  scanf("%s", filename);

  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Файл '%s' не найден.\n", filename);
    return 1;
  }

  char data[1024];
  while (fgets(data, sizeof(data), file)) {
    printf("%s", data);
  }

  // Файл автоматически закроется при выходе из области видимости

  free(filename);

  return 0;
}
