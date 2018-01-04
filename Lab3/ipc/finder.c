#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BSIZE 256

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/usr/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/usr/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"

int main(int argc, char *argv[]) {
  int status;
  pid_t pid_1, pid_2, pid_3, pid_4;

  if (argc != 4) {
    printf("usage: finder DIR STR NUM_FILES\n");
    exit(0);
  }

  //declare pipes file descriptors
  int p1[2], p2[2], p3[2];
  //pipe(p1);
  //pipe(p2);
  //pipe(p3);

  if (pipe(p1) < 0 || pipe(p2) < 0 || pipe(p3) < 0) {
    fprintf(stderr, "\nError creating pipes. ERROR%d\n", errno);
    return EXIT_FAILURE;
  }

  pid_1 = fork();
  if (pid_1 == 0) {
    //First Child
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    sprintf(cmdbuf, "%s %s -name \'*\'.[ch]", FIND_EXEC, argv[1]);

    close(p1[0]);
    dup2(p1[1], STDOUT_FILENO);
    close(p2[0]);
    close(p2[1]);
    close(p3[0]);
    close(p3[1]);

    char* myArgs[] = {"BASH_EXEC", "-c", cmdbuf, (char*) 0};
    if ((execv(BASH_EXEC, myArgs)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  pid_2 = fork();
  if (pid_2 == 0) {
    /* Second Child */

    dup2(p1[0], STDIN_FILENO);
    close(p1[1]);
    close(p2[0]);
    dup2(p2[1], STDOUT_FILENO);
    close(p3[0]);
    close(p3[1]);

    char* myArgs[] = {XARGS_EXEC, GREP_EXEC, "-c", argv[2], (char*) 0};
    if ((execv(XARGS_EXEC, myArgs)) < 0) {
      fprintf(stderr, "\nError execing grep. ERROR%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  //working on
  pid_3 = fork();
  if (pid_3 == 0) {
    /* Third Child */

    close(p1[0]);
    close(p1[1]);
    dup2(p2[0], STDIN_FILENO);
    close(p2[1]);
    close(p3[0]);
    dup2(p3[1], STDOUT_FILENO);

    char* myArgs[] = {SORT_EXEC, "-t", ":", "+1.0", "-2.0", "--numeric", "--reverse", (char*) 0};
    if ((execv(SORT_EXEC, myArgs)) < 0) {
      fprintf(stderr, "\nError execing sort. ERROR%d\n", errno);
      return EXIT_FAILURE;
    }
    exit(0);
  }

  pid_4 = fork();
  if (pid_4 == 0) {
    /* Fourth Child */

    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);
    dup2(p3[0], STDIN_FILENO);
    close(p3[1]);

    char* myArgs[] = {HEAD_EXEC, "--lines", argv[3], (char*) 0};
    if ((execv(HEAD_EXEC, myArgs)) < 0) {
      fprintf(stderr, "\nError execing sort. ERROR%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  close(p1[0]);
  close(p1[1]);
  close(p2[0]);
  close(p2[1]);
  close(p3[0]);
  close(p3[1]);

  if ((waitpid(pid_1, &status, 0)) == -1) {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_2, &status, 0)) == -1) {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_3, &status, 0)) == -1) {
    fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_4, &status, 0)) == -1) {
    fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  return 0;
}
