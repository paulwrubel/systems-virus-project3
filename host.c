#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <time.h>

int main( int argc, char** argv){

    int fd;
    int check;
    time_t lastModified;

    struct stat filestats;

    fd = open( argv[1], O_RDWR);

    if(fd == -1){
        printf("Error: open failed\n");
        perror("open");
        return -1;
    }

    check = fstat(fd, &filestats);

    if(check == -1){
        printf("Error: fstat failed\n");
        perror("fstat");
        return -1;
    }

    lastModified = filestats.st_mtime;
    
    printf("This file was last modified: %s", ctime(&lastModified));
    return 0;

}
