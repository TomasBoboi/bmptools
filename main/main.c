#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../src/bmptools.h"

int main(int argc, char **argv)
{
    /* this is just a plyground to test the implementation / provide some examples in the future */
    int fd = open("output.bmp", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    bmp_WriteImage(fd, NULL, 31, 47, 4);
    close(fd);
    return 0;
}