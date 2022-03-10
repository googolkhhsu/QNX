#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int testdev;
    int i, rf = 0;
    char buf[15] = "abcdefg";
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s message...\n", argv[0]);
        exit(1);
    }

    // memset(buf, 0, sizeof(buf));
    testdev = open("/dev/powertrain", O_RDWR);
    if (testdev == -1)
    {
        perror("open\n");
        exit(0);
    }

    for (int i = 1; i < argc; i++)
    {
        rf = write(testdev, argv[i], sizeof argv[i]);
        if (rf < 0)
            perror("write error\n");
        printf("W:%s\n", argv[i]);
    }
    close(testdev);
    return 0;
}