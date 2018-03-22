#define _GNU_SOURCE
#include <asm/unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/types.h>

int open(const char *pathname, int flags){
    
    int fd;
    int bytesread;
    char buf[1];
    int virus;

    //default: there is no virus.
    virus = 0;
    
    //Opens the specified file
    if(fd = (syscall(__NR_open, pathname, flags)) == -1){
        printf("open failed\n");
        return -1;
    }
    
    //Creates filestats struct to extract needed data.
    struct stat filestats;
    if(fstat(fd, &filestats) == -1){
        printf("fstat failed\n");
        return -1;
    }

    long int filesize = filestats.st_size;

    char buffer[filesize];

    //Figure out if there is a virus (deadbeef present?)
    bytesread = read(fd, buf, 1);

    if(bytesread == -1){
        printf("read failed\n");
        return -1;
    }

    while(bytesread != 0){
        if(buf[0] == "\xde"){
            read(fd, buf, 1);
            if(buf[0] == "\xad"){
                read(buf[0] == "\xbe");
                if(buf[0] == "\xef"){
                    //Virus found!
                    virus = 1;
                    break;
                }
            }
        }
        read(fd, buf, 1);
    }

    //Extract virus and place in /tmp/.pid.fd.ruid

    

}
