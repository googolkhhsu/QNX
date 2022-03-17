#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "inc/my_devctl.h"
#include "inc/powertrain.h"
#include "inc/log.h"

POWERTRAIN_T powertrain_t;

int main(void)
{
    int testdev;
    int i, rf = 0, cnt = 100;
    char buf[15];

    while (1)
    {
        memset(buf, 0, sizeof(buf));
        testdev = open("/dev/powertrain", O_RDWR);
        if (testdev == -1)
        {
            perror("open\n");
            exit(0);
        }

        // char log[50];
        // rf = read(testdev, buf, 12);
        // if (rf < 0)
        //     perror("read error\n");
        // // printf("R:%s, %d\n", buf, cnt);
        // snprintf(log, 50, "R:%s, %d", buf, cnt);
        // dlog(log);
        // DLOG2("%s, %d", log, 10);

        memset(&powertrain_t, 0, sizeof(POWERTRAIN_T));
        rf = devctl(testdev, MY_DEVCTL_GETVAL, &powertrain_t, sizeof(POWERTRAIN_T), NULL);
        printf("GET ret = %d, powertrain_t{%ld, %d, %d}\n", rf, powertrain_t.odometer,
               powertrain_t.speedometor, powertrain_t.accelerate);

        close(testdev);
    }
    return 0;
}