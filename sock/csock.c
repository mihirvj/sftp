/*****************************************************
                       csock.c
           client socket implementation
       Authors: Mihir Joshi, Fenil Kavathia
                     csc573 NCSU
*****************************************************/

#include "csock.h"

void bind_sock(int sock, int port, int timeout)
{
 int opt = 1;
 struct sockaddr_in addr;
 struct timeval tv;

 tv.tv_sec = timeout; // second timeout on receive
 tv.tv_usec = 0;

 bzero((char *) &addr, sizeof(addr));

 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = htons(port);

 setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(int));

 setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof(struct timeval));

 if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
   error("error while binding");
}

void read_my_ip(char ip[50])
{
 int fd;
 struct ifreq ifr;
 struct sockaddr_in *client;

 fd = socket(AF_INET, SOCK_STREAM, 0);

 ifr.ifr_addr.sa_family = AF_INET;

 strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 close(fd);

 client = (struct sockaddr_in *) &ifr.ifr_addr;

 /* display result */
 sprintf(ip, "%d.%d.%d.%d", (int)(client->sin_addr.s_addr&0xFF),
    (int)((client->sin_addr.s_addr&0xFF00)>>8),
    (int)((client->sin_addr.s_addr&0xFF0000)>>16),
    (int)((client->sin_addr.s_addr&0xFF000000)>>24));
}
