#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LEN 10

static int ask_name(char **name, size_t *cap, const char *prompt) {
  ssize_t n;

  fputs(prompt, stdout);
  fflush(stdout);
  n = getline(name, cap, stdin);
  if (n <= 0)
    return -1;
  if ((*name)[n - 1] == '\n')
    (*name)[n - 1] = '\0';
  return 0;
}

static pid_t spawn(const char *child, const char *name, int p[2], int q[2]) {
  pid_t pid;
  int fd;

  fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror(name);
    return -1;
  }

  pid = fork();
  if (pid < 0) {
    perror("fork");
    close(fd);
    return -1;
  }

  if (pid == 0) {
    dup2(p[0], STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    close(p[0]);
    close(p[1]);
    close(q[0]);
    close(q[1]);
    close(fd);

    execl(child, child, (char *)NULL);
    perror(child);
    _exit(127);
  }

  close(fd);
  return pid;
}

int main(int argc, char **argv) {
  const char *child = (argc > 1) ? argv[1] : "./child";
  char *line = NULL, *name1 = NULL, *name2 = NULL;
  size_t cap = 0, cap1 = 0, cap2 = 0;
  ssize_t n;
  int p1[2], p2[2];
  pid_t pid1, pid2;
  FILE *to_child1, *to_child2;

  if (ask_name(&name1, &cap1, "file name 1: ") < 0 ||
      ask_name(&name2, &cap2, "file name 2: ") < 0) {
    fputs("empty file name\n", stderr);
    return 1;
  }

  if (pipe(p1) < 0 || pipe(p2) < 0) {
    perror("pipe");
    return 1;
  }

  pid1 = spawn(child, name1, p1, p2);
  if (pid1 < 0)
    return 1;
  pid2 = spawn(child, name2, p2, p1);
  if (pid2 < 0) {
    close(p1[1]);
    waitpid(pid1, NULL, 0);
    return 1;
  }

  close(p1[0]);
  close(p2[0]);

  to_child1 = fdopen(p1[1], "w");
  to_child2 = fdopen(p2[1], "w");

  while ((n = getline(&line, &cap, stdin)) > 0) {
    if (line[n - 1] == '\n')
      line[--n] = '\0';

    if (strlen(line) > MAX_LEN)
      fprintf(to_child2, "%s\n", line);
    else
      fprintf(to_child1, "%s\n", line);
  }

  fclose(to_child1);
  fclose(to_child2);

  free(line);
  free(name1);
  free(name2);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  return 0;
}
