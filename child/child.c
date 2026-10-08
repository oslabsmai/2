#include <stdio.h>
#include <stdlib.h>

int main(void) {
  char *line = NULL;
  size_t cap = 0;
  ssize_t n, i;

  while ((n = getline(&line, &cap, stdin)) > 0) {
    if (line[n - 1] == '\n')
      line[--n] = '\0';

    for (i = n - 1; i >= 0; i--)
      putchar(line[i]);
    putchar('\n');
  }

  free(line);
  return 0;
}
