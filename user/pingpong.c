#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int to_parent[2];
  int to_child[2];

  if(pipe(to_parent) == -1){
    fprintf(2, "Pipe fail");
    exit(-1);
  }

  if(pipe(to_child) == -1){
    fprintf(2, "Pipe fail");
    exit(-1);
  }
  
  int pid = fork();
  
  if(pid < 0){
    fprintf(2, "fork fail");
    exit(-1);    
  }

  if(pid == 0){
    // child
    char received;
    read(to_child[0], &received, 1);
    printf("%d: received ping\n", getpid());
    write(to_parent[1], "x", 1);
  } else {
    // parent
    write(to_child[1], "b", 1);
    char received;
    read(to_parent[0], &received, 1);
    printf("%d: received pong\n", getpid());
  }
  exit(0);
}

