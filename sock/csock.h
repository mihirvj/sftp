#include "data.h"
#include<netdb.h>
#include<sys/ioctl.h>
#include<net/if.h>
#include<string.h>

void bind_sock(int sock, int port, int timeout);
void read_my_ip(char ip[50]);
