#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#define MAXN 1024

int main(int argc, char *argv[])
{
    // invalid arguments
    if (argc < 2){
        fprintf(2, "usage: xargs command\n");
        exit(1);
    }

    char *buf_argv[MAXARG];
    for (int i = 1; i < argc; i++)
        buf_argv[i - 1] = argv[i];
    char buf[MAXN];
    char c = 0;
    int stat = 1;

    while (stat){
        int buf_count = 0;
        int arg_begin = 0;
        int argv_count = argc - 1;
        
	// reading command line arguments
	while (1){
            stat = read(0, &c, 1);
            if (stat == 0)
                exit(0);
            if (c == ' ' || c == '\n'){
                buf[buf_count++] = 0;
                buf_argv[argv_count++] = &buf[arg_begin];
                arg_begin = buf_count;
                if (c == '\n')
                    break;
            } else{
                buf[buf_count++] = c;
            }
        }

        // executing established commands
        buf_argv[argv_count] = 0;
        if (fork() == 0){
            exec(buf_argv[0], buf_argv);
        } else{
            // wait for child if parent
            wait(0);
        }
    }
    exit(0);
}
