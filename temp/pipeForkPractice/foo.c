#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  /*char* args[3];

  string ls = "ls";
  args[0] = (char*)ls.c_str();
  args[1] = NULL;*/

  int bufSize = 20;

  char buf[bufSize];
  int fileDesc[2];
  pipe(fileDesc);


  pid_t pid = fork();

  if (pid < 0) {
    //error creating fork
    perror("ERROR: ERROR CREATING FORK");
  } else if (pid == 0) {
    //Child
    printf("CHILD\n");
    write(fileDesc[1], "HELLO PIPE!", 11);
    close(fileDesc[0]);

  } else {
    //parent
    wait(0);
    printf("PARENT\n");
    read(fileDesc[0], buf, 15);
    printf("buf: %s\n", buf);
  }

  return 0;
}
