#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char** argv) {

    int file = open(argv[1], O_RDONLY);

    if (file == -1) {
        return -1;
    }

    struct stat filestat;
    int res = fstat(file, &filestat);

    if (res == -1) {
        return -1;
    }

    printf("The specified file's size is %ld\n", filestat.st_size);
    printf("The file's first 8 bytes are:\n");
    
    char bytes[8];
    read(file, bytes, 8);

    for (int i = 0; i < 8; i++) {
        printf("   %x\n", bytes[i]);
    }

    close(file);

    return 0;

}
