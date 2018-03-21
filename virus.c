
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

int main(int argc, char** argv) {
    
    // Copy Host to /tmp/host.ruid
    // check if host exists there already

    int fd = open(argv[1])

    // delete if so

    // else create the file and execute it

}
