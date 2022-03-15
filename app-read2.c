#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "inc/log.h"

int main(void)
{
    int testdev;
    int i, rf = 0, cnt = 100;
    char buf[15];

    while (cnt--)
    {
        memset(buf, 0, sizeof(buf));
        testdev = open("/dev/powertrain", O_RDWR);
        if (testdev == -1)
        {
            perror("open\n");
            exit(0);
        }

        char log[50];
        rf = read(testdev, buf, 12);
        if (rf < 0)
            perror("read error\n");
        // printf("R:%s, %d\n", buf, cnt);
        snprintf(log, 50, "R:%s, %d", buf, cnt);
        dlog(log);
        DLOG2("%s, %d", log, 10);
        close(testdev);
    }
    return 0;
}