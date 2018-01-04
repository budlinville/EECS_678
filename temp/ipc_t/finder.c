#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BSIZE 256

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"

int main(int argc, char *argv[]) {
  int status;
  pid_t pid_1, pid_2;
/*
  if (argc != 4) {
    printf("usage: finder DIR STR NUM_FILES\n");
    exit(0);
  }
*/
  //declare pipes file descriptors
  int p1[2];

  if (pipe(p1) < 0) {
    fprintf(stderr, "\nError creating pipes. ERROR%d\n", errno);
    return EXIT_FAILURE;
  }

  pid_1 = fork();
  if (pid_1 == 0) {
    //First Child

    close(p1[0]);
    dup2(p1[1], STDOUT_FILENO);
    //close(p2[0]);
    //close(p2[1]);

    execl("/bin/bash", "bash", "ls", "-ll", (char*) NULL);

    exit(0);
  }


  pid_2 = fork();
  if (pid_2 == 0) {
    //First Child

    dup2(p1[0], STDIN_FILENO);
    close(p1[1]);
    //close(p2[0]);
    //close(p2[1]);

    char* myArgs[] = {HEAD_EXEC, "--lines", "10", (char*) 0};
    if ((execv(HEAD_EXEC, myArgs)) < 0) {
      fprintf(stderr, "\nError execing sort. ERROR%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  close(p1[0]);
  close(p1[1]);

  if ((waitpid(pid_1, &status, 0)) == -1) {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  } if ((waitpid(pid_2, &status, 0)) == -1) {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  return(0);
}


/*
execl (“/bin/bash”, “bash”, “-c”, “ls -ll”, (char *)NULL);
	execl (“/bin/ls”, "ls", "-ll", (char *)NULL);

*/
