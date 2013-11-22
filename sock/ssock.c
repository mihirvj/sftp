/*****************************************************
                       ssock.c
           server socket implementation
       Authors: Mihir Joshi, Fenil Kavathia
                     csc573 NCSU
*****************************************************/

#include "ssock.h"

void bind_sock(int sock, int port)
{
 int opt = 1;
 struct sockaddr_in addr;

 bzero((char *) &addr, sizeof(addr));

 addr.sin_family = AF_INET;
 addr.sin_addr.s_addr = INADDR_ANY;
 addr.sin_port = htons(port);

 setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(int));

 if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
   error("error while binding");
}

void listen_sock(int sock)
{
 listen(sock, MAX_CONNECTIONS);
}

