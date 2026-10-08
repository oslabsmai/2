#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
  const char *child = (argc > 1) ? argv[1] : "./child";
  char *line = NULL;
  size_t cap = 0;
  ssize_t n;
  int p1[2], p2[2], fd;
  pid_t pid;
  FILE *to_child, *from_child;

  fputs("file name: ", stdout);
  n = getline(&line, &cap, stdin);
  if (n <= 0) {
    fputs("empty file name\n", stderr);
    return 1;
  }
  if (line[n - 1] == '\n')
    line[n - 1] = '\0';

  fd = open(line, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror(line);
    return 1;
  }

  if (pipe(p1) < 0 || pipe(p2) < 0) {
    perror("pipe");
    return 1;
  }

  pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    char arg[16];

    dup2(p1[0], STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(fd);

    sprintf(arg, "%d", p2[1]);
    execl(child, child, arg, (char *)NULL);
    perror(child);
    _exit(127);
  }

  close(p1[0]);
  close(p2[1]);
  close(fd);

  to_child = fdopen(p1[1], "w");
  while ((n = getline(&line, &cap, stdin)) > 0)
    fputs(line, to_child);
  fclose(to_child);

  from_child = fdopen(p2[0], "r");
  while (getline(&line, &cap, from_child) > 0)
    printf("error from child: %s", line);
  fclose(from_child);

  free(line);
  wait(NULL);
  return 0;
}
