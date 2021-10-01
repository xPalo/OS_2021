#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *dir_name, char *file_name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // checking if directory "dir_name" exists
    if ((fd = open(dir_name, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", dir_name);
        return;
    }

    // saving retrieved information about the opened directory into "st"
    if (fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", dir_name);
        close(fd);
        return;
    }

    // checking if "dir_name" is indeed a directory
    if (st.type != T_DIR){
        fprintf(2, "find: %s is not a directory\n", dir_name);
        close(fd);
        return;
    }

    // too long directory name; exceeding length of "buf"
    if(strlen(dir_name) + 1 + DIRSIZ + 1 > sizeof buf){
        fprintf(2, "find: directory too long\n");
        close(fd);
        return;
    }

    // "dir_name" exists, copying into buf for a later use
    strcpy(buf, dir_name);
    p = buf+strlen(buf);
    *p++ = '/';

    // recursively finding directory, avoiding "." and ".."
    while (read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;
        if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;

        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        if (st.type == T_DIR){
            find(buf, file_name);
        }
        else if (st.type == T_FILE && !strcmp(de.name, file_name)){
            printf("%s\n", buf);
        }
    }
}

int main(int argc, char *argv[])
{
    // invalid arguments
    if (argc != 3){
        fprintf(2, "usage: find dir_Name file_Name\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}

