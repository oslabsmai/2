#include <stdio.h>
#include <stdlib.h>

static int starts_with_upper(const unsigned char *s) {
  if (s[0] >= 'A' && s[0] <= 'Z')
    return 1;
  return s[0] == 0xD0 && ((s[1] >= 0x90 && s[1] <= 0xAF) || s[1] == 0x81);
}

int main(int argc, char **argv) {
  char *line = NULL;
  size_t cap = 0;
  ssize_t n;
  FILE *err;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <fd>\n", argv[0]);
    return 2;
  }

  err = fdopen(atoi(argv[1]), "w");
  if (err == NULL) {
    perror("fdopen");
    return 1;
  }

  while ((n = getline(&line, &cap, stdin)) > 0) {
    if (line[n - 1] == '\n')
      line[--n] = '\0';

    if (starts_with_upper((const unsigned char *)line))
      printf("%s\n", line);
    else
      fprintf(err, "\"%s\" is not a proper name\n", line);
  }

  free(line);
  fclose(err);
  return 0;
}
