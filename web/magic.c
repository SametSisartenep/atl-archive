#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void
sysfatal(char *s)
{
        perror(s);
        exit(1);
}

void
usage(void)
{
        fprintf(stderr, "usage: magic file\n");
        exit(1);
}

int
main(int argc, char *argv[])
{
        int f;

        if(argc < 2)
                usage();

        f = open(argv[1], O_RDONLY);
        if(f < 0)
                sysfatal("open");
        switch(fork()){
        case -1: sysfatal("fork");
        case 0:
                dup2(f, 0);
                execl("/usr/bin/file", "file", "-i", "-", NULL);
                sysfatal("exec");
        default:
                wait(NULL);
        }
        close(f);
        exit(0);
}
