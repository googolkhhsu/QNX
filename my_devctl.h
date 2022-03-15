#ifndef _MY_DEVCTL_H_
#define _MY_DEVCTL_H_

#include <devctl.h>

typedef union _my_devctl_msg
{
    int tx; /* Filled by client on send */
    int rx; /* Filled by server on reply */
} data_t;

#define MY_CMD_CODE 1
#define MY_DEVCTL_GETVAL __DIOF(_DCMD_MISC, MY_CMD_CODE + 0, int)
#define MY_DEVCTL_SETVAL __DIOT(_DCMD_MISC, MY_CMD_CODE + 1, int)
#define MY_DEVCTL_SETGET __DIOTF(_DCMD_MISC, MY_CMD_CODE + 2, union _my_devctl_msg)

#endif